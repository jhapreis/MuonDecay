import pandas as pd

from ROOT import TFile

#====================================================================================================
def Get_AcquisitionParameters(path_to_output_file):
    
    """
    Read output.txt
    """
    #----------------------------------------------------------------------------------------------------
    with open(path_to_output_file, 'r') as f:
        
        lines = f.read()

        df = pd.DataFrame(
            {
            'encoder'                 :[lines.split(   "DATA:ENCDG?,"   )[1].split("\n")[0]],
            'trigger_main_level'      :[float( lines.split(   "TRIGGER:MAIN:LEVEL?,"   )[1].split("\n")[0] )],
            'horizontal_main_scale'   :[float( lines.split(  "HORIZONTAL:MAIN:SCALE?," )[1].split("\n")[0] )],
            'horizontal_main_position':[float( lines.split("HORIZONTAL:MAIN:POSITION?,")[1].split("\n")[0] )],
            'channel_scale'           :[float( lines.split(       "CH1:SCALE?,"        )[1].split("\n")[0] )],
            'channel_position'        :[float( lines.split(      "CH1:POSITION?,"      )[1].split("\n")[0] )],
            'channel_probe'           :[float( lines.split(       "CH1:PROBE?,"        )[1].split("\n")[0] )]
            },
                          
            index=['parameter']
                          )
    
    return df


