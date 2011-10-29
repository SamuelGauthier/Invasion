CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=-lSDL -lGL -lGLU
EXEC=ex
SRC=$(wildcard *.cpp) 
OBJ= $(SRC:.cpp=.o)

all: $(EXEC)

ex: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

bug: $(OBJ)
	$(CC) -g -o $@ $^ $(LDFLAGS)

main.o: main.cpp

%.o: %.cpp %.h
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm *.o

mrproper: clean
	rm $(EXEC)
