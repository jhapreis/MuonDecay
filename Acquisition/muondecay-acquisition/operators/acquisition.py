import ROOT as root

from time import sleep, time
from array import array

from libs.logmanager import LogManager
from libs.read_waveform import WaveformData
from libs.scope import Scope
from libs.convert_values import ConvertValues
from libs.instrument_connector import InstrumentConnector
from libs.yaml_file import YAMLFile



class AcquisitionOperator:
    
    def __init__(self, path_destination: str, path_configs_acquisition: str, path_configs_scope: str, **kwargs) -> None:
        
        self.__log_level = kwargs.get("log_level", "warning")
        self.__log_filename = kwargs.get("log_filename")
        
        self.__logmanager = LogManager(name="Acquisition", level=self.__log_level, filename=self.__log_filename)
        
        self.__path_destination = path_destination
        self.__path_configs_acquisition = path_configs_acquisition
        self.__path_configs_scope = path_configs_scope
        
        self.number_good_samples = 0
        self.start_time = int(time())
    
    
    def read_configs(self):
        self.configs_acquisition = YAMLFile(path=self.__path_configs_acquisition, log_level=self.__log_level, log_filename=self.__log_filename).read()
        
        self.configs_scope = YAMLFile(path=self.__path_configs_scope, log_level=self.__log_level, log_filename=self.__log_filename).read()
        
        return self
    
    
    def connect_to_scope(self):
        usb_id = self.configs_scope['USB_Instrument']
        
        self.connector = InstrumentConnector(usb_id=usb_id)
        
        if not self.connector.test_connection():
            self.__logmanager.exception("Could not open scope connection.")
            raise ConnectionError
        
        self.scope = self.connector.__get_instrument()
        
        self.__logmanager.info("Succesfully connected to the instrument...")
        
        return self
    
    
    def convert_values(self):
        trigger_volts = self.configs_scope['ChannelTrigger']
        
        self.__logmanager.info(f"Trigger value: {1000*trigger_volts} mV")
        
        converter = ConvertValues(
            encoder                     = self.configs_scope['DataEncodeFormat'], 
            channel_position            = self.configs_scope['ChannelPosition'], 
            channel_scale               = self.configs_scope['ChannelScale'], 
            number_of_display_divisions = self.configs_scope['NumberOfDisplayDivisions'], 
            log_level                   = self.__log_level, 
            log_filename                = self.__log_filename
        )

        self.trigger_units = converter.volts_to_units(value_in_volts=trigger_volts)
        
        return self
    
    
    def create_root_file(self):
        
        self.__logmanager.info("Creating new root file...")
        
        number_adchannels = self.configs_scope['NumberADChannels']
        
        self.start_time = int(time())
        self.event_array = array('i', [0])
        self.waveform_in_units_array = array('i', [0]*number_adchannels)
        
        self.root_filename = f"{self.__path_destination}/{int(self.start_time)}.root"
        
        self.root_file = root.TFile(self.root_filename, "CREATE")
        
        self.tree_waveforms = root.TTree("tree_waveforms", "waveforms")
        
        self.tree_waveforms.Branch(
            "names", 
            self.event_array, 
            "name/I"
        )
        self.tree_waveforms.Branch(
            "waveforms", 
            self.waveform_in_units_array, 
            f"waveforms[{number_adchannels}]/I"
        )
        
        return self
    
    
    def close_root_file(self):
        
        self.__logmanager(f"Closing file. Total of events: {self.number_good_samples}")
        if self.root_file:
            self.root_file.Write()
            self.root_file.Close()
            
        return self
    
    
    def save_waveform(self):
        
        number_adchannels = self.configs_scope['NumberADChannels']

        self.__waveform.search_peaks(
            height         = self.trigger_units, 
            expected_peaks = self.configs_scope['MinPeaks'], 
            pulse_width    = self.configs_scope['PulseWidth'], 
            waveform_size  = number_adchannels
        )

        if not self.__waveform.waveform_is_valid: 
            return self

        self.event_array[0] = self.start_time

        for i in range(number_adchannels):
            self.waveform_in_units_array[i] = self.__waveform.waveform[i]

        self.tree_waveforms.Fill()

        self.number_good_samples += 1
        
        return self
    
    
    def execute(self):
        
        self.__logmanager.info("PREPARING EXECUTION")
        
        self.read_configs()\
            .connect_to_scope()\
            .convert_values()\
            .create_root_file()
        
        self.__logmanager.info("STARTING ACQUISITION")
        
        self.__waveform = WaveformData(instrument=self.scope, encoder=self.configs_scope['DataEncodeFormat'])
        
        while(True):
            now_time = int(time())
            
            if now_time - self.start_time > self.configs_acquisition['RunTimeMaximum']:
                
                self.close_root_file()\
                    .create_root_file()

                continue

            try:
                self.__waveform.read()
                
            except ValueError:
                self.__logmanager.exception("Could not read waveform from oscilloscope.")
                sleep(2)
                continue
            
            else:
                self.save_waveform()
