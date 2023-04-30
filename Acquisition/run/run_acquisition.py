import sys

from libs.logmanager import LogManager
from operators.acquisition import AcquisitionOperator


if __name__ == "__main__":
    
    
    PATH_DESTINATION = sys.argv[1] # "./"
    PATH_CFG_ACQ     = sys.argv[2] # "configs/acquisition.yaml"
    PATH_CFG_SCOPE   = sys.argv[3] # "configs/scope.yaml"
    LOG_LEVEL        = sys.argv[4] # "info"
    LOG_FILENAME     = sys.argv[5] # "./logs/run.log"
    
    
    logmanager = LogManager(name="main", level=LOG_LEVEL, filename=LOG_FILENAME)
    
    logmanager.info("Starting Execution...")
    
    operator = AcquisitionOperator(
        path_destination         = PATH_DESTINATION,
        path_configs_acquisition = PATH_CFG_ACQ,
        path_configs_scope       = PATH_CFG_SCOPE,
        log_level                = LOG_LEVEL,
        log_filename             = LOG_FILENAME
    )
    
    operator.execute()
    