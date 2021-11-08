CC=g++
CFLAGS=-std=c++11 # -O3
EXENAME=raytracer
SCENESPATH=/mnt/d/metu/2021-1/CENG477/hw1/sample_scenes/inputs
SCENE=simple

all: parser.cpp parser.h ppm.cpp ppm.h raytracer.cpp tinyxml2.cpp tinyxml2.h
	$(CC) $(CFLAGS) -o $(EXENAME) parser.cpp parser.h ppm.cpp ppm.h raytracer.cpp tinyxml2.cpp tinyxml2.h

run:
	./$(EXENAME) $(SCENESPATH)/$(SCENE).xml

clean:
	rm $(EXENAME)
