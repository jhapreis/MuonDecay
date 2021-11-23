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
     *  - incluir método que salva os eventos problemáticos:
     *      -- exemplo: salvar caso peak_x_1 = 0 ou peak_x_2 = 0
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
    float waveform[Scope_NumberADChannels];     // array to store the waveform data
    float time_difference = 0;
    float time_difference_min = RightLimit_HistogramTimeDifference;
    float time_difference_max = LeftLimit_HistogramTimeDifference;



    TTree* tree_peaks       = new TTree("tree_peaks"    , "peaks");
    TTree* tree_integrals   = new TTree("tree_integrals", "integrals");
    TTree* tree_differences = new TTree("tree_time_differences", "delta_t");



    tree_peaks->Branch("x_peak_1", &x_peak_1, "x_peak_1/I");
    tree_peaks->Branch("x_peak_2", &x_peak_2, "x_peak_2/I");
    tree_peaks->Branch("y_peak_1", &y_peak_1, "y_peak_1/F");
    tree_peaks->Branch("y_peak_2", &y_peak_2, "y_peak_2/F");

    tree_integrals->Branch("integral_1", &integral_1, "integral_1/F");
    tree_integrals->Branch("integral_2", &integral_2, "integral_2/F");

    TBranch* delta_t_values_branch = tree_differences->Branch("delta_t", &time_difference, "time_difference/F");
    TBranch* delta_t_min_branch = tree_differences->Branch("min", &time_difference_min, "min/F");
    TBranch* delta_t_max_branch = tree_differences->Branch("max", &time_difference_max, "max/F");



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
            for(int i=0; i<Scope_NumberADChannels; i++){ 
                
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

            tree_peaks->Fill(); // At every line, fill the correspondent values on the TTree
        }

        row_number += 1;
    }



    int number_peaks_entries = tree_peaks->GetEntries(); // Number of entries on the peaks

    for(int i=0; i<number_peaks_entries; i++){

        tree_peaks->GetEntry(i);

        if(x_peak_1 > 0 && x_peak_2 > 0){ // check for peak-not-found error

            time_difference = float(x_peak_2 - x_peak_1)*MaxTimeStampScope/Scope_NumberADChannels;
            delta_t_values_branch->Fill();

            if(time_difference < time_difference_min){
                time_difference_min = time_difference;
            }
            else if(time_difference > time_difference_max){
                time_difference_max = time_difference;
            }
        }
    }
    delta_t_min_branch->Fill();
    delta_t_max_branch->Fill();



    root_file->Write();
    root_file->Close();

    printf("\n\n\n");

    return 0;
}
//
//
//
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


    
    /* Canvas */
    TCanvas* c = new TCanvas();



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
