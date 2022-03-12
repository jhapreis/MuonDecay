CC 			 = gcc
CPP 		 = g++
CFLAGS 		 = -g3 -Wall
CPPFLAGS 	 = -g -Wall
ROOTFLAGS  	 = `root-config --cflags --libs`
LIBS 	     = -L. -lvisa
CLEARFLAGS   = *.o


all: analyze.o

libanalyze.o: fit/expFit.cpp fit/findPeaks.cpp fit/readwfm_root.cpp fit/readwfm_csv.cpp 
	$(CPP) $(CPPFLAGS) -c $@ $^ $(LIBS) $(ROOTFLAGS)

analyze.o:  fit/expFit.cpp fit/findPeaks.cpp fit/readwfm_root.cpp fit/readwfm_csv.cpp Analyze.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)

memory: analyze.o
	valgrind --leak-check=full --log-file="logfile.out" ./$^

clear:
	rm $(CLEARFLAGS)
