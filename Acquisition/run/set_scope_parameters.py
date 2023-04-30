import sys

from libs.logmanager import LogManager
from libs.instrument_connector import InstrumentConnector
from operators.scope_parameters import ScopeParametersOperator


if __name__ == "__main__":
    LOG_LEVEL = sys.argv[3]
    LOG_FILENAME = sys.argv[4]
    
    instrument_connector = InstrumentConnector()
    
    scope_parameters = ScopeParametersOperator(
        instrument_connector = ,
        path_configs_scope   = ,
        log_level            = LOG_LEVEL,
        log_filename         = LOG_FILENAME
    )
    
    scope_parameters.execute()
    
    