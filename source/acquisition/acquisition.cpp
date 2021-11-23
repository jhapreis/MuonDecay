/**
 * @file acquisition.cpp
 * @brief 
 * @version 0.1
 * @date 2021-11-23
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctime>

#include <visa.h>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TImage.h>
#include <TLatex.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../configs/cfg.h"
#include "acquisition.h"



int main(){

    // Error string
    char err[32];

    // ROOT file name as time epoch
    char root_FileName[16];

    // Number of good samples collected until then
    int numberGoodSamples = 0;

    // Waveform curve, as an event
    int* WaveformAsInt = NULL;

    // Scope parameters
    ViSession rm      = VI_NULL;             // Resource Manager
    ViSession scope   = VI_NULL;             // Oscilloscope
    ViStatus status   = VI_NULL;             // failure or success
    ViUInt32 retCount = VI_NULL;             // retCount
    ViChar buffer[4*Scope_NumberADChannels]; // Buffer; size > Scope_NumberADChannels




    /**
     * @brief ROOT FILE AND TTREES
     *  
     * Create ROOT file and set the TTree to store the 
     * required waveforms. 
     */

    sprintf(root_FileName, "../data/%lu.root", time(NULL));
    TFile* root_file = new TFile(root_FileName, "CREATE");

    TTree* tree_waveforms = new TTree("tree_waveforms", "waveforms");
    TBranch* branch_wvfm = NULL; // New branch to store every waveform, one per branch
    int event_point = 0;         // Data point from the waveform
    char event_name[20];         // Name of the event, as "event_[number]"




    /**
     * @brief CONNECT TO OSCILLOSCOPE
     *  
     * Open a default session and the USB device.
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
     * @brief SET SCOPE PARAMETERS 
     * 
     * Run a SetScopeParameters function in order to prepare for
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
     * @brief ACQUISITION 
     * 
     * The following block of code stands for the acquisition.
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
        WaveformAsInt = Get_CurveData(buffer);
        if( WaveformAsInt == NULL ){
            printf("error on query: waveform unpacking error: \n");
            continue;
        }
        

        /**
         * @brief SAVING DATA 
         *    If no errors, then we got a good event. We create a new branch 
         * on the TTree and name it as the number of the event.
         *    After that, we read the current waveform and store the values 
         * on the ROOT file (point to point).
         *    Then, free the current waveform data.
         */
        sprintf(event_name, "event_%d", numberGoodSamples);
        branch_wvfm = tree_waveforms->Branch(event_name, &event_point, "event/I");

        for(int i=0; i<Scope_NumberADChannels; i++){
            event_point = WaveformAsInt[i];
            branch_wvfm->Fill();
        }
        numberGoodSamples++;

        free(WaveformAsInt);
    }


    /**
     * @brief Finish execution 
     */

    printf("      Finishing...\n\n");

    root_file->Write();
    root_file->Close();

    return 0;
    
    

    
    error:
        // Report error and clean up
        viStatusDesc(scope, status, buffer);
        fprintf(stderr, "\n       %s: failure: %s\n", err, buffer);
        if(rm != VI_NULL){
            viClose(rm);
        }
        root_file->Write();
        root_file->Close();
        return 1;  
}
