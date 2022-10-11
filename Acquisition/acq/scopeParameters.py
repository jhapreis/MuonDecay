import pyvisa
import logging
import pandas as pd
import sys

logger = logging.getLogger(__name__)


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
        
        logger.warning("Could not execute a query for the IDN.")
        
        return False

    
    logger.info(f"\n\n{query}\n")
    
    return True
# ====================================================================================================



# ====================================================================================================
def Set_Scope_Parameters(instrument: pyvisa.Resource, scopeParam: dict) -> int:
    
    if not instrument:
        
        return -1
    
    writes = {
        'ChannelName':\
            f'SELECT: {scopeParam["ChannelName"]} ON',
        'DataSource':\
            f'DATa:SOUrce {scopeParam["ChannelName"]}',
        'ChannelScale':\
            f'{scopeParam["ChannelName"]}:SCAle {scopeParam["ChannelScale"]}',
        'ChannelPosition':\
            f'{scopeParam["ChannelName"]}:POSition {scopeParam["ChannelPosition"]}',
        'ChannelProbe':\
            f'{scopeParam["ChannelName"]}:PRObe {scopeParam["ChannelProbe"]}',
        'HorizontalDelayScale':\
            f'HORizontal:DELay:SCAle: {scopeParam["HorizontalDelayScale"]}',
        'Encoder':\
            f'DATa:ENCdg {scopeParam["DataEncodeFormat"]}',
        'Width':\
            f'DATa:ENCdg {scopeParam["DataEncodeWidth"]}',
        'TriggerLevel':\
            f'TRIGger:MAIn:LEVel {scopeParam["ChannelTrigger"]}',
        'TriggerSlope':\
            f'TRIGGER:MAIN:EDGE:SLOPE {scopeParam["ChannelTriggerSlope"]}',
        'HorizontalScale':\
            f'HORizontal:MAIn:SCAle {scopeParam["ChannelHorizontalScale"]}',
        'HorizontalPosition':\
            f'HORizontal:MAIn:POSition {scopeParam["ChannelHorizontalPosition"]}',
        'Persistence':\
            f'DISplay:PERSistence {scopeParam["Persistence"]}'
    }

    for param in writes.keys():

        try:
            instrument.write(writes.get(param))

        except KeyError as ke:
            logger.warning(f"Problem setting \"{param}\": Missing \"{ke}\" key.")

            continue

        except Exception as e:
            logger.exception(f"Problem setting \"{param}\".")

            continue
        
    # if channel: 
        
    #     if channel_scale: instrument.write()
                
    #     if channel_position: instrument.write()
                    
    #     if channel_probe: instrument.write()
    
    # if horizontal_delay_scale: instrument.write()
    
    # if horizontal_delay_position: instrument.write(f'HORizontal:DELay:POSition:{horizontal_delay_position} ON')
                
    # if encode_format: instrument.write(f'DATa:ENCdg {encode_format}') 

    # if width: instrument.write(f'DATa:WIDth {width}') 
                
    # if trigger: instrument.write(f'TRIGger:MAIn:LEVel {trigger}')
                
    # if horizontal_scale: instrument.write(f'HORizontal:MAIn:SCAle {horizontal_scale}')
                
    # if horizontal_position_1: instrument.write(f'HORizontal:MAIn:POSition {horizontal_position_1};') 
                
    # if horizontal_position_2: instrument.write(f'HORizontal:MAIn:POSition {horizontal_position_2};') 
                
    # if persistence: instrument.write(f'DISplay:PERSistence {persistence}') 
                
    # if slope: instrument.write(f'TRIGGER:MAIN:EDGE:SLOPE {slope}') 
            
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
            logger.warning(f"Could not run the query for \"{q}\"")

            qResult = None
        
        df[q] = [qResult]
    
    df.index = ['values']

    return df.T
# ====================================================================================================
