#ifndef ACQUISITION_H
#define ACQUISITION_H



#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <TTree.h>

#include <visa.h>

#include "../../Configs/cfg.h"




//====================================================================================================
/**
 * @brief This function gets the buffer ViChar* string read from NI_VISA read function and
 * tries to unpack the values like the necessity. The array passed to this function is the container
 * that is going to recieve the values from the buffer.
 * 
 * @param buffer string read from NI_VISA
 * 
 * @param Array_WaveformAsInt pointer to array of int that contains the waveforms
 *  
 * @returns In case of success, returns 0. In case of inexistence of the array,
 * or in case of error on the number of points or any other error, returns -1.
 * 
 * @warning The result is extremely dependent on the encoder format, and it's
 * recommended to use the RPBINARY format (with numbers from 0 to 255 -- 8 bytes).
 */
int Get_CurveData(ViChar* buffer, int* Array_WaveformAsInt);



//====================================================================================================
/**
 * @brief This function runs the settings on the scope setup,
 * following the correspondent config file. 
 * 
 * The settings values are stored as std:string and they are acessed from the ROOT file,
 * on the correspondent TTree. Those correspondent values are retrieved directly
 * from the instrument, throught query method.
 * 
 * @param status 
 * @param scope 
 * @param retCount 
 * @param tree Optional (may be NULL). If it's NULL, doesn't do nothing with 
 * the TTree or ROOT file. If it is not NULL, creates a branch for every parameter
 * that is required to save on the TTree. 
 * @return int ; success or failure
 */
int Set_ScopeParameters(ViStatus status, ViSession scope, ViUInt32 retCount, TTree* tree);



#endif //ACQUISITION_H
