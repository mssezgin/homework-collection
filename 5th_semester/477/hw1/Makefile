CC=g++
CFLAGS=-std=c++11 -O3
EXENAME=raytracer
FILEPATH=./inputs_v3
FILE=simple_shading.xml

all: parser.cpp parser.h ppm.cpp ppm.h raytracer.cpp tinyxml2.cpp tinyxml2.h models.cpp models.h
	$(CC) $(CFLAGS) -o $(EXENAME) parser.cpp ppm.cpp raytracer.cpp tinyxml2.cpp models.cpp

run:
	./$(EXENAME) $(FILEPATH)/$(FILE)

clean:
	rm $(EXENAME)
	rm *.ppm
