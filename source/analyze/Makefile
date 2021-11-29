CC 			 = gcc
CPP 		 = g++
CFLAGS 		 = -g3 -Wall
CPPFLAGS 	 = -g -Wall
ROOTFLAGS  	 = `root-config --cflags --libs`
LIBS 	     = -L. -lvisa
CLEARFLAGS   = *.o


all: analyze.o

libanalyze.o: expFit.cpp findPeaks.cpp readwfm_root.cpp readwfm_csv.cpp 
	$(CPP) $(CPPFLAGS) -c $@ $^ $(LIBS) $(ROOTFLAGS)

analyze.o:  expFit.cpp findPeaks.cpp readwfm_root.cpp readwfm_csv.cpp analyze.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)

memory: analyze.o
	valgrind --leak-check=full --log-file="logfile.out" ./$^

clear:
	rm $(CLEARFLAGS)
