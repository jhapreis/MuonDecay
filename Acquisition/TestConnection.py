import sys
import yaml
import pyvisa
import logging

from acq.scopeParameters import query_idn



# ====================================================================================================
def run(config_file_path: str):

    usbID: str

    try:

        with open(config_file_path, 'r') as f:

            usbID = yaml.safe_load(f)["Scope_Parameters"]["USB_Instrument"]

            rm = pyvisa.ResourceManager()

            scope = rm.open_resource(usbID)

            status = query_idn(scope)

            if status: logger.info("Connection OK!")
                
            else: logger.info("Connection NOT OK!")

            scope.close()

            return 0
    
    except KeyError as ke:

        logger.exception(f"Could not locate the \"{ke}\" key.")

    except yaml.YAMLError as e:

        print(f"Error opening the yaml config file \"{config_file_path}\".\n")

        return 1

    except FileNotFoundError as e:

        print(f"Could not locate the config file \"{config_file_path}\".\n")

        return 1
# ====================================================================================================



# ====================================================================================================
def main(config_file_path: str):

    status = run(config_file_path)

    return status
# ====================================================================================================



# ====================================================================================================
if __name__ == "__main__":
    

    # -------------------- Log configuration -------------------- # 

    logger = logging.getLogger(__name__)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    logger.setLevel(logging.INFO)

    streamHandler = logging.StreamHandler(sys.stdout)
    streamHandler.setFormatter(formatter)
    logger.addHandler(streamHandler)    


    # -------------------- Run main -------------------- # 
    logger.info(" =============== STARTING =============== ")

    status = main(*sys.argv[1:])

    logger.info(" =============== ENDING =============== ")
# ====================================================================================================
