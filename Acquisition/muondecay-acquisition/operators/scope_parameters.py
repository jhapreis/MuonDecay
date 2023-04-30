from time import time

from libs.logmanager import LogManager
from libs.instrument_connector import InstrumentConnector
from libs.scope import Scope
from libs.yaml_file import YAMLFile


class ScopeParametersOperator:
    
    def __init__(self, instrument_connector: InstrumentConnector, path_destination: str, path_configs_scope: str, **kwargs) -> None:
        
        self.__log_level    = kwargs.get("log_level", "warning")
        self.__log_filename = kwargs.get("log_filename")
        
        self.__logmanager = LogManager(name="Parameters", level=self.__log_level, filename=self.__log_filename)
        
        self.instrument_connector = instrument_connector
        self.__path_destination   = path_destination
        self.__path_configs_scope = path_configs_scope
    
    
    def read_configs(self):
        self.__logmanager.info("Reading configs...")
        
        self.configs_scope = YAMLFile(path=self.__path_configs_scope, log_level=self.__log_level, log_filename=self.__log_filename).read()
        
        return self
    
    
    def execute(self):
        self.__logmanager.info("Setting Scope Parameters...")
        
        self.read_configs()
        
        self.__scope = Scope(
            instrument_connector = self.instrument_connector, 
            parameters           = self.configs_scope, 
            log_level            = self.__log_level, 
            log_filename         = self.__log_filename
        )

        df = self.__scope\
            .set_values()\
            .get_values()
        
        filename = f"{self.__path_destination}/{int(time())}.csv"
        
        self.__logmanager.info(f"Saving Scope parameters to file \'{filename}\'")
        
        df.to_csv(filename, sep=",", header=True, index=True, index_label="index")
