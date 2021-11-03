/**
 * This is a file with the implementation of the functions and auxiliars functions 
 * for the readWaveform.cpp file
 */
#include <TStopwatch.h>
#include <TFile.h>
#include <TTree.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../configs/cfg.h"
#include "analyze.h"
using namespace std;





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
     *      Ele gera duas TTrees e preenche os valores de pico e da integral dos pulsos encontrados.
     * 
     * 
     * 
     * Ainda é necessário:
     *  - implementar verificação de um pico acima de um valor de height
     *  - implementar largura de pulso (verificar proximidade com as bordas)
    */

    // Measure elapsed time
    TStopwatch time_elapsed;



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
    time_elapsed.Print();

    return 0;
}



void root_file(char* root_file, std::string path_to_data_file_folder, std::string file_name_with_extension){
    
    // file name without .csv extension
    char* data_file_name = strtok( (char *) file_name_with_extension.c_str(), "." );

    // root_file_name as a C++ string
    std::string root_file_name(data_file_name);

    // adds ".root" extension to file name
    root_file_name += ".root";

    // full path name
    std::string root_file_path_string = path_to_data_file_folder + "/" + root_file_name;
    
    // root file path as char*
    const char* root_file_path = &root_file_path_string[0];
    
    // copy int the original root_file variable, in order to modify that
    strcpy(root_file, root_file_path);    
}





