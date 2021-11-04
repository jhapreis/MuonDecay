#ifndef ANALYZE_H
#define ANALYZE_H

#include <string>
#include <TF1.h>


/**
 * @brief This functions recieves the path to the data file and its name (with extension)
 * and returns the full path of the relative root file, in order to create lately
 * 
 * @param path_to_data_file_folder
 * @param file_name_with_extension 
 */
void root_file(char* root_file, std::string path_to_data_file_folder, std::string file_name_with_extension);



/**
 * @brief 
 * 
 * @param path_to_data_file_folder
 * @param file_name_with_extension
 * 
 * @returns int; success or failure
 * 
 */
int Analyze_DataFile(std::string path_to_data_file_folder, std::string file_name_with_extension);



/**
 * @brief 
 * 
 * @param path_to_root_file
 * 
 * @returns int; success or failure
 * 
 */
int ExponentialFit(const char* path_to_root_file);



double f_Aexpx_C(double x, double* par);



#endif //ANALYZE_H
