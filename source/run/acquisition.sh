#====================================================================================================
# Recompile the .o files
printf "\n\n===== Recompiling... =====\n\n"
cd ../Acquisition
make clear
make acquisition.o
make setparameters.o
cp acquisition.o ../run/exec
cp setparameters.o ../run/exec





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

    status=0


    while true; do

        sleep 2
        
        ((numberOfTries++))

        exec/setparameters.o $1

        if [ $? -eq 0 ]; then

            status=0

            break
        fi

        if [[ $numberOfTries -gt 3 ]]; then

            printf "\n\n\nProblema no Set dos Parâmetros do Osciloscópio. Foram feitas $numberOfTries tentativas\n\n\n"

            exit 1
        fi
    done
}





#====================================================================================================
# Run acquisition
printf "\n\n===== Running acquisition... =====\n\n"
cd ../run



#----------------------------------------------------------------------------------------------------
# Create folder and time variables

CURRENT_DAY=$(date +"%Y%m%d")

NOW_AS_DATE=$(date +"%Y%m%d_%H%M%S")

FOLDER=../data/$NOW_AS_DATE

mkdir $FOLDER

numberOfTries=0



#----------------------------------------------------------------------------------------------------
SetScopeParameters_MultipleTries $FOLDER



#----------------------------------------------------------------------------------------------------
# Run an infinite acquisition
while true; do


    # Se o dia virou:
    #   Feche o diretório atual
    #   Atualize os valores
    #   Tente SetScopeParameters
    if [[ $"CURRENT_DAY" < $(date +"%Y%m%d") ]]; then

        zip -r "../data/$NOW_AS_DATE.zip" $FOLDER

        rm -rf $FOLDER


        CURRENT_DAY=$(date +"%Y%m%d")

        NOW_AS_DATE=$(date +"%Y%m%d_%H%M%S")

        FOLDER=../data/$NOW_AS_DATE

        mkdir $FOLDER


        sleep 2


        SetScopeParameters_MultipleTries $FOLDER
    fi



    # Wait for a few seconds
    sleep 5



    # Execute o script da aquisição

    exec/acquisition.o $FOLDER |& tee ../data/output/output.txt

    if [ $? -ne 0 ]; then

        ((numberOfTries++))

        if [[ $numberOfTries -gt 3 ]]; then

            printf "\n\n\nACQUISITION FAILED!!! Number of retries: $numberOfTries times.\n\n\n"

            zip -r "../data/$NOW_AS_DATE.zip" $FOLDER

            rm -rf $FOLDER

            break
        fi
    
    else

        numberOfTries=0
        
    fi



done

printf "\n\n\nFinishing acquisition.\n\n"



# SEND EMAIL
cd ../email/
python3 SendEmail.py
