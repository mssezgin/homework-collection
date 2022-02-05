all:
	g++ Main.cpp EclipseMap.cpp Shader.cpp -o hw3 -std=c++11 -lXi -lGLEW -lGLU -lm -lGL -lm -lpthread -ldl -ldrm -lXdamage  -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -ljpeg

hw3: Main.cpp EclipseMap.h EclipseMap.cpp Shader.h Shader.cpp
	g++ Main.cpp EclipseMap.cpp Shader.cpp -o hw3 -std=c++11 -lXi -lGLEW -lGLU -lm -lGL -lm -lpthread -ldl -ldrm -lXdamage  -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -ljpeg

run:
	./hw3 height_map.jpg earth_texture_map.jpg moon_texture_map.jpg