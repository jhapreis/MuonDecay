import numpy as np
import logging
import sys

logger = logging.getLogger('__main__')


# ====================================================================================================
def SearchPeaksNumber(waveformAsInt: np.ndarray , height: float, expectedPeaks: int, PulseWidth: int, waveformSize: int = 2500) -> int:

    number_of_peaks: int = 0
    index: int           = 0

    if expectedPeaks <= 0:

        return expectedPeaks

    while(index < waveformSize):

        if(number_of_peaks > expectedPeaks):
            
            return -1 # more peaks than expected

        if(waveformAsInt[index] <= height): # if triggered

            number_of_peaks += 1
            index += PulseWidth
            
        else:

            index += 1

    return number_of_peaks
# ====================================================================================================
