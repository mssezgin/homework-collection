CC=g++
CFLAGS=-std=c++11 -O3
EXENAME=rasterizer

rasterizer: Main.cpp Bundle.h Bundle.cpp tinyxml2.h tinyxml2.cpp
	$(CC) $(CFLAGS) -o $(EXENAME) Main.cpp Bundle.cpp tinyxml2.cpp

clean:
	rm $(EXENAME)

ppmclean:
	rm *.ppm

pngclean:
	rm *.png

imgclean:
	rm *.ppm
	rm *.png
