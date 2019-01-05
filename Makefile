SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
TARGET  := run.exe

all: $(OBJECTS)
	clang++ -Wall $^ -o $(TARGET)

%.o: %.cpp
	clang++ -Wall -c $< -o $@

clear:
	@rm -f *.o $(TARGET)
	@echo cleared
