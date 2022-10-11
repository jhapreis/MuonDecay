import os
import sys
import yaml
import pyvisa
import logging

import ROOT as root

from time import sleep, time
from datetime import datetime, timedelta
from array import array


from acq import convertValues, searchPeaks, readwvf
from acq.scopeParameters import query_idn

# ====================================================================================================
def run(
    path_to_save_files: str,
    UsbInstrument: str,
    Encoder: str,
    TriggerInVolts: float,
    ExpectedPeaks: int,
    PulseWidth: int,
    ChannelPosition: float,
    ChannelScale: float,
    NumberOfDisplayDivisions: int = 10,
    numberADChannels: int = 2500,
    max_runtime_file: int = 3600
    ):


    # ------------------------- Open Session with the instrument ------------------------- # 

    try:

        logger.info("Opening session with the instrument...")

        rm    = pyvisa.ResourceManager()

        if UsbInstrument not in rm.list_resources():

            logger.critical(f"The USB instrument \"{UsbInstrument}\" could not be located as an avaliable resource in {rm.list_resources()}.")

            return 1

        scope = rm.open_resource(UsbInstrument)
    
    except Exception as e:

        logger.exception("Error while trying to connect to the instrument...")

        return 1


    # ------------------------- Test connection ------------------------- #

    logger.info("Querying for an IDN (check connection)...")

    idn = query_idn(scope)

    if idn is True:

        logger.info("Succesfully connected to the instrument...")
    
    else:

        logger.info(f"Failed to connected to the instrument \"{UsbInstrument}\"...")

        return 1



    # ------------------------- Convert trigger to ADChannels ------------------------- #

    trigger_in_units = convertValues.Convert_VoltsToUnits(
        TriggerInVolts,
        Encoder,
        ChannelPosition,
        ChannelScale,
        NumberOfDisplayDivisions
    )

    logger.info(f"Trigger value: {1000*float(TriggerInVolts)} mV.")



    # ------------------------- Set initial parameters ------------------------- #
    # ------------------------- and the folder to save the results ------------------------- #

    startTime  = time()

    nowTime    = time()

    numberGoodSamples: int = 0



    # --------- Create ROOT file and set the TTree to store the required waveforms --------- #

    root_file_name = f"{path_to_save_files}/{int(nowTime)}.root"

    arr_waveform_in_units = array('i', [0]*numberADChannels)

    event_name = array('i', [0])

    root_file = root.TFile(root_file_name, "CREATE")
    tree_waveforms = root.TTree("tree_waveforms", "waveforms")
    tree_waveforms.Branch("names", event_name  , "name/I")
    tree_waveforms.Branch("waveforms", arr_waveform_in_units, f"waveforms[{numberADChannels}]/I")



    # ------------------------- Run acquisition ------------------------- # 

    logger.info("STARTING ACQUISITION")

    while(True):


        if nowTime - startTime > max_runtime_file:

            logger.info(f"Closing file. Total of events: {numberGoodSamples}")

            if root_file:

                root_file.Write()
                root_file.Close()

            logger.info(f"Creating new root file...")

            root_file_name = f"{path_to_save_files}/{int(nowTime)}.root"    

            root_file = root.TFile(root_file_name, "CREATE")
            tree_waveforms = root.TTree("tree_waveforms", "waveforms")
            tree_waveforms.Branch("names", event_name  , "name/I")
            tree_waveforms.Branch("waveforms", arr_waveform_in_units, f"waveforms[{numberADChannels}]/I")

            startTime = time()

            continue


        try:
            waveform = readwvf.Get_Curve_Data(scope, Encoder)

        except:
            logger.error("Could not read waveform from oscilloscope.")

            waveform = None

            sleep(2)

            continue

        nowTime = time() # update nowTime after getting a sample

        if waveform is None: continue


        numberPeaks = searchPeaks.SearchPeaksNumber(
                    waveform, 
                    trigger_in_units, 
                    ExpectedPeaks, 
                    PulseWidth, 
                    numberADChannels
        )

        if numberPeaks != ExpectedPeaks: continue


        event_name[0] = int(time())

        for i in range(numberADChannels):

            arr_waveform_in_units[i] = waveform[i]

        tree_waveforms.Fill()

        numberGoodSamples += 1


    return status
# ====================================================================================================


# ====================================================================================================
def main(path_to_save_files: str, config_file_path: str, scope_config_name: str, acquisition_config_name: str):

    try:

        with open(config_file_path, 'r') as f:

            configs: dict = yaml.safe_load(f)

            status = run(
                path_to_save_files       = path_to_save_files,
                max_runtime_file         = configs.get(acquisition_config_name).get("RunTimeMaximum"),
                ExpectedPeaks            = configs.get(acquisition_config_name).get("MinPeaks"),
                PulseWidth               = configs.get(acquisition_config_name).get("PulseWidth"),
                UsbInstrument            = configs.get(scope_config_name).get("USB_Instrument"),
                Encoder                  = configs.get(scope_config_name).get("DataEncodeFormat"),
                TriggerInVolts           = configs.get(scope_config_name).get("ChannelTrigger"),
                ChannelPosition          = configs.get(scope_config_name).get("ChannelPosition"),
                ChannelScale             = configs.get(scope_config_name).get("ChannelScale"),
                NumberOfDisplayDivisions = configs.get(scope_config_name).get("NumberOfDisplayDivisions"),
                numberADChannels         = configs.get(scope_config_name).get("NumberADChannels")
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
    log_file_name = f"{path_to_save}/acquisition.log"
    
    

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
