from array import array



#====================================================================================================
def Convert_WaveformToMiliVolts(waveform_in_units, encoder, Scope_ChannelPosition, Scope_ChannelScale, Scope_NumberOfDisplayDivisions=10):
    """
    Converts Waveform in units to mV
    
        V(v) = Delta_V / Delta_v * (v - v_min) + V_min

    Args:
        waveform_in_units (_type_): _description_
        encoder (_type_): _description_
        Scope_ChannelPosition (_type_): _description_
        Scope_ChannelScale (_type_): _description_
        Scope_NumberOfDisplayDivisions (int, optional): _description_. Defaults to 10.

    Returns:
        array: waveform in mV
    """
    
    
    
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
    
    ratio = (Position_max - Position_min) / (units_max - units_min)
    
        
    Waveform_mV = array('f', [0]*len(waveform_in_units))
    
    for i in range(len(waveform_in_units)):

        y_pos = ratio*(waveform_in_units[i] - units_min) + Position_min

        y_pos -= Scope_ChannelPosition                     # relative to Scope_ChannelPosition value

        Waveform_mV[i] = 1000 * Scope_ChannelScale * y_pos # converts from position to mV, using y-scale value (in volts)
    
    
    
    return Waveform_mV



#====================================================================================================
def Convert_TimeToMicroSec(delta_t, max_time_stamp=10, number_ADChannels=2500):
    
    return delta_t*max_time_stamp/number_ADChannels



