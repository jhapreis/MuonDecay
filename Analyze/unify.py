from modules.root_file.assemble_root_files import unify_data_files

from modules.graph.graph_waveforms import GraphWaveforms_File

from modules.graph.graph_names import GraphIncidence_File

import sys



#----------------------------------------------------------------------------------------------------
unify_data_files(
    sys.argv[1:]    
)



#----------------------------------------------------------------------------------------------------
GraphWaveforms_File(
    file_name        = 'muondecay.root',
    folder_path      = './',
    numberADChannels = 2500,
    tree_name        = 'tree_waveforms',
    branch_name      = 'waveforms'
)



#----------------------------------------------------------------------------------------------------
GraphIncidence_File(
    file_name     = 'muondecay.root',
    folder_path   = './',
    tree_name     = 'tree_waveforms',
    branch_name   = 'names',
    number_of_bins= 50
)
