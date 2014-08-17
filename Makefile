CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=src/Wav_file.cpp src/main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=wh

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -rf src/*o $(EXECUTABLE)
