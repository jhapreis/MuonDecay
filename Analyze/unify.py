from modules.root_file.assemble_root_files import unify_data_files, root_files_names

import sys



unify_data_files(
    sys.argv[1:]    
)
