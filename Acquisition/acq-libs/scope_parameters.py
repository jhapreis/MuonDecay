import pyvisa
import pandas as pd
from typing import Union

from logmanager import LogManager


class ScopeParameters:
    
    def __init__(self, instrument: pyvisa.Resource, parameters: 'dict[str]', **kwargs) -> None:
        
        self.instrument = instrument
        self.parameters = parameters
        
        log_level    = kwargs.get('log_level', 'warning')
        log_filename = kwargs.get('log_level')
        
        self.logmanager = LogManager(name="scope_parameters", level=log_level, filename=log_filename)
    
    
    def uppercase_dictionary(self, dictionary: 'dict[str,str]'): 
        new_dict = dict([  (key.upper(), dictionary.get(key).upper()) for key in dictionary.keys()  ])
        
        return new_dict
    
    
    def query(self, command: str) -> Union[str, None]:
        try:
            query = self.instrument.query(command)
            return query
            
        except Exception:
            self.logmanager.exception(f"Could not execute a query for the command: \'{command}\'")
            return None
    
    
    def query_idn(self) -> Union[str, None]:
        return self.query(command="*IDN?")
        
    
    def query_preamble(self) -> Union[str, None]:
        return self.query(command="WFMPre?")
    
    
    def write(self, command: str):
        try:
            self.instrument.write(command)
        except Exception:
            self.logmanager.exception(f"Could not write command: \'{command}\'")

        return self
    
    
    def set_values(self):
        
        self\
            .write(f"SELECT: {self.parameters['ChannelName']} ON")\
            .write(f"DATa:SOUrce {self.parameters['DataSource']}")\
            .write(f"{self.parameters['ChannelName']}:SCAle {self.parameters['ChannelScale']}")\
            .write(f"{self.parameters['ChannelName']}:POSition {self.parameters['ChannelPosition']}")\
            .write(f"{self.parameters['ChannelName']}:PRObe {self.parameters['ChannelProbe']}")\
            .write(f"HORizontal:DELay:SCAle {self.parameters['HorizontalDelayScale']}")\
            .write(f"DATa:ENCdg {self.parameters['DataEncodeFormat']}")\
            .write(f"DATa:ENCdg {self.parameters['DataEncodeWidth']}")\
            .write(f"TRIGger:MAIn:LEVel {self.parameters['ChannelTrigger']}")\
            .write(f"TRIGGER:MAIN:EDGE:SLOPE {self.parameters['ChannelTriggerSlope']}")\
            .write(f"HORizontal:MAIn:SCAle {self.parameters['ChannelHorizontalScale']}")\
            .write(f"HORizontal:MAIn:POSition {self.parameters['ChannelHorizontalPosition']}")\
            .write(f"DISplay:PERSistence {self.parameters['Persistence']}")

        return self


    def get_values(self):
        
        df = pd.DataFrame()
        
        channel = self.scope['ChannelName']
        
        df['channel'] = [channel]
        
        queries = {
            'DataSource'             : 'DATa:SOUrce?',
            'HorizontalDelayScale'   : 'HORizontal:DELay:SCAle?',
            'HorizontalDelayPosition': 'HORizontal:DELay:SCAle?',
            'Encoder'                : 'DATa:ENCdg?',
            'Width'                  : 'DATa:WIDth?',
            'TriggerLevel'           : 'TRIGger:MAIn:LEVel?',
            'TriggerSlope'           : 'TRIGGER:MAIN:EDGE:SLOPE?',
            'HorizontalScale'        : 'HORizontal:MAIn:SCAle?',
            'HorizontalPosition'     : 'HORizontal:MAIn:POSition?',
            'Persistence'            : 'DISplay:PERSistence?',
            'ChannelScale'           : f'{channel}:SCAle?',
            'ChannelPosition'        : f'{channel}:POSition?',
            'ChannelProbe'           : f'{channel}:PRObe?'
        }

        for q in queries.keys():
            
            query_result = self.query(command=queries[q])
            df[q] = [query_result]
        
        df.index = ['values']

        return df.T
