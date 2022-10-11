import pyvisa
import logging
import numpy as np


logger = logging.getLogger(__name__)


# ====================================================================================================
def Get_Curve_Data(instrument: pyvisa.Resource, encoder: str = "RPBinary") -> np.ndarray:

    values: np.ndarray = None

    if encoder.upper() == "RPBINARY":

        values = np.array(
            instrument.query_binary_values('CURVE?', datatype='B')
        )

    elif encoder.upper() == "ASCII":

        values = np.array(
            instrument.query_ascii_values('CURVE?')
        )

    return values
# ====================================================================================================
