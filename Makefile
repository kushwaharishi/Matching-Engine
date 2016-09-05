CC=g++
CFLAGS=-I.
DEPS = SymbolStack.h
OBJ = SymbolStack.o MatchingEngine.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

matching: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) -lpthread

clean:
	rm -f *.o matching 
