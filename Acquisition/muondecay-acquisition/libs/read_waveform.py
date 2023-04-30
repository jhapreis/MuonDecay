import pyvisa
import numpy as np

from logmanager import LogManager


class WaveformData:
    
    def __init__(self, instrument: pyvisa.Resource, encoder: str = "RPBinary", **kwargs) -> None:
        
        log_level    = kwargs.get('log_level', 'warning')
        log_filename = kwargs.get('log_level')
        
        self.logmanager = LogManager(name="waveform_data", level=log_level, filename=log_filename)
        
        self.instrument = instrument
        self.encoder = encoder
        self.waveform_is_valid = False
        
    
    def read(self):

        if self.encoder.upper() == "RPBINARY":
            self.waveform = np.array(  self.instrument.query_binary_values('CURVE?', datatype='B')  )

        elif self.encoder.upper() == "ASCII":
            self.waveform = np.array(  self.instrument.query_ascii_values('CURVE?')  )
        
        else:
            self.logmanager.critical(f"Could not interpret the encoder \'{self.encoder}\'")
            raise ValueError
        
        return self


    def search_peaks(self, height: float, expected_peaks: int, pulse_width: int, waveform_size: int=2500):
        
        self.number_of_peaks = 0
        self.waveform_is_valid = False
    
        if expected_peaks <= 0:
            self.waveform_is_valid = True
            return self

        while(index < waveform_size):

            if(self.number_of_peaks > expected_peaks): # more peaks than expected
                self.waveform_is_valid = False
                return self

            if(self.waveform[index] <= height): # if triggered
                self.number_of_peaks += 1
                index += pulse_width
                
            else:
                index += 1

        self.waveform_is_valid = True
        
        return self
