CC=g++
FLAGS=-O3 -Wall -Wextra
SOURCES=M11215084.cpp parser.cpp kernel.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HEADERS=arg_parser.hpp parser.hpp kernel.hpp
EXECUTABLE=M11215084

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $@

.cpp.o: $(HEADERS)
	$(CC) $(FLAGS) $< -c -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)
