#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "../Acquisition/cfg.h"


//====================================================================================================

int* FindPeaks_Waveform(int* waveformAsInt, int waveformSize, double height, int pulseWidth, int minPeaks){

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

            for(int i=index; i<pulseWidth+index; i++){ // find max/min on region
                if(waveformAsInt[index] < y_peak){
                    x_peak = i;
                    y_peak = waveformAsInt[i];
                }
            }
            waveformPeaks[number_peaks] = x_peak;

            index += pulseWidth;
            number_peaks++;
        }

        index++;
    }

    // printf("number peaks = %d\n", number_peaks);
    if(number_peaks != minPeaks) return NULL; // error on search for peaks


    int* x_coordinates_peaks = (int*) calloc(number_peaks, sizeof(int));
    for(int i=0; i<number_peaks; i++){

        x_coordinates_peaks[i] = waveformPeaks[i];
    }

    return x_coordinates_peaks;
}



//====================================================================================================

double* Convert_WaveformMiliVolts(int* Waveform_Units, int numberPoitsWaveform, char* encoder, float chScale, float chPosition){

    /**
     * V(v) = Delta_V / Delta_v * (v - v_min) + V_min 
     */

    // Min and Max values in units (depends on the encoding format)
    int units_min, units_max;

    // Min and Max display positions; symmectric, supposedly
    int Position_min = (-1)*Scope_NumberOfDisplayDivisions / 2;
    int Position_max =      Scope_NumberOfDisplayDivisions / 2;

    // Delta_V / Delta_v
    double ratio = 0;
    
    // Value of the waveform point as y-position
    double y_pos = 0;

    if(   strcmp(encoder, "RPBINARY") == 0   ){

        units_min = 0;
        units_max = 255;
    }
    else if(   strcmp(encoder, "ASCII") == 0   ){

        units_min = -127;
        units_max = 128;
    }
    else{
        
        return NULL;
    }

    ratio = (double) (Position_max - Position_min) / (units_max - units_min);



    // Waveform converted to mV values
    double* Waveform_mV = (double*) calloc(numberPoitsWaveform, sizeof(double));

    

    for(int i=0; i<numberPoitsWaveform; i++){

        y_pos = ratio*(Waveform_Units[i] - units_min) + Position_min;

        y_pos -= chPosition; // relative to Scope_ChannelPosition value

        Waveform_mV[i] = 1000 * chScale * y_pos; // converts from position to mV, using y-scale value (in volts)
    }
    

    return Waveform_mV;
}



//====================================================================================================

double Convert_VoltsToUnits(double valueVolts, char* encoder, float chScale, float chPosition){

    /**
     * v(V) = Delta_v / Delta_V * (V - V_min) + v_min 
     */

    // Min and Max display positions; symmectric, supposedly
    int Position_min = (-1)*Scope_NumberOfDisplayDivisions / 2;
    int Position_max =      Scope_NumberOfDisplayDivisions / 2;


    // Min and Max values in units (depends on the encoding format)
    int units_min, units_max;
    if(   strcmp(encoder, "RPBINARY\n") == 0   ){

        units_min = 0;
        units_max = 255;
    }
    else if(   strcmp(encoder, "ASCII\n") == 0   ){

        units_min = -127;
        units_max = 128;
    }
    else{
        return 0;
    }


    // Delta_v / Delta_V
    double ratio = (double) (units_max - units_min) / (Position_max - Position_min);
    

    // y position on screen, relative to Scope_ChannelPosition
    double y_pos = valueVolts / chScale  +  chPosition;


    // Convert value from position to units
    double valueUnits = ratio*(y_pos - Position_min) + units_min;

    return valueUnits;
}
