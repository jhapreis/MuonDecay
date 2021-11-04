/**
 * This is a file with the implementation of the functions and auxiliars functions 
 * for the readWaveform.cpp file
 */
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

#include "../configs/cfg.h"
#include "analyze.h"
// using namespace std;



//==================================================
// FIRST LEVEL METHODS
//==================================================
//
int Analyze_DataFile(std::string path_to_data_file_folder, std::string file_name_with_extension){
    /**
     * Funcionamento:
     * 
     *      O código tem a função de ler os arquivos de dados, em .csv, no formato 
     * "nome-do-evento" vs "quantidade-de-waveforms", com os seus respectivos labels. 
     * 
     *      Ele funciona lendo o arquivo, linha por linha, em cada waveform e procurando
     * por valores de pico. Tal valor é procurado dentro de dois intervalos, definidos 
     * previamente. Isto é, o programa vai dividir a waveform em duas regiões e tentará
     * encontrar picos nas duas, baseando-se que o primeiro deve ocorrer até um certo ponto
     * e, o segundo, depois desse ponto.
     * 
     *      Para que um valor seja considerado "pico", ele precisa (a) estar abaixo do valor de
     * trigger (porque o slope é negativo) e (b) ser o menor valor na região (mais negativo).
     * 
     * 
     *      Ele gera duas TTrees (dentro de um arquivo .root) 
     * e preenche os valores de pico e da integral dos pulsos encontrados.
     * 
     * 
     * 
     * Ainda é necessário:
     *  - implementar integração do pulso
     *  - implementar largura de pulso (verificar proximidade com as bordas)
    */



    // Read waveforms file
    std::fstream data_file;
    std::string data_file_path = path_to_data_file_folder + "/" + file_name_with_extension;
    data_file.open(data_file_path, std::ios::in);



    //Root file
    char root_file_path[50];
    root_file(root_file_path, path_to_data_file_folder, file_name_with_extension);
    printf("   Creating .root file...\n"); 
    TFile* root_file = new TFile(root_file_path, "RECREATE");



    // TTrees, to store the results
    int   x_peak_1   = 0, x_peak_2   = 0; // x coordinates from the peaks of the pulse
    float y_peak_1   = 0, y_peak_2   = 0; // y coordinates from the peaks of the pulse
    float integral_1 = 0, integral_2 = 0; // integral values for the pulse
    float waveform[NumberADChannels];     // array to store the waveform data

    TTree* t_peaks     = new TTree("tree_peaks"    , "peaks");
    TTree* t_integrals = new TTree("tree_integrals", "integrals");

    t_peaks->Branch("x_peak_1", &x_peak_1, "x_peak_1/I");
    t_peaks->Branch("x_peak_2", &x_peak_2, "x_peak_2/I");
    t_peaks->Branch("y_peak_1", &y_peak_1, "y_peak_1/F");
    t_peaks->Branch("y_peak_2", &y_peak_2, "y_peak_2/F");

    t_integrals->Branch("integral_1", &integral_1, "integral_1/F");
    t_integrals->Branch("integral_2", &integral_2, "integral_2/F");



    printf("   Reading data-file(s)...\n");

    std::string row;    // contend of row on the data-file
    int row_number = 0; // row-index of the data-file

    while( std::getline(data_file, row) ){// reads the entire csv file, line by line
        /**
         * While this program reads the data-file, it also searchs for the occourrence of
         * the peaks and calculates the pulse integrals aswell. 
        */

        if(row_number>=1){ // discard the columns-names row

            char* token = strtok( (char *) row.c_str(), "," ); // separate by ","
            // char* event_name = token; // name of the event, as a char*
            
            // Reset variables to zero, in order to use again
            x_peak_1   = 0; x_peak_2   = 0;
            y_peak_1   = 0; y_peak_2   = 0;
            integral_1 = 0; integral_2 = 0;

            /* search on the entire waveform, starting from the event_name */
            for(int i=0; i<NumberADChannels; i++){ 
                
                token = strtok(NULL, ","); // this is the value, as a char* or NULL

                if (token != NULL){ // if NOT reached the end of the file

                    waveform[i] = atof(token); // converts value to float and assign to waveform

                    if(i <= FisrtPeak_MaxIndex && waveform[i] <= TriggerInUnits){ // the peaks are with negative slope
                        if(waveform[i] < y_peak_1){
                            x_peak_1 = i;
                            y_peak_1 = waveform[i];
                        }
                    }
                    else if(i > FisrtPeak_MaxIndex && waveform[i] <= TriggerInUnits){ // the peaks are with negative slope
                        if(waveform[i] < y_peak_2){
                            x_peak_2 = i;
                            y_peak_2 = waveform[i];
                        }
                    }
                }
            }

            t_peaks->Fill(); // At every line, fill the correspondent values on the TTree
        }

        row_number += 1;
    }

    root_file->Write();
    root_file->Close();

    printf("\n\n\n");

    return 0;
}
//
//
//
int ExponentialFit(const char* path_to_root_file){

    /* TFile with data */
    TFile* input  = new TFile(path_to_root_file, "UPDATE");



    /* TTrees within the .root file */
    TTree* tree_peaks     = (TTree*) input->Get("tree_peaks"); // TTree for peaks
    // TTree* tree_integrals = (TTree*) input->Get("tree_integrals"); // TTree for integrals



    /* Variables to unpack the values on the TTrees */
    int   x_peak_1   = 0, x_peak_2   = 0; // x coordinates from the peaks of the pulse
    float y_peak_1   = 0, y_peak_2   = 0; // y coordinates from the peaks of the pulse
    // float integral_1 = 0, integral_2 = 0; // integral values for the pulse

    tree_peaks->SetBranchAddress("x_peak_1", &x_peak_1);
    tree_peaks->SetBranchAddress("x_peak_2", &x_peak_2);
    tree_peaks->SetBranchAddress("y_peak_1", &y_peak_1);
    tree_peaks->SetBranchAddress("y_peak_2", &y_peak_2);
    // tree_integrals->SetBranchAddress("integral_1", &integral_1);
    // tree_integrals->SetBranchAddress("integral_2", &integral_2);
    
    int number_peaks_entries    = tree_peaks->GetEntries(); // Number of entries on the peaks
    // int numer_integrals_entries = tree_integrals->GetEntries(); // Number of entries on the integrals
    
    

    /* Canvas */
    TCanvas* c = new TCanvas();

    /* Histogram for the time differences (delta_t) */
    TH1F* hist_time_difference = new TH1F(
        "hist_time_difference", "Time difference from #mu to e", NumberBins_HistogramTimeDifference, LeftLimit_HistogramTimeDifference, RightLimit_HistogramTimeDifference
        );
    hist_time_difference->GetXaxis()->SetTitle("Time difference #Deltat (#mus)");
    hist_time_difference->GetYaxis()->SetTitle("Counting rate dN/dt");
    


    /* Unpack all values into the variables and calculate time differences */
    float time_difference = 0;

    for(int i=0; i<number_peaks_entries; i++){

        tree_peaks->GetEntry(i);
        // tree_integrals->GetEntry(i);

        /* time difference, in micro-seconds */
        time_difference = float(x_peak_2 - x_peak_1)*MaxTimeStampScope/NumberADChannels;
        hist_time_difference->Fill(time_difference);

        // graphic->SetPoint(i, hist_time_difference->GetXaxis()->GetBinCenter(i), hist_time_difference->GetBin(i));
    }



    /* Fit curve */
    float bins_centers; int bins_values;
    TGraph* fit_curve = new TGraph();
    fit_curve->SetMarkerStyle(12);

    /* Fit function */
    TF1* f = new TF1("f_Aexpx_C", "[0]*exp(-x/[1]) + [2]", LeftLimit_HistogramTimeDifference, RightLimit_HistogramTimeDifference);
    f->SetParNames("N_0/tau", "tau", "constant");
    f->SetParameters(220, 2.2, 10);
    f->SetLineColor(kRed);
    f->SetLineStyle(2);



    for(int i=1; i<NumberBins_HistogramTimeDifference; i++){
        /* Remember that bin_0 = underflow and bin_n+1 = overflow */

        bins_centers = hist_time_difference->GetXaxis()->GetBinCenter(i);
        bins_values  = hist_time_difference->GetBinContent(i);

        // printf("%f %d ", bins_centers, bins_values);
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
//
//
//
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
//
//
//
//
//
//
//
//==================================================
// SECOND LEVEL (AUXILIAR) METHODS
//==================================================
//
void root_file(char* root_file, std::string path_to_data_file_folder, std::string file_name_with_extension){
    
    /* file name without .csv extension */
    char* data_file_name = strtok( (char *) file_name_with_extension.c_str(), "." );

    /* root_file_name as a C++ string */
    std::string root_file_name(data_file_name);

    /* adds ".root" extension to file name */
    root_file_name += ".root";

    /* full path name */
    std::string root_file_path_string = path_to_data_file_folder + "/" + root_file_name;
    
    /* root file path as char* */
    const char* root_file_path = &root_file_path_string[0];
    
    /* copy int the original root_file variable, in order to modify that */
    strcpy(root_file, root_file_path);    
}
//
//
//
double f_Aexpx_C(double x, double* par){
    return par[0]*exp(-x/par[1])+par[2];
}
