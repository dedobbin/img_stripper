EXE_FILES=main.cpp
EXE_OUTPUT=img_stripper
OBJS=helpers.o params.o img_stripper.o main.o
#COMPILER_FLAGS = -w -g 
COMPILER_FLAGS = -w 
LINKER_FLAGS = -lstdc++ -lwav_hammer -lm `pkg-config --cflags --libs opencv` 
GCC=g++

all:  $(OBJS)
	$(CC) $(OBJS) -o $(EXE_OUTPUT) $(LINKER_FLAGS)
helpers.o: helpers.cpp
	$(CC) -c helpers.cpp $(COMPILER_FLAGS)
params.o: params.cpp
	$(CC) -c params.cpp $(COMPILER_FLAGS)
main.o: main.cpp
	$(CC) -c main.cpp $(COMPILER_FLAGS)
img_stripper.o: img_stripper.cpp
	$(CC) -c img_stripper.cpp $(COMPILER_FLAGS)
clean:
	rm -f *.o *.a img_stripper