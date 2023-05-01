from time import time

from libs.scope import Scope
from libs.yaml_file import YAMLFile
from libs.logmanager import LogManager
from libs.instrument_connector import InstrumentConnector


class ScopeParametersOperator:
    
    def __init__(self, path_destination: str, path_configs_scope: dict, **kwargs) -> None:
        self.__log_parameters = {
            "log_level"    : kwargs.get("log_level", "warning"),
            "log_filename" : kwargs.get("log_filename")
        }
        
        self.__logmanager = LogManager(name="Parameters", **self.__log_parameters)
        
        self.__path_destination   = path_destination
        self.__path_configs_scope = path_configs_scope
    
    
    def __read_configs(self):
        self.__logmanager.info(f"Reading configs at \'{self.__path_configs_scope}\'")
        self.__scope_configs = YAMLFile(path=self.__path_configs_scope, **self.__log_parameters).read()
        return self
    
    
    def __open_connection(self):
        self.__logmanager.info("Opening Scope Connection...")
        
        self.__instrument_connector = InstrumentConnector(ubs_id=self.__scope_configs['USB_Instrument'], **self.__log_parameters)

        self.__instrument_connector\
            .connect()\
            .test_connection()
            
        return self
    
    
    def execute(self):
        
        self.__read_configs()\
            .__open_connection()
        
        self.__logmanager.info("Setting Scope Parameters...")
        
        self.__read_configs()
        
        self.__scope = Scope(
            instrument_connector = self.__instrument_connector, 
            parameters           = self.__path_configs_scope, 
            log_level            = self.__log_level, 
            log_filename         = self.__log_filename
        )

        df = self.__scope\
            .set_values()\
            .get_values()
        
        filename = f"{self.__path_destination}/{int(time())}.csv"
        
        self.__logmanager.info(f"Saving Scope parameters to file \'{filename}\'")
        
        df.to_csv(filename, sep=",", header=True, index=True, index_label="index")

        self.__logmanager.info("Closing connection...")
        
        self.__instrument_connector.close()
        
        self.__logmanager.info("Done.")
