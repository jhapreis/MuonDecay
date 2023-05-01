import sys

from libs.logmanager import LogManager
from operators.scope_parameters import ScopeParametersOperator


if __name__ == "__main__":
    
    PATH_DESTINATION = sys.argv[1]
    PATH_CFG_SCOPE   = sys.argv[2]
    LOG_LEVEL        = sys.argv[3]
    LOG_FILENAME     = sys.argv[4]
    
    log_parameters = {
        "log_level"    : LOG_LEVEL,
        "log_filename" : LOG_FILENAME
    }
    
    scope_parameters = ScopeParametersOperator(
        path_destination   = PATH_DESTINATION, 
        path_configs_scope = PATH_CFG_SCOPE, 
        **log_parameters
    )
    
    scope_parameters.execute()
