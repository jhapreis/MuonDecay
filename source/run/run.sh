# Recompile the acquisition.o file
printf "\n\n===== Recompiling acquisition.o =====\n\n"
cd ../
cd acquisition
make clear
make acquisition.o
cd ../
cd run/


# Run acquisition
printf "\n\n===== Running acquisition =====\n\n"
time ../libs/acquisition.o |& tee ../data/output/output.txt


# SEND EMAIL
python3 SendEmail.py
