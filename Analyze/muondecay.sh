#!/bin/bash

# bash muondecay.sh origin_folder destination_folder results_file_name_with_extension

#====================================================================================================

python3 muondecay/unify.py "$@" || { printf '\n\n\nFATAL ERROR\n\n\n'; exit 1; }

python3 muondecay/muondecay.py "$@" || { printf '\n\n\nFATAL ERROR\n\n\n'; exit 1; }
