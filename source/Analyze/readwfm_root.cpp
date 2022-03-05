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

int Analyze_ROOTDataFile(std::string path_to_root_file){
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



    /**
     * @brief READ ROOT FILE
     */
    printf("   Reading .root file... %s\n", path_to_root_file.c_str()); 
    char root_file[32];
    strcpy(root_file, path_to_root_file.c_str());
    TFile* input = new TFile( root_file, "UPDATE");



    /**
     * @brief READ PARAMETERS 
     * Read TTree with infos about the acquisition parameters.
     */
    TTree* tree_infos = (TTree*) input->Get("tree_infos");

    char encoder[16];
    int numberSamples      = 0;
    int numberADChannels   = 0;
    int pulseWidth         = 0;
    int min_peaks          = 0;
    float chScale          = 0;
    float chPosition       = 0;
    double triggerVolts    = 0;
    double triggerUnits    = 0;
    std::string* str       = 0;
    
    tree_infos->SetBranchAddress("NECESSARY_SAMPLES", &str);
    tree_infos->GetBranch("NECESSARY_SAMPLES")->GetEntry(0);
    sscanf(str->c_str(), "%d", &numberSamples);

    tree_infos->SetBranchAddress("NUMBER_ADCHANNELS", &str);
    tree_infos->GetBranch("NUMBER_ADCHANNELS")->GetEntry(0);
    sscanf(str->c_str(), "%d", &numberADChannels);

    tree_infos->SetBranchAddress("PULSE_WIDTH", &str);
    tree_infos->GetBranch("PULSE_WIDTH")->GetEntry(0);
    sscanf(str->c_str(), "%d", &pulseWidth);

    tree_infos->SetBranchAddress("MIN_PEAKS", &str);
    tree_infos->GetBranch("MIN_PEAKS")->GetEntry(0);
    sscanf(str->c_str(), "%d", &min_peaks);

    tree_infos->SetBranchAddress("TRIGGER:MAIN:LEVEL?", &str);
    tree_infos->GetBranch("TRIGGER:MAIN:LEVEL?")->GetEntry(0);
    sscanf(str->c_str(), "%lf", &triggerVolts);

    tree_infos->SetBranchAddress("DATA:ENCDG?", &str);
    tree_infos->GetBranch("DATA:ENCDG?")->GetEntry(0);
    strcpy(encoder, str->c_str());

    tree_infos->SetBranchAddress("CH1:SCALE?", &str);
    tree_infos->GetBranch("CH1:SCALE?")->GetEntry(0);
    sscanf(str->c_str(), "%f", &chScale);

    tree_infos->SetBranchAddress("CH1:POSITION?", &str);
    tree_infos->GetBranch("CH1:POSITION?")->GetEntry(0);
    sscanf(str->c_str(), "%f", &chPosition);

    triggerUnits = Convert_VoltsToUnits(triggerVolts, encoder, chScale, chPosition);
    printf("      Trigger in units: %f\n", triggerUnits);

    if(min_peaks == 0){
        input->Close();
        return 0;
    }
    printf("   Infos read.\n");



    /**
     * @brief READ TTREE WITH WAVEFORMS
     * Set address to the branch on the TTree with values on the tree_waveform from the ROOT file.
     */
    int* x_peaks_units    = NULL;        // array to store x-values for the peaks
    int waveformAsInt[numberADChannels]; // array to store the waveform data
    TTree* tree_waveforms = (TTree*) input->Get("tree_waveforms");
    TBranch* branch_waveforms = tree_waveforms->GetBranch("waveforms");
    branch_waveforms->SetAddress(waveformAsInt);


    /**
     * @brief NEW TTREES
     * New TTrees, to store the peaks and integrals values. 
     */
    int   x_peak_1   = 0, x_peak_2   = 0;   // x coordinates from the peaks of the pulse
    int   y_peak_1   = 0, y_peak_2   = 0;   // y coordinates from the peaks of the pulse
    float integral_1 = 0, integral_2 = 0;   // integral values for the pulse
    float time_difference = 0;
    float time_difference_min = RightLimit_HistogramTimeDifference;
    float time_difference_max = LeftLimit_HistogramTimeDifference;

    TTree* tree_peaks       = new TTree("tree_peaks"           , "peaks");
    TTree* tree_integrals   = new TTree("tree_integrals"       , "integrals");
    TTree* tree_differences = new TTree("tree_time_differences", "delta_t");

    tree_peaks->Branch("x_peak_1", &x_peak_1, "x_peak_1/I");
    tree_peaks->Branch("x_peak_2", &x_peak_2, "x_peak_2/I");
    tree_peaks->Branch("y_peak_1", &y_peak_1, "y_peak_1/I");
    tree_peaks->Branch("y_peak_2", &y_peak_2, "y_peak_2/I");

    tree_integrals->Branch("integral_1", &integral_1, "integral_1/F");
    tree_integrals->Branch("integral_2", &integral_2, "integral_2/F");

    TBranch* delta_t_values_branch = tree_differences->Branch("delta_t", &time_difference, "time_difference/F");
    TBranch* delta_t_min_branch    = tree_differences->Branch("min", &time_difference_min, "min/F");
    TBranch* delta_t_max_branch    = tree_differences->Branch("max", &time_difference_max, "max/F");

    printf("   New branchs created.\n");



    /**
     * @brief READ WAVEFORMS FROM ROOT FILE
     */

    int event_number = 0;

    while( event_number < numberSamples ){

        // Read the current waveform
        branch_waveforms->GetEntry(event_number);
        event_number++;


        // Find peaks coordinates, check matching and pass to TTree
        x_peaks_units = FindPeaks_Waveform(waveformAsInt, numberADChannels, triggerUnits, pulseWidth, min_peaks);
        if(x_peaks_units == NULL) continue;

        x_peak_1      = x_peaks_units[0]; 
        x_peak_2      = x_peaks_units[1];
        y_peak_1      = waveformAsInt[x_peak_1]; 
        y_peak_2      = waveformAsInt[x_peak_2];
        integral_1    = 0; 
        integral_2    = 0;
        
        tree_peaks->Fill();
        tree_integrals->Fill();


        // time difference
        time_difference = float(x_peak_2 - x_peak_1)*MaxTimeStampScope/Scope_NumberADChannels;
        delta_t_values_branch->Fill();
        if(time_difference < time_difference_min){
            time_difference_min = time_difference;
        }
        else if(time_difference > time_difference_max){
            time_difference_max = time_difference;
        }


        free(x_peaks_units);
    }

    delta_t_min_branch->Fill();
    delta_t_max_branch->Fill();  

    printf("   Time differences calculated.\n");


    tree_peaks->Write();
    tree_integrals->Write();
    tree_differences->Write();

    input->Close();

    printf("   File updated and closed.\n\n\n");

    return 0;
}
