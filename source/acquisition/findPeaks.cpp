#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "../configs/cfg.h"



//====================================================================================================

int* FindPeaks_Waveform(int* waveformAsInt, int waveformSize, double height){

    /**
     * @brief This functions is built considering that the
     * trigger slope is negative. It means that the waveform is "upside down".
     * 
     * The method consists on findind a value that triggers the search and the find the
     * local min/max within an specific interval. After that, it stores the x-coordinate
     * of that local min/max on an array. Does that for the entire array, searching for 
     * regions that contains peaks (with defined width).
     * 
     * The return is an dinamically allocated array that has as size the number of peaks and,
     * as values, the peaks x-coordinates.
     */

    int x_peak = 0, y_peak = 0;
    
    int waveformPeaks[waveformSize];
    memset(waveformPeaks, -1, sizeof(waveformPeaks));

    int number_peaks    = 0;
    int index           = 0;


    while(index < waveformSize){

        if(waveformAsInt[index] <= height){ // if triggered

            x_peak = index;
            y_peak = waveformAsInt[index];

            for(int i=index; i<Acquisition_PulseWidth+index; i++){ // find max/min on region
                if(waveformAsInt[index] < y_peak){
                    x_peak = i;
                    y_peak = waveformAsInt[i];
                }
            }
            waveformPeaks[number_peaks] = x_peak;

            index += Acquisition_PulseWidth;
            number_peaks++;
        }

        index++;
    }


    if(number_peaks == 0) return NULL; // no peaks found


    int* x_coordinates_peaks = (int*) calloc(number_peaks, sizeof(int));
    for(int i=0; i<number_peaks; i++){

        x_coordinates_peaks[i] = waveformPeaks[i];
    }

    return x_coordinates_peaks;
}



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
