CC 			 = gcc
CPP 		 = g++
CFLAGS 		 = -g3 -Wall
CPPFLAGS 	 = -g -Wall
ROOTFLAGS  	 = `root-config --cflags --libs`
LIBS 	     = -L. -lvisa
CLEARFLAGS   = *.o


all: analyze.o

libanalyze.o: expFit.cpp readwfm_root.cpp readwfm_csv.cpp ../acquisition/convertValues.cpp ../acquisition/findPeaks.cpp
	$(CPP) $(CPPFLAGS) -c $@ $^ $(LIBS) $(ROOTFLAGS)

analyze.o:  expFit.cpp readwfm_root.cpp readwfm_csv.cpp ../acquisition/convertValues.cpp ../acquisition/findPeaks.cpp analyze.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)
	cp ./analyze.o ../run/libs/

memory: analyze.o
	valgrind --leak-check=full --log-file="logfile.out" ./$^

clear:
	rm $(CLEARFLAGS)
