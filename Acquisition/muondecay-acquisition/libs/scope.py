import pandas as pd
from time import sleep
from typing import Union

from logmanager import LogManager
from instrument_connector import InstrumentConnector


class Scope:
    
    def __init__(self, instrument_connector: InstrumentConnector, parameters: 'dict[str]', **kwargs) -> None:
        
        self.__instrument_connector = instrument_connector
        self.__parameters = parameters
        
        self.__log_level    = kwargs.get('log_level', 'warning')
        self.__log_filename = kwargs.get('log_level')
        
        self.self.__logmanager = LogManager(name="scope", level=self.__log_level, filename=self.__log_filename)
    
    
    def query_idn(self) -> Union[str, None]:
        return self.__instrument_connector.query(command="*IDN?")
        
    
    def query_preamble(self) -> Union[str, None]:
        return self.__instrument_connector.query(command="WFMPre?")
    
    
    def set_values(self):
        sleep(2)
        
        self.__instrument_connector\
            .write(f"SELECT: {self.__parameters['ChannelName']} ON")\
            .write(f"DATa:SOUrce {self.__parameters['DataSource']}")\
            .write(f"{self.__parameters['ChannelName']}:SCAle {self.__parameters['ChannelScale']}")\
            .write(f"{self.__parameters['ChannelName']}:POSition {self.__parameters['ChannelPosition']}")\
            .write(f"{self.__parameters['ChannelName']}:PRObe {self.__parameters['ChannelProbe']}")\
            .write(f"HORizontal:DELay:SCAle {self.__parameters['HorizontalDelayScale']}")\
            .write(f"DATa:ENCdg {self.__parameters['DataEncodeFormat']}")\
            .write(f"DATa:ENCdg {self.__parameters['DataEncodeWidth']}")\
            .write(f"TRIGger:MAIn:LEVel {self.__parameters['ChannelTrigger']}")\
            .write(f"TRIGGER:MAIN:EDGE:SLOPE {self.__parameters['ChannelTriggerSlope']}")\
            .write(f"HORizontal:MAIn:SCAle {self.__parameters['ChannelHorizontalScale']}")\
            .write(f"HORizontal:MAIn:POSition {self.__parameters['ChannelHorizontalPosition']}")\
            .write(f"DISplay:PERSistence {self.__parameters['Persistence']}")
        
        sleep(2)

        return self


    def get_values(self):
        
        df = pd.DataFrame()
        
        channel = self.__parameters['ChannelName']
        
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
            
            query_result = self.__instrument_connector.query(command=queries[q])
            df[q] = [query_result]
            
            sleep(1)
        
        
        df.index = ['values']

        return df.T
