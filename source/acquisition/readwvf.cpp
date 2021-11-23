#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>

#include <visa.h>

#include "../configs/cfg.h"
#include "acquisition.h"



//==================================================

int* Get_CurveData(ViChar* buffer){
    /**
     * @brief This function unzips the query result in two steps:
     * 1) It unzips the buffer on unsigned char variables and
     * 2) Stores that on an integer array
     * 
     * If the values are from RPBINARY, the range is from 0 to 255 (from bottom to top, 
     * with center on 127).
     * Therefore, the values need to be converted from the specific range after that. Now, 
     * they are only being stored like they are.
     * 
     * By the way, this dinamically allocates memory to store the result. 
     * 
     */

    unsigned char waveformAsChar[Scope_NumberADChannels+1];
    memset(waveformAsChar, 0, sizeof(waveformAsChar));

    int numberDigitsLenght = 0, numberPointsCurve = 0, index = 0;

    if( buffer[index] != '#' ){ // error on format
        return NULL;
    }



    index++; // index = 1

    numberDigitsLenght = buffer[index] - '0'; // char to int



    index++; // index = 2

    char pointsCurveInfo[numberDigitsLenght+1];

    for(int i=0; i<numberDigitsLenght; i++){
        pointsCurveInfo[i] = buffer[index];
        index++; // increase index until fisrt data value
    }

    sscanf(pointsCurveInfo, "%d", &numberPointsCurve);

    // printf("Number of points in curve: %d\n", numberPointsCurve);



    if(numberPointsCurve != Scope_NumberADChannels){ // compare with NumberADCChannels
        printf("Error on numberPointsCurve: %d != %d\n", numberPointsCurve, Scope_NumberADChannels);
        return NULL;
    }


    for(int i=0; i<numberPointsCurve; i++){ // unzips curve data into waveform as unsigned char

        waveformAsChar[i] = buffer[index];

        index++;
    }    
    // printf("%s\n", waveformAsChar);



    int* waveformAsInt = (int*) calloc(numberPointsCurve, sizeof(int)); // allocates memory to waveform curve

    for(int i=0; i<numberPointsCurve; i++){

        waveformAsInt[i] = waveformAsChar[i];

        // printf("%d ", waveformAsInt[i]);
    }
    // printf("\n");



    return waveformAsInt;
}



//==================================================

char* uppercase(char* str){

    int lenght = strlen(str);

    for(int i=0; i<lenght; i++){
        str[i] = toupper( str[i] );
    }

    return str;
}



//==================================================

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
