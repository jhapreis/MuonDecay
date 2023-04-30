import pyvisa
from typing import Union

from logmanager import LogManager


class InstrumentConnector:
    
    
    def __init__(self, ubs_id: str, **kwargs) -> None:
        
        self.__log_level    = kwargs.get('log_level', 'warning')
        self.__log_filename = kwargs.get('log_level')
        
        self.logmanager = LogManager(name="instrument_connector", level=self.__log_level, filename=self.__log_filename)
        self.usb_id = ubs_id
    
    
    def connect(self):
        
        self.rm = pyvisa.ResourceManager()
        self.instrument = self.rm.open_resource(self.usb_id)
        
        return self


    def test_connection(self):
        
        idn = self.query(command="*IDN?")
        
        if idn is None:
            self.logmanager.warning(f"Could not connect to instrument: \'{self.usb_id}\'")
            return self
        
        self.logmanager.info(f"IDN: \n{idn}")
        
        return self
    
    
    def close(self) -> None:
        self.instrument.close()
        self.rm.close()
        
        self.instrument = None
        self.rm = None
    
    
    def query(self, command: str) -> Union[str, None]:
        try:
            query = self.instrument.query(command)
            return query
            
        except Exception:
            self.logmanager.exception(f"Could not execute a query for the command: \'{command}\'")
            return None
    
    
    def write(self, command: str):
        try:
            self.instrument.write(command)
        except Exception:
            self.logmanager.exception(f"Could not write command: \'{command}\'")

        return self
