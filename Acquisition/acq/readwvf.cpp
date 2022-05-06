#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <string>
#include <iostream>

#include <visa.h>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>

#include "../includes/acquisition.h"
#include "../configs/cfg.h"



//====================================================================================================

int Get_CurveData(ViChar* buffer, int* Array_WaveformAsInt){
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

    if( buffer[index] != '#' ) return 1; // error on format


    index++; // index = 1
    numberDigitsLenght = buffer[index] - '0'; // char to int


    index++; // index = 2

    char pointsCurveInfo[numberDigitsLenght+1];

    for(int i=0; i<numberDigitsLenght; i++){

        pointsCurveInfo[i] = buffer[index];
        index++; // increase index until fisrt data value
    }
    sscanf(pointsCurveInfo, "%d", &numberPointsCurve); // convert the value from char-array to int



    /**
     * @brief UNPACKS INTO CHAR-ARRAY
     * Unpacks the values from the buffer to the char-array.
     * If the number of points doesn't match, returns 1. 
     */
    if(numberPointsCurve != Scope_NumberADChannels){
        
        printf("Error on numberPointsCurve: %d != %d\n", numberPointsCurve, Scope_NumberADChannels);
        return 1;
    }
    for(int i=0; i<numberPointsCurve; i++){ // unzips curve data into waveform as unsigned char

        waveformAsChar[i] = buffer[index];
        index++;
    }    



    /**
     * @brief SAVE DATA AS INT 
     * Now that the char-array contains the data, we are going to pass that to
     * the int-array that was previously passed as parameter. 
     */
    if(Array_WaveformAsInt == NULL) return -1;

    for(int i=0; i<numberPointsCurve; i++){

        Array_WaveformAsInt[i] = waveformAsChar[i];
    }


    return 0;
}
