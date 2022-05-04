from modules.root_file.delete_files import delete_root_files_in_folder

from modules.root_file.assemble_root_files import unify_data_files

from modules.graph.graph_waveforms import GraphWaveforms_File

from modules.graph.graph_names import GraphIncidence_File

import sys

import os



#----------------------------------------------------------------------------------------------------
if len(sys.argv) == 1:
    
    print('You must pass the data folder as an argument.\n\n')
    
    exit(1)

if len(sys.argv) == 2:
    
    folder_path = f'{sys.argv[1]}/muondecay'
    
    file_name   = folder_path+'/muondecay.root'
    
    os.makedirs(folder_path, exist_ok=True)
    
else:
    
    folder_path = '../data/muondecay'
    
    file_name   = folder_path+'/muondecay.root'



#----------------------------------------------------------------------------------------------------
print('Deleting files...')

for folder in sys.argv[1:]:
    
    delete_root_files_in_folder(folder, 'tree_waveforms')



#----------------------------------------------------------------------------------------------------
print('Unifying data files...')

unify_data_files(
    path_to_folders    = sys.argv[1:],
    tree_name          = 'tree_waveforms',
    out_file           = 'output.txt',
    outroot_folder_path= folder_path,
    outroot_file_path  = file_name,
    number_ADChannels  = 2500    
)



#----------------------------------------------------------------------------------------------------
print('Graphing waveforms...')

GraphWaveforms_File(
    file_name        = file_name,
    folder_path      = folder_path,
    numberADChannels = 2500,
    tree_name        = 'tree_waveforms',
    branch_name      = 'waveforms'
)



#----------------------------------------------------------------------------------------------------
print('Graphing incidence...')

GraphIncidence_File(
    file_name     = file_name,
    folder_path   = folder_path,
    tree_name     = 'tree_waveforms',
    branch_name   = 'names',
    number_of_bins= 50
)
