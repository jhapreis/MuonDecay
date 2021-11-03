CC = gcc
CPP = g++
CFLAGS = -g3 -Wall
CPPFLAGS = -g -Wall
ROOTFLAGS = `root-config --cflags --libs`
LIBSFOLDER = ~/GitHub/MuonDecay/source/libs
PARENTFOLDER = ~/GitHub/MuonDecay/source/analyze



all: run

analyze.o: analyze.cpp analyze.h
	$(CPP) $(CPPFLAGS) -c analyze.cpp $(ROOTFLAGS)
	cp analyze.o $(LIBSFOLDER)

readWaveform.o: analyze.o
	$(CPP) $(CPPFLAGS) -o readWaveform.o readWaveform.cpp analyze.o $(ROOTFLAGS)
	cp readWaveform.o $(LIBSFOLDER)
# rm readWaveform.o

libstaticanalyze.a: analyze.o
	ar rcs libstaticanalyze.a analyze.o

run: readWaveform.o
	$(LIBSFOLDER)/readWaveform.o

clear:
	rm *.o *.so *.a *.pcm *.d *.stackdump *.exe *.root
