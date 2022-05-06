#!/bin/bash


#====================================================================================================

python3 muondecay/unify.py "$@" || exit

python3 muondecay/muondecay.py "$@" || exit
