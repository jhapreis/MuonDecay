#!/bin/bash

#====================================================================================================
# Set Scope Parameters method: calling multiple times
SetScopeParameters_MultipleTries () {

    # Set Scope Parameters:
    #   Aguarde alguns segundos
    #   Tente executar o SetScopeParameters
    #   Caso funcione, encerre
    #   Caso não funcione, tente novamente
    #       Se, após dado número de tentativas, não houver sucesso, então quebre a aquisição inteira

    numberOfTries=0


    while true; do

        sleep 2

        if python3 SetScopeParameters.py $1 $2 $3 $4; then

            break
        fi

        if [[ $numberOfTries -gt 3 ]]; then

            printf "\n\n\nCould not set the Scope Parameters (after %s tries).\n\n" "$numberOfTries" 

            exit 1
        fi
        
        ((numberOfTries++))
    done
}
#====================================================================================================


NOW_AS_DATE=$(date +"%Y%m%d_%H%M%S")

FOLDER=$1/$NOW_AS_DATE

mkdir "$FOLDER"

numberOfTries=0


# bash AcqPython.sh ../Data/raw/ configs/cfg.yaml Scope_Parameters Acquisition_Parameters

python3 SetScopeParameters.py $FOLDER $2 $3

python3 RunAcquisition.py $FOLDER $2 $3 $4
