# Recompile the acquisition.o file
printf "\n\n===== Recompiling acquisition.o =====\n\n"
cd ../
cd acquisition
make clear
make acquisition.o
cp acquisition.o ../run/libs
cd ../
cd run/


# Run acquisition
printf "\n\n===== Running acquisition =====\n\n"
libs/acquisition.o |& tee ../data/output/output.txt

# SEND EMAIL
python3 SendEmail.py
