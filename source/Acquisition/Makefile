CPP  	   = g++ 
CPPFLAGS   = -g3 -Wall
LIBS 	   = -L. -lvisa
CLEARFLAGS = *.o
ROOTFLAGS  = `root-config --cflags --libs`

all: acquisition.o

readWfm.o: acq/readwvf.cpp
	$(CPP) $(CPPFLAGS) -c $^ $(LIBS)

acquisition.o: acq/scopeParameters.cpp acq/readwvf.cpp acq/convertValues.cpp acq/searchPeaks.cpp Acquisition.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)

interactive.o: acq/scopeParameters.cpp acq/readwvf.cpp acq/convertValues.cpp acq/searchPeaks.cpp interactive.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)

clear:
	rm $(CLEARFLAGS)
