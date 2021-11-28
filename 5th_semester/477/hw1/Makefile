CC=g++
CFLAGS=-std=c++11 -O3 -pthread
EXENAME=raytracer
FILEPATH=./inputs_v4
FILE=simple_shading.xml
PPMS=berserker bunny Car Car_front cornellbox_front cornellbox_inverse cornellbox_top cow dragon_lowres horse_and_mug killeroo low_poly_scene mirror_spheres monkey simple simple_shading simple_reflectance

all: parser.cpp parser.h ppm.cpp ppm.h raytracer.cpp tinyxml2.cpp tinyxml2.h models.cpp models.h
	$(CC) $(CFLAGS) -o $(EXENAME) parser.cpp ppm.cpp raytracer.cpp tinyxml2.cpp models.cpp

run:
	./$(EXENAME) $(FILEPATH)/$(FILE)

convert:
	for PPM in $(PPMS) ; do \
		convert $$PPM.ppm $$PPM.png ; \
	done

clean:
	rm $(EXENAME)
