#!/bin/bash

PATH_DESTINATION="."
PATH_CFG_ACQ="configs/acquisition.yaml"
PATH_CFG_SCOPE="configs/scope.yaml"
LOG_LEVEL="info"
LOG_FILENAME="./logs/run.log"


NOW_AS_DATE=$(date +"%Y%m%d_%H%M%S")

FOLDER=$PATH_DESTINATION/$NOW_AS_DATE

mkdir $FOLDER

export PYTHONPATH=${PYTHONPATH}:muondecay-acquisition

python3 \
    ${FOLDER} \
    ${PATH_CFG_ACQ} \
    ${PATH_CFG_SCOPE} \
    ${LOG_LEVEL} \
    ${LOG_FILENAME} \
    ./run/run_acquisition.py
