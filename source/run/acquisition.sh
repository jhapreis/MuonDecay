# Recompile the acquisition.o file
printf "\n\n===== Recompiling acquisition.o =====\n\n"
cd ../Acquisition
make clear
make acquisition.o
cp acquisition.o ../run/libs


# Run acquisition
printf "\n\n===== Running acquisition =====\n\n"
cd ../run

NOW=$( date +%s ) # Get time epoch to use as folder name
FOLDER=../data/$NOW

mkdir $FOLDER

libs/acquisition.o $FOLDER # |& tee ../data/output/output.txt

zip -r "../data/$NOW.zip" $FOLDER

rm -rf $FOLDER


# SEND EMAIL
cd ../email/
python3 SendEmail.py
