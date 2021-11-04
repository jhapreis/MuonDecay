CC = gcc
CPP = g++
CFLAGS = -g3 -Wall
CPPFLAGS = -g -Wall
ROOTFLAGS = `root-config --cflags --libs`
LIBSFOLDER = ~/GitHub/MuonDecay/source/libs
DATAFOLDER = ~/GitHub/MuonDecay/source/data
PARENTFOLDER = ~/GitHub/MuonDecay/source/analyze
CLEARFLAGS   = *.o *.so *.a *.pcm *.d *.stackdump *.exe *.root


all: run

analyze.o: analyze.cpp analyze.h
	$(CPP) $(CPPFLAGS) -c analyze.cpp $(ROOTFLAGS)
	cp analyze.o $(LIBSFOLDER)

readWaveform.o: analyze.o
	$(CPP) $(CPPFLAGS) -o readWaveform.o readWaveform.cpp analyze.o $(ROOTFLAGS)
	cp readWaveform.o $(LIBSFOLDER)

libstaticanalyze.a: analyze.o
	ar rcs libstaticanalyze.a analyze.o

run: readWaveform.o
	rm *.o
	$(LIBSFOLDER)/readWaveform.o 

clear:
	rm $(CLEARFLAGS)
