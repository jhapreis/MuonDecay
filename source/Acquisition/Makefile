CPP  	   = g++ 
CPPFLAGS   = -g3 -Wall
LIBS 	   = -L. -lvisa
CLEARFLAGS = *.o
ROOTFLAGS  = `root-config --cflags --libs`

all: acquisition.o

idn.o: idn.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

readWfm.o: readwvf.cpp
	$(CPP) $(CPPFLAGS) -c $^ $(LIBS)

acquisition.o: scopeParameters.cpp readwvf.cpp convertValues.cpp searchPeaks.cpp Acquisition.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)

interactive.o: scopeParameters.cpp readwvf.cpp convertValues.cpp searchPeaks.cpp interactive.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)

clear:
	rm $(CLEARFLAGS)
