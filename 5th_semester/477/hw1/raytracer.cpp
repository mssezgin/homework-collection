#include <iostream>
#include "ppm.h"
#include "models.h"

#define IS_DEVELOPMENT true

#if IS_DEVELOPMENT
    #include <ctime>
#endif


int main(int argc, char* argv[]) {


    #if IS_DEVELOPMENT
        clock_t begin, end;
        std::cout << "\n";
        if ((begin = clock()) == -1)
            std::cout << "\x1b[31mClock error occurred while calculating the total start time.\x1b[0m\n";
        std::cout << argv[1] << "\n\n";
    #endif


    Scene::loadFromXml(argv[1]);

    for (auto itr = Scene::cameras.begin(); itr != Scene::cameras.end(); ++itr) {

        Camera &camera = *itr;
        int width = camera.nearPlane.imageWidth;
        int height = camera.nearPlane.imageHeight;


        #if IS_DEVELOPMENT
            clock_t begin_camera, begin_write, end_camera;
            std::cout << "\t" << camera.nearPlane.imageName << " (" << width << "x" << height << ")\n";
            if ((begin_camera = clock()) == -1)
                std::cout << "\t\x1b[31mClock error occurred while calculating the camera start time.\x1b[0m\n";
        #endif


        unsigned char *image = new unsigned char[width * height * 3];
        int index = 0;

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {

                Ray ray = camera.createRay(i, j);
                RGBColor color = ray.traceRay().toRGBColor();
                image[index++] = color.r;
                image[index++] = color.g;
                image[index++] = color.b;
            }
        }


        #if IS_DEVELOPMENT
            if ((begin_write = clock()) == -1)
                std::cout << "\t\x1b[31mClock error occurred while calculating the write start time.\x1b[0m\n";
            std::cout << "\tRendered the image in " << (double) (begin_write - begin_camera) / CLOCKS_PER_SEC << " seconds\n";
        #endif


        write_ppm(camera.nearPlane.imageName.c_str(), image, width, height);
        delete[] image;


        #if IS_DEVELOPMENT
            if ((end_camera = clock()) == -1)
                std::cout << "\t\x1b[31mClock error occurred while calculating the camera end time.\x1b[0m\n";
            std::cout << "\tWrote to the file in " << (double) (end_camera - begin_write) / CLOCKS_PER_SEC << " seconds\n";
            std::cout << "\t\x1b[32mCamera execution time: " << (double) (end_camera - begin_camera) / CLOCKS_PER_SEC << " seconds\x1b[0m\n\n";
        #endif
    }


    #if IS_DEVELOPMENT
        if ((end = clock()) == -1)
            std::cout << "\x1b[31mClock error occurred while calculating the total end time.\x1b[0m\n";
        std::cout << "\x1b[42mTotal execution time: " << (double) (end - begin) / CLOCKS_PER_SEC << " seconds\x1b[0m\n" << std::endl;
    #endif
}
