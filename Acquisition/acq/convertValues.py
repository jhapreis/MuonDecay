import numpy as np
import logging
import sys

logger = logging.getLogger(__name__)



# ====================================================================================================
def Convert_WaveformMiliVolts(waveform_units: np.ndarray, encoder: str, ChannelPosition: int, ChannelScale: float, NumberOfDisplayDivisions: int = 10) -> np.ndarray:

    """
    V(v) = Delta_V / Delta_v * (v - v_min) + V_min
    """

    units_min: int
    units_max: int
    Position_min: int = (-1)*NumberOfDisplayDivisions / 2
    Position_max: int =      NumberOfDisplayDivisions / 2

    ratio: float
    y_pos: float

    waveform_mv: np.ndarray


    if encoder.upper() == "RPBINARY":

        units_min = 0
        units_max = 255

    elif encoder.upper() == "ASCII":

        units_min = -127
        units_max =  128

    else:

        logging.warn(f"Could not identify the encoder \"{encoder}\" between ASCII or RPBINARY.\n\n")

        return None


    ratio = (Position_max - Position_min) / (units_max - units_min)

    y_pos = ratio*(waveform_units - units_min) + Position_min

    y_pos -= ChannelPosition # relative to Scope_ChannelPosition value

    waveform_mv = 1000 * ChannelScale * y_pos # converts from position to mV, using y-scale value (in volts)

    return waveform_mv
# ====================================================================================================



# ====================================================================================================
def Convert_VoltsToUnits(value_in_volts: float, encoder: str, ChannelPosition: int, ChannelScale: float, NumberOfDisplayDivisions: int = 10) -> float:

    """
    v(V) = Delta_v / Delta_V * (V - V_min) + v_min
    """

    units_min: int
    units_max: int
    Position_min: int = (-1)*NumberOfDisplayDivisions / 2
    Position_max: int =      NumberOfDisplayDivisions / 2

    ratio: float
    y_pos: float
    valueUnits: float

    waveform_mv: np.ndarray

    value_in_volts           = float(value_in_volts)
    ChannelPosition          = int(ChannelPosition)
    ChannelScale             = float(ChannelScale)
    NumberOfDisplayDivisions = int(NumberOfDisplayDivisions)

    if encoder.upper() == "RPBINARY":

        units_min = 0
        units_max = 255

    elif encoder.upper() == "ASCII":

        units_min = -127
        units_max =  128

    else:

        logging.warn(f"Could not identify the encoder \"{encoder}\" between ASCII or RPBINARY.\n\n")

        return None


    ratio = (units_max - units_min) / (Position_max - Position_min)

    # y position on screen, relative to ChannelPosition
    y_pos = value_in_volts / ChannelScale  +  ChannelPosition


    # Convert value from position to units
    valueUnits = ratio*(y_pos - Position_min) + units_min

    return valueUnits
# ====================================================================================================
