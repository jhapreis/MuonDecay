import numpy as np


#====================================================================================================
def Convert_WaveformToMiliVolts(waveform_in_units, encoder, Scope_ChannelPosition, Scope_ChannelScale, Scope_NumberOfDisplayDivisions=10):
    
        
    waveform_in_mv = np.zeros(   ( len(waveform_in_units), 1 )   )
    
    
    for i in range( len(waveform_in_units) ):
    
        position = Convert_UnitsPosition(
            x=waveform_in_units[i],
            x_param='units',
            y_param='position',
            encoder=encoder,
            Scope_NumberOfDisplayDivisions=Scope_NumberOfDisplayDivisions
        )
    
        waveform_in_mv[i] = Convert_PositionToMiliVolts(
            x_position=position,
            Scope_ChannelPosition=Scope_ChannelPosition,
            Scope_ChannelScale=Scope_ChannelScale,
            voltage_reference=0
        )
    
    return waveform_in_mv



#====================================================================================================
def Convert_PositionToMiliVolts(x_position, Scope_ChannelPosition, Scope_ChannelScale, voltage_reference=0):
    
        
    value_in_mV = 1000 * Scope_ChannelScale * (x_position - Scope_ChannelPosition) + voltage_reference
    
    return value_in_mV



#====================================================================================================
def Convert_UnitsPosition(x, x_param, y_param, encoder, Scope_NumberOfDisplayDivisions=10):
    
    """
    Converts a value in "units" to "position" on the scope screen
    
        V, v: position or units
            
        V(v) = Delta_V / Delta_v * (v - v_min) + V_min

    Args:
        encoder (_type_): _description_
        Scope_ChannelPosition (_type_): _description_
        Scope_ChannelScale (_type_): _description_
        Scope_NumberOfDisplayDivisions (int, optional): _description_. Defaults to 10.

    Returns:
        
    """
    
    x_param = x_param.upper()
    
    y_param = y_param.upper()
    
    if( x_param == "UNITS"):
        
        if(y_param != "POSITION"):
            print(f"Erro nos parâmetros de conversão: 'y_param' foi achado como '{y_param}' e, para acompanhar 'x_param', deveria ser 'POSITION'")
            exit(1)
            
    elif(x_param == "POSITION"):
        
        if(y_param != "UNITS"):
            print(f"Erro nos parâmetros de conversão: 'y_param' foi achado como '{y_param}' e, para acompanhar 'x_param', deveria ser 'UNITS'")
            exit(1)
            
    else:
        print(f"Erro nos parâmetros de conversão: 'x_param' deve ser ou 'UNITS' ou 'POSITION', mas foi achado como {x_param}.")
   

   
    Position_min = (-1)*Scope_NumberOfDisplayDivisions / 2
    
    Position_max =      Scope_NumberOfDisplayDivisions / 2
    
    if(   encoder.upper() == "RPBINARY" ):
        units_min =    0
        units_max =  255    
        
    elif( encoder.upper() == "ASCII"    ):
        units_min = -127
        units_max =  128
        
    else:
        print(f"Erro no encoder: {encoder}\n\n")
        
        exit(1)
    
    
    # Relação de 'units' para 'position'
    ratio = (Position_max - Position_min) / (units_max - units_min)
    
    x_min = units_min
    
    y_min = Position_min
    
    # Caso seja de 'position' para 'units', inverta a relação
    if y_param == "UNITS":
        
        ratio = 1/ratio
        
        x_min = Position_min
        
        y_min = units_min


    y = ratio * (x - x_min) + y_min

    return y



#====================================================================================================
def Convert_WaveformToMiliVolts_ScopeParameters(waveform_in_units, y_zero, y_off, y_mult):

    waveform_in_mv = np.zeros( (len(waveform_in_units), 1) )
    
    for i in range(len(waveform_in_units)):
        
        waveform_in_mv[i] = Convert_UnitsToMiliVolts_ScopeParameters(
            waveform_in_units[i],
            y_zero,
            y_off,
            y_mult
        )
    
    return waveform_in_mv



#====================================================================================================
def Convert_UnitsToMiliVolts_ScopeParameters(y_units, y_zero, y_off, y_mult):
    
    y_volts = y_zero + y_mult * (y_units - y_off)
    
    y_mv    = 1000 * y_volts
    
    return y_mv



#====================================================================================================
def ConvertMiliVoltsToUnits_ScopeParameters(y_mv, y_zero, y_off, y_mult):
    
    y_volts = y_mv / 1000
    
    y_units = y_off + 1/y_mult * (y_volts - y_zero)

    return y_units



#====================================================================================================
def Convert_TimeToMicroSec(delta_t, Scope_Xaxis_Scale, Scope_Xaxis_Number_Divisions=10, number_ADChannels=2500):
    
    time_per_division = Scope_Xaxis_Scale * Scope_Xaxis_Number_Divisions / number_ADChannels
    
    time_in_micro_sec = 1E6 * time_per_division * delta_t
    
    return time_in_micro_sec
