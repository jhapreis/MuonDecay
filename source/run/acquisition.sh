# Recompile the acquisition.o file
printf "\n\n===== Recompiling acquisition.o =====\n\n"
cd ../Acquisition
make clear
make acquisition.o
cp acquisition.o ../run/exec



# Run acquisition
printf "\n\n===== Running acquisition =====\n\n"
cd ../run



# Run an infinite acquisition
while true; do

    # Wait for a few seconds
    sleep 5

    # Get time to use as folder name
    NOW_AS_DATE=$( date +"%Y%m%d_%H%M%S" ) 
    FOLDER=../data/$NOW_AS_DATE

    mkdir $FOLDER

    exec/acquisition.o $FOLDER |& tee ../data/output/output.txt

    statusAcquisition=0


    if [ $? -ne 0 ]; then

        ((statusAcquisition++))

        if [ $statusAcquisition -eq 3 ]; then

            printf "\n\n\nACQUISITION FAILED!!! Number of retries: $statusAcquisition times.\n\n\n"

            rm -rf $FOLDER

            break
        fi

        rm -rf $FOLDER

        continue
    fi


    zip -r "../data/$NOW_AS_DATE.zip" $FOLDER

    rm -rf $FOLDER
done

printf "\n\n\nFinishing acquisition.\n\n"



# SEND EMAIL
cd ../email/
python3 SendEmail.py
