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

#include "cfg.h"
#include "acquisition.h"



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

    if( buffer[index] != '#' ) return -1; // error on format


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
     * If the number of points doesn't match, returns -1. 
     */
    if(numberPointsCurve != Scope_NumberADChannels){
        
        printf("Error on numberPointsCurve: %d != %d\n", numberPointsCurve, Scope_NumberADChannels);
        return -1;
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



//====================================================================================================

int GraphWaveforms(char* path_to_root_file){

    /**
     * @brief Open ROOT file
     * 
     */
    TFile* input  = new TFile(path_to_root_file, "UPDATE");



    /**
     * @brief Read ROOT file and find the number of necessary samples and the number of ADchannels.
     * 
     */

    TTree* tree_infos = (TTree*) input->Get("tree_infos");

    int numberSamples    = 0;
    int numberADChannels = 0;
    std::string* str     = 0;
    
    tree_infos->SetBranchAddress("NECESSARY_SAMPLES", &str);
    tree_infos->GetBranch("NECESSARY_SAMPLES")->GetEntry(0);
    sscanf(str->c_str(), "%d", &numberSamples);

    tree_infos->SetBranchAddress("NUMBER_ADCHANNELS", &str);
    tree_infos->GetBranch("NUMBER_ADCHANNELS")->GetEntry(0);
    sscanf(str->c_str(), "%d", &numberADChannels);



    /**
     * @brief Read Waveforms from the correspondent TTree and create Canvas
     * 
     */
    int waveformAsInt[numberADChannels];
    memset(waveformAsInt, -1, sizeof(waveformAsInt));

    TTree* tree_waveforms     = (TTree*) input->Get("tree_waveforms"); // TTree with waveforms
    TBranch* branch_waveforms = tree_waveforms->GetBranch("waveforms");
    branch_waveforms->SetAddress(waveformAsInt);

    TCanvas* c = new TCanvas(); // Canvas



    /**
     * @brief GRAPHIC ON THE CANVAS
     * Read the branch on the TTree and plot on the canvas.
     */

    TGraph* gr = new TGraph();
    char graph_title[64];
    sprintf(graph_title, "%d Waveforms; time (units); value", numberSamples);
    gr->SetTitle(graph_title);
    gr->SetLineWidth(2);
    gr->SetLineColor(kBlack);

    int event_number = 0;

    int index        = 0;

    while(event_number < numberSamples){


        branch_waveforms->GetEntry(event_number);

        for(int i=0; i<numberADChannels; i++){

            // printf("%d ", waveformAsInt[i]);

            gr->SetPoint(index, i, waveformAsInt[i]);

            index++;
        }

        // printf("\n\n");

        gr->Draw("ALP");

        event_number++;
    }

    printf("%d events graphed\n", numberSamples);

    input->WriteObject(c, "waveforms");
    input->Close();

    return 0;
}
