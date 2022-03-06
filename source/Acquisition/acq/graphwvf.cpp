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
#include "../../Configs/cfg.h"



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