#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "includes/peaks.h"
#include "Configs/cfg.h"



//====================================================================================================

double* Convert_WaveformMiliVolts(int* Waveform_Units, int numberPoitsWaveform){

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

    // Encoder format; values of the units min/max based on the encoder format
    char encoder[12] = Scope_DataEncodeFormat;

    if(   strcmp(uppercase(encoder), "RPBINARY") == 0   ){

        units_min = 0;
        units_max = 255;
    }
    else if(   strcmp(uppercase(encoder), "ASCII") == 0   ){

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

        y_pos -= Scope_ChannelPosition; // relative to Scope_ChannelPosition value

        Waveform_mV[i] = 1000 * Scope_ChannelScale * y_pos; // converts from position to mV, using y-scale value (in volts)
    }
    


    return Waveform_mV;
}



//====================================================================================================

double Convert_VoltsToUnits(double valueVolts){

    /**
     * v(V) = Delta_v / Delta_V * (V - V_min) + v_min 
     */

    // Min and Max display positions; symmectric, supposedly
    int Position_min = (-1)*Scope_NumberOfDisplayDivisions / 2;
    int Position_max =      Scope_NumberOfDisplayDivisions / 2;

    
    // Encoder format; values of the units min/max based on the encoder format
    char encoder[12] = Scope_DataEncodeFormat;

    // Min and Max values in units (depends on the encoding format)
    int units_min, units_max;
    if(   strcmp(uppercase(encoder), "RPBINARY") == 0   ){

        units_min = 0;
        units_max = 255;
    }
    else if(   strcmp(uppercase(encoder), "ASCII") == 0   ){

        units_min = -127;
        units_max = 128;
    }
    else{
        return 0;
    }


    // Delta_v / Delta_V
    double ratio = (double) (units_max - units_min) / (Position_max - Position_min);
    

    // y position on screen, relative to Scope_ChannelPosition
    double y_pos = valueVolts / Scope_ChannelScale  +  Scope_ChannelPosition;


    // Convert value from position to units
    double valueUnits = ratio*(y_pos - Position_min) + units_min;


    return valueUnits;
}



//====================================================================================================

char* uppercase(char* str){

    int lenght = strlen(str);

    for(int i=0; i<lenght; i++){
        str[i] = toupper( str[i] );
    }

    return str;
}
