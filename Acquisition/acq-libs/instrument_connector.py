import pyvisa

from logmanager import LogManager
from scope_parameters import ScopeParameters


class InstrumentConnector:
    
    
    def __init__(self, ubs_id: str, **kwargs) -> None:
        
        self.__log_level    = kwargs.get('log_level', 'warning')
        self.__log_filename = kwargs.get('log_level')
        
        self.logmanager = LogManager(name="instrument_connector", level=self.__log_level, filename=self.__log_filename)
        self.usb_id = ubs_id
        self.instrument = self.connect()
        
    
    def connect(self) -> pyvisa.Resource:
        rm = pyvisa.ResourceManager()
        instrument = rm.open_resource(self.usb_id)

        return instrument


    def test_connection(self) -> bool:
        
        idn = ScopeParameters(instrument=self.instrument, parameters={}, log_level=self.__log_level, log_filename=self.__log_filename).query_idn()
        
        if idn is None:
            self.logmanager.warning(f"Could not connect to instrument: \'{self.usb_id}\'")
            return False
        
        self.logmanager.info(f"IDN: \n{idn}")
        
        return True
        
    
    def get_instrument(self) -> pyvisa.Resource:
        return self.instrument
