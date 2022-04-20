import os 

from ROOT import TFile



#====================================================================================================
def delete_root_files_in_folder(folder, tree_name='tree_waveforms'):
    """
    Given path to folder, remove the empty root files.

    Args:
        folder (string)   : path to folder
        tree_name (string): name of the TTree
    """
      
    
    root_files  = [i for i in os.listdir(folder) if i.endswith(".root")]
    

    for i in range( len(root_files) ):
        
        
        file = folder+'/'+root_files[i]
                
        tree_exists = delete_blank_root_file(file, tree_name)
        
        if tree_exists == False:
            
            print(f'   {file}...removed')



#====================================================================================================
def delete_blank_root_file(path_to_root_file, tree_name):
    """
    Delete file if the given TTree is not found on it
    """
    
    tree_exists = True
    
    
    try:
        file        = TFile(path_to_root_file, "read")
        
    except:
        tree_exists = False
    
    else:
        tree_exists = file.GetListOfKeys().Contains(tree_name)
                
        file.Close()
    
    
    if tree_exists == False:

        if os.path.isfile(path_to_root_file):
            
            os.remove(path_to_root_file)
            
        else:
            
            print(f"Error: {path_to_root_file} file not found")


    return tree_exists
