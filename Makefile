EXE_FILES=main.cpp
EXE_OUTPUT=img_stripper
COMPILER_FLAGS = -w -g 
LINKER_FLAGS = -lstdc++ -lwav_hammer -lm `pkg-config --cflags --libs opencv` 
GCC=g++

#standalone for now
all: main.cpp
	$(CC) main.cpp -o $(EXE_OUTPUT) $(LINKER_FLAGS) $(COMPILER_FLAGS) 
clean:
	rm -f *.o *.a img