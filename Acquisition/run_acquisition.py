import os
import sys
import yaml
import pyvisa

import ROOT as root

from time import sleep, time
from datetime import datetime, timedelta
from array import array

from logmanager import LogManager
from read_waveform import WaveformData
from scope_parameters import ScopeParameters
from convert_values import ConvertValues
from instrument_connector import InstrumentConnector
from root_file import create_root_file


PATH_SOURCE      = ""
PATH_DESTINATION = ""

CFG_SCOPE        = "configs/scope.yaml"
CFG_ACQ          = "configs/acquisition.yaml"

LOG_LEVEL    = "info"
LOG_FILENAME = "./logs/run.log"

logmanager = LogManager(name="acquisition", level=LOG_LEVEL, filename=LOG_FILENAME)   



def main():
    
    usb_id = ""
    
    connector = InstrumentConnector(usb_id=usb_id)
        
    if not connector.test_connection():
        logmanager.exception("Could not open scope connection.")
        raise ConnectionError
    
    
    scope = connector.get_instrument()
    
    
    logmanager.info("Succesfully connected to the instrument...")


    trigger_volts = 1.E-3
    
    logmanager.info(f"Trigger value: {1000*trigger_volts} mV")
    
    converter = ConvertValues(
        encoder                    = "", 
        channel_position           = 0, 
        channel_scale              = 1., 
        number_of_display_divisions= 10, 
        log_level                  = LOG_LEVEL, 
        log_filename               = LOG_FILENAME
    )

    trigger_units = converter.volts_to_units(value_in_volts=trigger_volts)


    start_time = int( time() )
    
    now_time = int( time() )
    
    max_runtime_file = 3600
    
    number_good_samples = 0

    number_adchannels = 2500


    root_filename = f"{PATH_DESTINATION}/{int(now_time)}.root"

    root_file = create_root_file(
        path                  = root_filename, 
        arr_waveform_in_units = array('i', [0]*number_adchannels), 
        event_name            = array('i', [0])
    )


    logmanager.info("STARTING ACQUISITION")

    
    arr_waveform_in_units = array('i', [0]*number_adchannels)
    event_name = array('i', [0])


    while(True):

        if now_time - start_time > max_runtime_file:

            logmanager.info(f"Closing file. Total of events: {number_good_samples}")

            if root_file:
                root_file.Write()
                root_file.Close()

            logmanager.info("Creating new root file...")

            root_filename = f"{PATH_DESTINATION}/{int(time())}.root"
            
            root_file = create_root_file(
                path                  = root_filename, 
                arr_waveform_in_units = arr_waveform_in_units, 
                event_name            = event_name
            )
            
            start_time = int(time())

            continue


        waveform = WaveformData(instrument=scope, encoder="")
        
        try:
            waveform.read()
            
        except ValueError:
            logmanager.exception("Could not read waveform from oscilloscope.")
            sleep(2)
            continue
        
        else:
            
            now_time = int(time()) # update nowTime after getting a sample

            waveform.search_peaks(height=0, expected_peaks=0, pulse_width=0, waveform_size=number_adchannels)

            if not waveform.waveform_is_valid: 
                continue

            event_name[0] = now_time

            for i in range(number_adchannels):
                arr_waveform_in_units[i] = waveform.waveform[i]

            tree_waveforms.Fill()

            numberGoodSamples += 1


# ====================================================================================================
def main(path_to_save_files: str, config_file_path: str, scope_config_name: str, acquisition_config_name: str):

    try:

        with open(config_file_path, 'r') as f:

            configs: dict = yaml.safe_load(f)

            status = run(
                path_to_save_files       = path_to_save_files,
                max_runtime_file         = configs.get(acquisition_config_name).get("RunTimeMaximum"),
                ExpectedPeaks            = configs.get(acquisition_config_name).get("MinPeaks"),
                PulseWidth               = configs.get(acquisition_config_name).get("PulseWidth"),
                UsbInstrument            = configs.get(scope_config_name).get("USB_Instrument"),
                Encoder                  = configs.get(scope_config_name).get("DataEncodeFormat"),
                TriggerInVolts           = configs.get(scope_config_name).get("ChannelTrigger"),
                ChannelPosition          = configs.get(scope_config_name).get("ChannelPosition"),
                ChannelScale             = configs.get(scope_config_name).get("ChannelScale"),
                NumberOfDisplayDivisions = configs.get(scope_config_name).get("NumberOfDisplayDivisions"),
                numberADChannels         = configs.get(scope_config_name).get("NumberADChannels")
            )

            return status

    except yaml.YAMLError as e:

        logger.exception(f"Error opening the yaml config file \"{config_file_path}\".\n")

        return 1

    except FileNotFoundError as e:

        logger.exception(f"Could not locate the config file \"{config_file_path}\".\n")

        return 1

    return 1
# ====================================================================================================



if __name__ == "__main__":
    main()
