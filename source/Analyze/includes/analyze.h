#ifndef ANALYZE_H
#define ANALYZE_H

#include <string>
#include <TF1.h>



int* FindPeaks_Waveform(int* waveformAsInt, int waveformSize, double height, int pulseWidth, int minPeaks);

double* Convert_WaveformMiliVolts(int* Waveform_Units, int numberPoitsWaveform, char* encoder, float chScale, float chPosition);

double Convert_VoltsToUnits(double valueVolts, char* encoder, float chScale, float chPosition);

//====================================================================================================
/**
 * @brief This functions recieves the path to the data file and its name (with extension)
 * and returns the full path of the relative root file, in order to create lately
 * 
 * @param path_to_data_file_folder
 * @param file_name_with_extension 
 */
void root_file(char* root_file, std::string path_to_data_file_folder, std::string file_name_with_extension);



//====================================================================================================
/**
 * @brief 
 * 
 * @param path_to_data_file_folder
 * @param file_name_with_extension
 * 
 * @returns int; success or failure
 * 
 */
int Analyze_CSVDataFile(std::string path_to_data_file_folder, std::string file_name_with_extension);



//====================================================================================================
/**
 * @brief 
 * 
 * @param path_to_root_file 
 * @return int 
 */
int Analyze_ROOTDataFile(std::string path_to_root_file);



//====================================================================================================
/**
 * @brief 
 * 
 * @param path_to_root_file
 * 
 * @returns int; success or failure
 * 
 */
int ExponentialFit(const char* path_to_root_file);



//====================================================================================================
/**
 * @brief 
 * 
 * @param x 
 * @param par 
 * @return double 
 */
double f_Aexpx_C(double x, double* par);



//====================================================================================================
/**
 * @brief 
 * 
 */
class ExpFitClass{

    public:

        double A_ExpX_C( double* t, double* parameters ){

            double x   = t[0];
            double A   = parameters[0]; // N_0/tau
            double tau = parameters[1]; // tau
            double C   = parameters[2]; // constant

            double _ = A*exp(-1*x/tau) + C;

            return _;
        }

        double N0_tau_expx_C( double* t, double* parameters ){

            double x   = t[0];
            double tau = parameters[0]; // tau
            double C   = parameters[1]; // constant

            double _ = 55.12/tau*exp(-1*x/tau) + C;

            return _;
        }
};



#endif //ANALYZE_H
