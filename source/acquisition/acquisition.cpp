#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <visa.h>

#include "../configs/cfg.h"
#include "acquisition.h"



int main(){

    // Error string
    char err[32];

    // Number of good samples collected until then
    int numberGoodSamples = 0;

    // Waveform curve, as an event
    int* WaveformAsInt_Single = NULL;

    // Waveforms array, pointer to pointer
    int* *WaveformAsInt_Array = (int**) calloc(Acquisition_NumberOfSamplesBuffer, sizeof(int*));

    // Waveform curve in mili volts
    double* Waveform_MiliVolts = NULL;

    // Scope parameters
    ViSession rm      = VI_NULL;
    ViSession scope   = VI_NULL;
    ViStatus status   = VI_NULL;
    ViUInt32 retCount = VI_NULL;

    // Buffer;  > Scope_NumberADChannels
    ViChar buffer[4*Scope_NumberADChannels];




    /**
     * @brief Open a default session and the USB device.
     * Handling situations in case of error or in case of success.
     */

    status = viOpenDefaultRM(&rm);
    status = viOpen(rm, USB_Instrument, VI_NULL, VI_NULL, &scope);

    if(status < VI_SUCCESS){
        strcpy(err, "Error opening device");
        goto error;
    }
    printf("\n\n\n      Oscilloscope RM: open.\n");




    /**
     * @brief Run a SetScopeParameters function in order to prepare for
     * acquisition. After that, tries to read an IDN query. If failed, exits. 
     */
    printf("\n      Setting Scope Parameters...\n");
    Set_ScopeParameters(status, scope, retCount);

    status = viWrite(scope, (ViBuf) "*IDN?\n"     , 6             , &retCount);
    status = viRead( scope, (ViBuf) buffer        , sizeof(buffer), &retCount);
    if(status < VI_SUCCESS){
        strcpy(err, "Error opening device");
        goto error;
    }
    printf("ID: %s\n\n      ...done\n\n", buffer);




    /**
     * @brief The following block of code stands for the acquisition.
     * It will fill a buffer-array-like with data (maybe good, maybe not so good).
     * After the buffer is filled, the code is going to evaluate the data and 
     * (a) save on the ROOT TTree or (b) discard the event.
     * 
     * -- While numberGoodSamples < NecessarySamples, search for a new sample.
     * -- Fill buffer
     * -- Analyze from buffer
     * -- Save good events
     * -- Repeat
     */

    printf("      Collecting data... %d events necessary.\n", Acquisition_NecessarySamples);

    while(numberGoodSamples < Acquisition_NecessarySamples){


        // Resets buffer
        memset(buffer, 0, sizeof(buffer));
  

        // Query for curve
        status = viWrite(scope, (ViBuf) "CURVE?\n", 7             , &retCount);
        status = viRead( scope, (ViBuf) buffer    , sizeof(buffer), &retCount);
        if( status < VI_SUCCESS ){
            printf("error on query: status < VI_SUCCESS: \n");
            continue;
        } 


        // Unpacking curve into int array
        WaveformAsInt_Single = Get_CurveData(buffer);
        if( WaveformAsInt_Single == NULL ){
            printf("error on query: waveform unpacking error: \n");
            continue;
        }


        // Convert to mili-volts
        // Waveform_MiliVolts   = Convert_WaveformMiliVolts(WaveformAsInt_Single, Scope_NumberADChannels);
        // if( Waveform_MiliVolts == NULL ){
        //     printf("error on query: waveform conversion error: \n");
        //     continue;
        // };
        

        // If no errors, then we got a good event
        WaveformAsInt_Array[numberGoodSamples] = WaveformAsInt_Single;
        numberGoodSamples++;
    }


    /**
     * @brief Free arrays and dinamically allocated pointers and finish execution 
     */

    printf("      Finishing...\n\n");

    free(WaveformAsInt_Single);
    free(Waveform_MiliVolts);


    return 0;
    
    
    
    error:
        // Report error and clean up
        viStatusDesc(scope, status, buffer);
        fprintf(stderr, "\n       %s: failure: %s\n", err, buffer);
        if(rm != VI_NULL){
            viClose(rm);
        }
        return 1;  
}
