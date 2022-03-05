#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TImage.h>
#include <TLatex.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../Acquisition/cfg.h"
#include "analyze.h"



//====================================================================================================

int ExponentialFit(const char* path_to_root_file){

    float time_difference     = 0;
    float time_difference_min = 0;
    float time_difference_max = 0;



    /* TFile with data */
    TFile* input  = new TFile(path_to_root_file, "UPDATE");



    /* TTrees within the .root file */
    // TTree* tree_integrals = (TTree*) input->Get("tree_integrals"); // TTree for integrals
    TTree* tree_time_differences = (TTree*) input->Get("tree_time_differences");
    tree_time_differences->SetBranchAddress("delta_t", &time_difference);
    tree_time_differences->SetBranchAddress("min", &time_difference_min);
    tree_time_differences->SetBranchAddress("max", &time_difference_max);



    /* Number of entries on the peaks */
    int number_peaks_entries = tree_time_differences->GetBranch("delta_t")->GetEntries();
    printf("number_peaks_entries = %d\n", number_peaks_entries);



    /* Integrals values */
    // float integral_1 = 0, integral_2 = 0; // integral values for the pulse
    // tree_integrals->SetBranchAddress("integral_1", &integral_1);
    // tree_integrals->SetBranchAddress("integral_2", &integral_2);
    // int numer_integrals_entries = tree_integrals->GetEntries(); // Number of entries on the integrals


    
    
    TCanvas* c = new TCanvas(); // Canvas



    /* Histogram for the time differences (delta_t) */
   
    time_difference_max = tree_time_differences->GetBranch("max")->GetEntry(1);
    time_difference_min = tree_time_differences->GetBranch("min")->GetEntry(1);
    printf("%f %f\n", time_difference_min, time_difference_max);

    TH1F* hist_time_difference = new TH1F(
        "hist_time_difference", 
        "Time difference from #mu to e", 
        NumberBins_HistogramTimeDifference, 
        0.15,
        9.6
        );
    hist_time_difference->GetXaxis()->SetTitle("Time difference #Deltat (#mus)");
    hist_time_difference->GetYaxis()->SetTitle("Counting rate dN/dt");



    /* Unpack all values into the variables and calculate time differences */
    for(int i=0; i<number_peaks_entries; i++){
        tree_time_differences->GetBranch("delta_t")->GetEntry(i);
        hist_time_difference->Fill(time_difference);
    }



    /* Fit curve */
    float bins_centers; int bins_values;
    TGraph* fit_curve = new TGraph();
    fit_curve->SetMarkerStyle(12);



    /* Fit function */
    ExpFitClass* FitFunction = new ExpFitClass();
    TF1* f = new TF1("A_expX_C", FitFunction, &ExpFitClass::A_ExpX_C, 0, 10, 3);
    f->SetParNames("A", "tau", "constant");
    f->SetParameters(200, 2, 3);
    f->SetLineColor(kRed);
    f->SetLineStyle(2);



    /* Remember that bin_0 = underflow and bin_n+1 = overflow */
    for(int i=1; i<NumberBins_HistogramTimeDifference; i++){
        bins_centers = hist_time_difference->GetXaxis()->GetBinCenter(i);
        bins_values  = hist_time_difference->GetBinContent(i);
        fit_curve->SetPoint(i, bins_centers, bins_values);
    }
    fit_curve->Fit(f);



    hist_time_difference->DrawClone();
    fit_curve->DrawClone("PSame");

    input->WriteObject(c, "canvas");
    input->WriteObject(hist_time_difference, "hist_time_difference");
    input->WriteObject(fit_curve, "fit_curve");

    input->Close();

    return 0;
}



//====================================================================================================

int WaveformPlots(const char* path_to_data_file){
    /**
     * Ainda é necessário implementar a leitura do arquivo .csv e 
     * subsequente plot dos valores.
    */ 


    /* Graphic for the waveforms plot */
    TGraph* waveforms_plot = new TGraph();
    waveforms_plot->GetXaxis()->SetTitle("time");
    waveforms_plot->GetYaxis()->SetTitle("y");
    waveforms_plot->SetMarkerSize(5);
    waveforms_plot->SetMarkerStyle(3);

    return 0;
}
