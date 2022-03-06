# Recompile the acquisition.o file
printf "\n\n===== Recompiling acquisition.o =====\n\n"
cd ../
cd Acquisition
make clear
make acquisition.o
cp acquisition.o ../run/libs
cd ../
cd run/


# Run acquisition
printf "\n\n===== Running acquisition =====\n\n"
libs/acquisition.o |& tee ../data/output/output.txt

# SEND EMAIL
cd ../
cd email/
python3 SendEmail.py
