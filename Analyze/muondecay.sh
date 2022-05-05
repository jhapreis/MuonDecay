#!/bin/bash


#====================================================================================================

python3 muondecay/unify.py $@

python3 muondecay/muondecay.py $@
