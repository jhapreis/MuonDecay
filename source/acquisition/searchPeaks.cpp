#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "cfg.h"



//====================================================================================================

int SearchPeaksNumber_WithExpected(int* waveformAsInt, int waveformSize, double height, int expectedPeaks){

    int number_of_peaks = 0;
    int index           = 0;

    while(index < waveformSize){

        if(number_of_peaks > expectedPeaks) return -1; // more peaks than expected

        if(waveformAsInt[index] <= height){ // if triggered

            number_of_peaks++;
            index += Acquisition_PulseWidth;
        }
        else{

            index ++;
        }
    }

    return number_of_peaks;
}



//====================================================================================================

int SearchPeaksNumber_WithoutExpected(int* waveformAsInt, int waveformSize, double height, int expectedPeaks){

    int number_of_peaks = 0;
    int index           = 0;

    while(index < waveformSize){

        if(waveformAsInt[index] <= height){ // if triggered

            number_of_peaks++;
            index += Acquisition_PulseWidth;
        }
        else{

            index ++;
        }
    }

    return number_of_peaks;
}



//====================================================================================================

int SearchPeaksNumber(int* waveformAsInt, int waveformSize, double height, int expectedPeaks){

    int number_of_peaks = 0;

    if(expectedPeaks > 0){ // we DO search for an specific number of peaks
        number_of_peaks = SearchPeaksNumber_WithExpected(waveformAsInt, waveformSize, height, expectedPeaks);
        if(number_of_peaks != expectedPeaks) return -1;        
    }
    else{
        number_of_peaks = SearchPeaksNumber_WithoutExpected(waveformAsInt, waveformSize, height, expectedPeaks);
    }

    return number_of_peaks;
}
