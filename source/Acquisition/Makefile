CPP  	    = g++ 
CPPFLAGS    = -g3 -Wall
LIBS 	    = -L. -lvisa
CFILES      = acq/scopeParameters.cpp acq/readwvf.cpp acq/convertValues.cpp acq/searchPeaks.cpp
CLEARFLAGS  = *.o
ROOTFLAGS   = `root-config --cflags --libs`
DESTINATION = exec/

all: acquisition.o

readWfm.o: acq/readwvf.cpp
	$(CPP) $(CPPFLAGS) -c $^ $(LIBS)

acquisition.o: $(CFILES) Acquisition.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)
	mv $@ $(DESTINATION)

setparameters.o: $(CFILES) SetScopeParameters.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)
	mv $@ $(DESTINATION)

interactive.o: $(CFILES) interactive.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)
	mv $@ $(DESTINATION)

clear:
	cd $(DESTINATION)
	rm $(CLEARFLAGS)
	cd ../
