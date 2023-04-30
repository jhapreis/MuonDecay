import numpy as np

from logmanager import LogManager


class ConvertValues:
    
    def __init__(self, encoder: str, channel_position: int, channel_scale: float, number_of_display_divisions: int = 10, **kwargs) -> None:
        
        log_level    = kwargs.get('log_level', 'warning')
        log_filename = kwargs.get('log_level')
        
        self.logmanager = LogManager(name="convert_values", level=log_level, filename=log_filename)
        
        self.encoder = encoder
        self.channel_position = channel_position
        self.channel_scale = channel_scale
        self.number_of_display_divisions = number_of_display_divisions

        self.__set_position()\
            .__set_encoder()
    
    
    def __set_position(self):
        
        self.position = {
            'min' : (-1)*self.number_of_display_divisions / 2,
            'max' :      self.number_of_display_divisions / 2
        }

        return self
        
    
    def __set_encoder(self):
        
        encoder_units = {
            "RPBINARY" : {
                'min' : 0,
                'max' : 255
            },
            
            "ASCII" : {
                'min' : -127,
                'max' : 128
            }
        }
        
        try:
            self.units = encoder_units[self.encoder.upper()]
        except KeyError:
            self.logmanager.exception(f"Could not interpret the given encoder: \'{self.encoder}\'")
            raise
            
        return self
    
    
    def waveform_to_mv(self, waveform_in_units: np.ndarray) -> np.ndarray:
        """
        The formula is something like V(v) = Delta_V / Delta_v * (v - v_min) + V_min
        """
        
        ratio = (self.position['max'] - self.position['min']) / (self.units['max'] - self.units['min'])

        y_pos = ratio*(waveform_in_units - self.units['min']) + self.position['min']

        y_pos -= self.channel_position # relative to Scope_ChannelPosition value

        waveform_mv = 1000 * self.channel_scale * y_pos # converts from position to mV, using y-scale value (in volts)

        return waveform_mv
    
    
    def volts_to_units(self, value_in_volts: float) -> float:
        """
        Conversion formula v(V) = Delta_v / Delta_V * (V - V_min) + v_min
        """
        ratio = (self.units['max'] - self.units['min']) / (self.position['max'] - self.position['min'])

        # y position on screen, relative to ChannelPosition
        y_pos = value_in_volts / self.channel_scale  +  self.channel_position

        # Convert value from position to units
        value_in_units = ratio*(y_pos - self.position['min']) + self.units['min']

        return value_in_units
