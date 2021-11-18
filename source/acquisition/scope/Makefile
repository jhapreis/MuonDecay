CPP  	   = g++ 
CPPFLAGS   = -g3
LIBS 	   = -L. -lvisa
CLEARFLAGS = *.o *.so *.a

simple.o: simple.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)



clear:
	rm $(CLEARFLAGS)
