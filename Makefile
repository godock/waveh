CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp WavFile.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/wh

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -rf *o $(EXECUTABLE)
