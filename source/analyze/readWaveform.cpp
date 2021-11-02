#include <TStopwatch.h>
#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "readWaveform.h"

using namespace std;



int read_csv(string path_to_data_file_folder, string file_name_with_extension){
    /**
     * Ainda é necessário:
     *  - implementar verificação de um pico acima de um valor de height
     *  - implementar largura de pulso (verificar proximidade com as bordas)
    */

    // Measure elapsed time
    TStopwatch time_elapsed;



    // Read waveforms file
    fstream data_file;
    string data_file_path = path_to_data_file_folder + "/" + file_name_with_extension;
    data_file.open(data_file_path, ios::in);



    //Root file
    char* data_file_name = strtok( (char *) file_name_with_extension.c_str(), "." ); // file name without .csv extension
    string root_file_name(data_file_name); // root_file_name as a C++ string
    root_file_name += ".root"; // adds ".root" extension to file name

    string root_file_path = path_to_data_file_folder + "/" + root_file_name;
    TFile* root_file = new TFile(root_file_path.c_str(), "recreate");



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



    string row;         // contend of row on the data-file
    int row_number = 0; // row-index of the data-file

    while( std::getline(data_file, row) ){// reads the entire csv file, line by line
        /**
         * While this program reads the data-file, it also searchs for the occourrence of
         * the peaks and calculates the pulse integrals aswell. 
        */

        if(row_number>=1){ // discard the columns-names row

            char* token = strtok( (char *) row.c_str(), "," ); // separate by ","
            char* event_name = token; // name of the event, as a char*
            printf("%s ", event_name);

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

            t_peaks->Fill();
        }

        row_number += 1;
    }

    root_file->Write();
    root_file->Close();

    printf("\n\n\n");
    time_elapsed.Print();

    return 0;

}



void readWaveform(){

    read_csv("../data", "99_eventos_T.csv");

}
