import os

import sys

from pathlib import Path

sys.path.insert(0, str( Path().resolve() ))


from modules.root_file.delete_files import delete_root_files_in_folder

from modules.root_file.assemble_root_files import unify_data_files

from modules.graph.graph_waveforms import GraphWaveforms_File

from modules.graph.graph_names import GraphIncidence_File



"""
python3 origin_folder destination_folder results_file_name_with_extension
"""



#====================================================================================================
def run(
    origin_folder: str,
    destination_folder: str,
    results_file_name_with_extension: str
    ):
    
    
    #----------------------------------------------------------------------------------------------------
    file_name = destination_folder+'/'+results_file_name_with_extension
    
    path_to_folders = [ f"{origin_folder}/{_}/" for _ in os.listdir(origin_folder) ]
    
    
    #----------------------------------------------------------------------------------------------------
    print('Deleting files...')

    list(
        map(
            lambda x: delete_root_files_in_folder(x, 'tree_waveforms'), 
            
            path_to_folders
        )
    )        
    
    
    #----------------------------------------------------------------------------------------------------
    print('Unifying data files...')

    unify_data_files(
        path_to_folders     = path_to_folders,
        tree_name           = 'tree_waveforms',
        out_file            = 'output.txt',
        outroot_folder_path = destination_folder,
        outroot_file_path   = file_name,
        number_ADChannels   = 2500    
    )


    #----------------------------------------------------------------------------------------------------
    print('Graphing waveforms...')

    GraphWaveforms_File(
        file_name        = file_name,
        folder_path      = destination_folder,
        numberADChannels = 2500,
        tree_name        = 'tree_waveforms',
        branch_name      = 'waveforms'
    )


    #----------------------------------------------------------------------------------------------------
    print('Graphing incidence...')

    GraphIncidence_File(
        file_name      = file_name,
        folder_path    = destination_folder,
        number_of_bins = 50,
        tree_name      = 'tree_waveforms',
        branch_name    = 'names'
    )
#====================================================================================================



#====================================================================================================
def main():
    
    expected_arguments = 4
    
    
    if len(sys.argv) != expected_arguments:
        
        print(f"CRITICAL ERROR: eram esperados {expected_arguments} argumentos,\
            mas foram passados {len(sys.argv)}.\nCancelando a execucao...\n\n")
        
        return 1
    
    
    origin_folder                    = sys.argv[1]
    destination_folder               = sys.argv[2]
    results_file_name_with_extension = sys.argv[3]  
    
    
    run(
        origin_folder                    = origin_folder,
        destination_folder               = destination_folder,
        results_file_name_with_extension = results_file_name_with_extension
    )
    
    return 0
#====================================================================================================



#====================================================================================================
if __name__ == "__main__":
    
    status = main()
    
    exit(status)
#====================================================================================================
