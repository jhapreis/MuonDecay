import os
import sys
import yaml
import pyvisa
import logging

from time import time
from datetime import datetime, timedelta


from acq import scopeParameters as sp


# ====================================================================================================
def run(InstrumentParameters: dict, path_to_save_file: str = "./scope.csv"):

    try:

        usbID: str = InstrumentParameters["USB_Instrument"]

        logger.info(f"Opening session with the instrument \"{usbID}\"...")

        rm    = pyvisa.ResourceManager()

        scope = rm.open_resource(usbID)


        logger.info("Querying for an IDN...")

        idn = sp.query_idn(scope)

        if idn is True:

            logger.info("Succesfully connected to the instrument...")
        
        else:

            logger.info(f"Failed to connected to the instrument \"{usbID}\"...")

            return 1
        

        logger.info("Setting scope parameters...")

        status = sp.Set_Scope_Parameters(
            instrument = scope,
            scopeParam = InstrumentParameters,
        )

    except KeyError as ke:

        logger.exception(f"Could not locate the key \"{ke}\" in the given InstrumentParameters. \
            Please, check the config file.")
    
    except Exception as e:

        logger.exception("Error while trying to set the oscilloscope parameters.")

        return 1

    
    df = sp.Check_Scope_Parameters(scope)
    df.to_csv(f"{path_to_save_file}/scope_configs.csv")

    return status
# ====================================================================================================


# ====================================================================================================
def main(path_to_save_file: str, config_file_path: str, scope_config_name: str):

    try:

        with open(config_file_path, 'r') as f:

            configs: dict = yaml.safe_load(f).get(scope_config_name)

            status = run(
                InstrumentParameters = configs,
                path_to_save_file    = path_to_save_file
            )

            return status

    except yaml.YAMLError as e:

        logger.exception(f"Error opening the yaml config file \"{config_file_path}\".\n")

        return 1

    except FileNotFoundError as e:

        logger.exception(f"Could not locate the config file \"{config_file_path}\".\n")

        return 1

    return 1
# ====================================================================================================



# ====================================================================================================
if __name__ == "__main__":


    path_to_save  = sys.argv[1]
    log_file_name = f"{path_to_save}/scope.log"
    
    

    # -------------------- Log configuration -------------------- # 

    logger = logging.getLogger(__name__)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    logger.setLevel(logging.INFO)

    streamHandler = logging.StreamHandler(sys.stdout)
    streamHandler.setFormatter(formatter)
    logger.addHandler(streamHandler)

    if log_file_name.endswith('.log'):
        
        fileHandler = logging.FileHandler(log_file_name)
        fileHandler.setFormatter(formatter)
        logger.addHandler(fileHandler)
    
    else:

        logger.warning(f"Invalid name for the log file: {log_file_name}.")
    


    # -------------------- Run main -------------------- # 

    path_to_save: str
    subfolder: str
    startTime: float
    endTime: float


    startTime = time()


    logger.info(" =============== STARTING =============== ")

    status = main(path_to_save, *sys.argv[2:])

    logger.info(" =============== ENDING =============== ")


    endTime = time()

    logger.info(f"Time elapsed: {timedelta(seconds=endTime - startTime)}.")

    logger.info(f"Execution returned with status {status}.\n\n")
# ====================================================================================================
