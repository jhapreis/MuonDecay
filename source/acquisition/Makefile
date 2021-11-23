CPP  	   = g++ 
CPPFLAGS   = -g3 -Wall
LIBS 	   = -L. -lvisa
CLEARFLAGS = *.o *.so *.a
ROOTFLAGS  = `root-config --cflags --libs`

idn.o: idn.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

readWfm.o: readwvf.cpp
	$(CPP) $(CPPFLAGS) -c $< $(LIBS)

acquisition.o: scopeParameters.cpp readwvf.cpp acquisition.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTFLAGS)

interactive.o: scopeParameters.cpp readwvf.cpp interactive.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clear:
	rm $(CLEARFLAGS)
