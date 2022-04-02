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

int GraphWaveforms(char* folder_path, char* path_to_root_file, int numberSamples, int numberADChannels){

    /**
     * @brief Open ROOT file
     */
    TFile* file  = new TFile(path_to_root_file, "UPDATE");



    /**
     * @brief Read Waveforms from the correspondent TTree and create Canvas
     */
    int waveformAsInt[numberADChannels];
    memset(waveformAsInt, -1, sizeof(waveformAsInt));

    TTree* tree_waveforms     = (TTree*) file->Get("tree_waveforms"); // TTree with waveforms
    TBranch* branch_waveforms = tree_waveforms->GetBranch("waveforms");
    branch_waveforms->SetAddress(waveformAsInt);

    TCanvas* c1 = new TCanvas(); // Canvas



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

            gr->SetPoint(index, i, waveformAsInt[i]);

            index++;
        }

        gr->Draw("ALP");

        event_number++;
    }

    printf("%d events graphed\n", numberSamples);

    // c1->SaveAs(folder_path);
    file->Close();

    return 0;
}
