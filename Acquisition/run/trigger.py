import os

import sys

#====================================================================================================
def get_trigger_cfg_file(path_to_cfg_file: "str") -> str:


    #----------------------------------------------------------------------------------------------------
    with open(path_to_cfg_file) as f:

        contend = f.read()

        trigger = contend.split('#define Scope_ChannelTrigger ')[1].split('\n')[0]

        return trigger



#====================================================================================================
def replace_trigger_cfg_file(path_to_cfg_file: "str", trigger: "str", new_trigger: "str") -> "str":


    #----------------------------------------------------------------------------------------------------
    with open(path_to_cfg_file) as f:

        contend     = f.read()

        new_contend = contend.replace(
            f'#define Scope_ChannelTrigger {trigger}\n', 
            f'#define Scope_ChannelTrigger {new_trigger}\n'
            )
        
        return new_contend



#====================================================================================================
def rewrite_cfg_file(path_to_cfg_file: "str", new_contend: "str") -> "int":

    os.remove(path_to_cfg_file)

    with open(path_to_cfg_file, "w+") as f:

        f.write(new_contend)

    return 0



#----------------------------------------------------------------------------------------------------

if len(sys.argv) <= 1:

    print('You must pass the new trigger as the parameter.')

    exit(1)



if __name__ == '__main__':

    path_to_cfg_file = '../configs/cfg.h'

    trigger          = get_trigger_cfg_file(path_to_cfg_file)

    new_contend      = replace_trigger_cfg_file( path_to_cfg_file, trigger, str(sys.argv[1]) )

    rewrite_cfg_file(path_to_cfg_file, new_contend)
