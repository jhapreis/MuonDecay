#!/bin/bash

#====================================================================================================
# Recompile the .o files
printf "\n\n===== Recompiling... =====\n\n"

make clear

if ! make acquisition.o; then
    exit 1
fi

if ! make setparameters.o; then
    exit 1
fi




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
        
        ((numberOfTries++))

        if ! exec/setparameters.o "$1"; then

            break
        fi

        if [[ $numberOfTries -gt 3 ]]; then

            printf "\n\n\nProblema no Set dos Parâmetros do Osciloscópio. Foram feitas %s tentativas\n\n\n" "$numberOfTries" 

            exit 1
        fi
    done
}




#====================================================================================================
# Run acquisition:
#   - Execute múltiplas tentativas de SetScopeParameters
#   - Inicie a aquisição com o script em cpp
#   - Caso dê algum erro, feche o folder e recomece
#   - Caso o erro persista, encerre e avise com um email
printf "\n\n===== Running acquisition... =====\n\n"

NOW_AS_DATE=$(date +"%Y%m%d_%H%M%S")

FOLDER=../data/$NOW_AS_DATE

mkdir "$FOLDER"

numberOfTries=0


#----------------------------------------------------------------------------------------------------
# Try to SetScopeParameters
SetScopeParameters_MultipleTries "$FOLDER"


#----------------------------------------------------------------------------------------------------
# Run an infinite acquisition; the loop only breaks in case of failure
while true; do

    # Wait for a few seconds
    sleep 5
   

    # Execute o script da aquisição e verifique por erros na aquisição
    if ! exec/acquisition.o "$FOLDER" |& tee ../data/output/output.txt; then

        ((numberOfTries++))

        if [[ $numberOfTries -gt 3 ]]; then

            printf "\n\n\nACQUISITION FAILED!!! Number of retries: %s times.\n\n\n" "$numberOfTries"

            break
        fi
    
    else

        numberOfTries=0
    fi

done


# SEND EMAIL
cd ../email/ || exit 1
python3 SendEmail.py
