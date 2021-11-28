CPP  	   = g++ 
CPPFLAGS   = -g3 -Wall
LIBS 	   = -L. -lvisa
CLEARFLAGS = *.o
ROOTFLAGS  = `root-config --cflags --libs`

idn.o: idn.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

readWfm.o: readwvf.cpp
	$(CPP) $(CPPFLAGS) -c $^ $(LIBS)

acquisition.o: scopeParameters.cpp readwvf.cpp convertValues.cpp findPeaks.cpp acquisition.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)
	cp ./acquisition.o ../libs/

interactive.o: scopeParameters.cpp readwvf.cpp convertValues.cpp findPeaks.cpp interactive.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)
	cp ./interactive.o ../libs/

clear:
	rm $(CLEARFLAGS)
