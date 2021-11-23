#ifndef ACQUISITION_H
#define ACQUISITION_H



#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <visa.h>

#include "../configs/cfg.h"




//==================================================
/**
 * @brief This function gets the buffer ViChar* string read from NI_VISA read function and
 * tries to unpack the values like the necessity. 
 * 
 * @param buffer string read from NI_VISA
 *  
 * @returns In case of success, returns a pointer to a dinamically allocated array that contains the
 * data as int numbers. If it fails, or if the number of points is different from the expected,
 * it returns NULL.
 * 
 * @warning The result is extremely dependent on the encoder format, and it's
 * recommended to use the RPBINARY format (with numbers from 0 to 255 -- 8 bytes).
 */
int* Get_CurveData(ViChar* buffer);



//==================================================
/**
 * @brief 
 * 
 * @param str 
 * @return char* 
 */
char* uppercase(char* str);


//==================================================
/**
 * @brief 
 * 
 * @param status 
 * @param scope 
 * @param retCount 
 * @return int 
 */
int Set_ScopeParameters(ViStatus status, ViSession scope, ViUInt32 retCount);



//==================================================
/**
 * @brief 
 * 
 * @param Waveform_Units 
 * @param numberPoitsWaveform 
 * @return double* 
 */
double* Convert_WaveformMiliVolts(int* Waveform_Units, int numberPoitsWaveform);



#endif //ACQUISITION_H
