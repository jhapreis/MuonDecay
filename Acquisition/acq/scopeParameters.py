import pyvisa
import logging
import pandas as pd
import sys

logger = logging.getLogger('__main__')


# ====================================================================================================
def uppercase_dictionary(dictionary: dict):
    
    new_dict = dict([  (key.upper(), dictionary.get(key).upper()) for key in dictionary.keys()  ])
    
    return new_dict
# ====================================================================================================



# ====================================================================================================
def query_idn(instrument: pyvisa.Resource) -> bool:
    
    try:
        
        query = instrument.query("*IDN?")
        
    except Exception as e:
        
        logger.exception("Could not execute a query for the IDN.")
        
        return False

    
    logger.info(f"\n\n{query}\n")
    
    return True
# ====================================================================================================



# ====================================================================================================
def Set_Scope_Parameters(instrument: pyvisa.Resource, scopeParam: dict) -> int:
    
    ch: str

    if not instrument: return -1
    
    writes = {
        'ChannelName':\
            'SELECT: [CH] ON',
        'DataSource':\
            'DATa:SOUrce [CH]',
        'ChannelScale':\
            '[CH]:SCAle [VALUE]',
        'ChannelPosition':\
            '[CH]:POSition [VALUE]',
        'ChannelProbe':\
            '[CH]:PRObe [VALUE]',
        'HorizontalDelayScale':\
            'HORizontal:DELay:SCAle [VALUE]',
        'DataEncodeFormat':\
            'DATa:ENCdg [VALUE]',
        'DataEncodeWidth':\
            'DATa:ENCdg [VALUE]',
        'ChannelTrigger':\
            'TRIGger:MAIn:LEVel [VALUE]',
        'ChannelTriggerSlope':\
            'TRIGGER:MAIN:EDGE:SLOPE [VALUE]',
        'ChannelHorizontalScale':\
            'HORizontal:MAIn:SCAle [VALUE]',
        'ChannelHorizontalPosition':\
            'HORizontal:MAIn:POSition [VALUE]',
        'Persistence':\
            'DISplay:PERSistence [VALUE]'
    }

    # try:
    #     ch = scopeParam['ChannelName']

    # except KeyError as ke:
    #     logger.exception("Could not get the channel name from the config file... A few writes will be skipped.")

    # except Exception as e:
    #     logger.exception("Error while trying to write configs on the channel.")


    for param in writes.keys():

        try:
            ch = scopeParam['ChannelName']

            wrt = writes.get(param)

            if '[CH]' in wrt: 
                wrt = wrt.replace( '[CH]', ch )
            
            if '[VALUE]' in wrt:
                wrt = wrt.replace( '[VALUE]', str(scopeParam[param]) )

            instrument.write(wrt)

        except KeyError as ke:
            logger.warning(f"Problem setting \"{param}\": Missing {ke} key.")

            continue

        except Exception as e:
            logger.exception(f"Problem setting \"{param}\".")

            continue
            
    logger.info( f'\nSCOPE INFOs:\n{instrument.query("WFMPre?")}\n' ) #Command to transfer waveform preamble information.
    
    return 0
# ====================================================================================================



# ====================================================================================================
def Check_Scope_Parameters(instrument: pyvisa.Resource) -> pd.DataFrame:

    if not instrument:
        
        return None
    
    queries = {
        'DataSource':'DATa:SOUrce?',
        'HorizontalDelayScale':'HORizontal:DELay:SCAle?',
        'HorizontalDelayPosition':'HORizontal:DELay:SCAle?',
        'Encoder':'DATa:ENCdg?',
        'Width':'DATa:WIDth?',
        'TriggerLevel':'TRIGger:MAIn:LEVel?',
        'TriggerSlope':'TRIGGER:MAIN:EDGE:SLOPE?',
        'HorizontalScale':'HORizontal:MAIn:SCAle?',
        'HorizontalPosition':'HORizontal:MAIn:POSition?',
        'Persistence':'DISplay:PERSistence?'
    }

    try:
        ch = instrument.query(queries.get('DataSource')).replace('\n', '')
    except:
        logger.warning("Could not run the query for instrument Channel. Cancelling the conference...")

        return None

    queries['ChannelScale']    = f'{ch}:SCAle?'
    queries['ChannelPosition'] = f'{ch}:POSition?'
    queries['ChannelProbe']    = f'{ch}:PRObe?'

    df = pd.DataFrame()

    for q in queries.keys():

        try: 
            qResult = instrument.query( queries.get(q) ).replace('\n', '')

        except:
            logger.exception(f"Could not run the query for \"{q}\"")

            qResult = None
        
        df[q] = [qResult]
    
    df.index = ['values']

    return df.T
# ====================================================================================================
