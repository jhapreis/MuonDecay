import sys

from modules.graph_waveforms import GraphWaveforms_Folder

from modules.delete_files import delete_root_files_in_folder



#----------------------------------------------------------------------------------------------------
if len(sys.argv) <= 1:
    
    print("Error: you must pass the folder(s) path(es) as parameters when calling the script, but no argument was found.\n")
    
    exit(1)


#----------------------------------------------------------------------------------------------------
tree_name = 'tree_waveforms'

for folder in sys.argv[1:]:
    
    print(f'Graphing {folder}')
    
    delete_root_files_in_folder(folder, tree_name)
    
    GraphWaveforms_Folder(folder)
