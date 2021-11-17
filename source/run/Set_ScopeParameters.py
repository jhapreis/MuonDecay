#==================================================
# IMPORTS
#==================================================

import pyvisa
from time import time


import sys
from pathlib import Path

_ = Path().resolve().parent.parent # Add source folder to sys.path
sys.path.insert(0, str(_))


from source.configs import cfg_scope



#==================================================
# SET SCOPE PARAMETERS
#==================================================

def Scope_Parameters(oscilloscope):

    # oscilloscope.write('ACQuire:STATE RUN')
    # print('\nState: RUN')
                
    oscilloscope.write(f'SELECT:{cfg_scope.channel} ON')
    # print(f'\n{cfg_scope.channel} ON')

    # oscilloscope.write(f'HORizontal:DELay:SCAle {cfg_scope.delay_scale}')

    # oscilloscope.write(f'HORizontal:DELay:POSition: {cfg_scope.delay_scale_position}')

    oscilloscope.write(f'DATa:SOUrce {cfg_scope.channel}') 
    # print(f'')
                
    oscilloscope.write(f'DATa:ENCdg {cfg_scope.encode_format}') 
    # print(f'Encode Format: {cfg_scope.encode_format}')

    oscilloscope.write(f'DATa:WIDth {cfg_scope.width}') 
    # print(f'Data Width: {cfg_scope.width}')
                
    oscilloscope.write(f'{cfg_scope.channel}:SCAle {cfg_scope.channel_scale}')
    # print(f'{cfg_scope.channel} scale: {cfg_scope.channel_scale}')
                
    oscilloscope.write(f'{cfg_scope.channel}:POSition {cfg_scope.channel_position}')
    # print(f'{cfg_scope.channel} position: {cfg_scope.channel_position}')
                
    oscilloscope.write(f'{cfg_scope.channel}:PRObe {cfg_scope.channel_probe}')
    # print(f'{cfg_scope.channel} probe: {cfg_scope.channel_probe}')
                
    oscilloscope.write(f'TRIGger:MAIn:LEVel {cfg_scope.trigger}')
    # print(f'Trigger: {1_000*cfg_scope.trigger} mV')
                
    oscilloscope.write(f'HORizontal:MAIn:SCAle {cfg_scope.horizontal_scale}')
    # print(f'Horizontal scale: {cfg_scope.horizontal_scale}')
                
    oscilloscope.write(f'HORizontal:MAIn:POSition {cfg_scope.horizontal_position_1};') 
    # print(f'Horizontal Position: {cfg_scope.horizontal_position_1}')
                
    oscilloscope.write(f'HORizontal:MAIn:POSition {cfg_scope.horizontal_position_2};') 
    # print(f'Horizontal Position: {cfg_scope.horizontal_position_2}')
                
    oscilloscope.write(f'DISplay:PERSistence {cfg_scope.persistence}') 
    #print(f'Persistence: {cfg_scope.persistence}')
                
    oscilloscope.write(f'TRIGGER:MAIN:EDGE:SLOPE {cfg_scope.slope}') 
    # print(f'Slope: {cfg_scope.slope}')



def Query_Curve(oscilloscope):
    
    start  = time()
    
    curve  = oscilloscope.query_ascii_values("curve?")
        
    finish = time()
    
    print(f"\n   Time elapsed: {round(finish - start, 2)} seconds.\n")
    
    return(curve)



rm    = pyvisa.ResourceManager()
scope = rm.open_resource(cfg_scope.SCOPEID)
