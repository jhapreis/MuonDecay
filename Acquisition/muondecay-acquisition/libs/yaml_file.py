import yaml

from logmanager import LogManager


class YAMLFile:
    
    def __init__(self, path: str, **kwargs) -> None:
        
        self.__log_level = kwargs.get("log_level", "warning")
        self.__filename  = kwargs.get("filename")
        
        self.__logger = LogManager(name="YAMLFile", level=self.__log_level, filename=self.__filename)
        
        self.path = path
        
    
    def read(self) -> dict:
        
        self.__logger.debug(f"Reading file at \'{self.path}\'")
        
        with open(self.path, 'r') as f:
            data = yaml.safe_load(f)
            return data
