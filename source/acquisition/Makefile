CPP  	   = g++ 
CPPFLAGS   = -g3 -Wall
LIBS 	   = -L. -lvisa
CLEARFLAGS = *.o *.so *.a

idn.o: idn.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

readWfm.o: readwvf.cpp
	$(CPP) $(CPPFLAGS) -c $< $(LIBS)

acquisition.o: scopeParameters.cpp readwvf.cpp acquisition.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

interactive.o: scopeParameters.cpp readwvf.cpp interactive.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clear:
	rm $(CLEARFLAGS)
