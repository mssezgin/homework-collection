#include <iostream>
#include <thread>
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
        size_t width = camera.nearPlane.imageWidth;
        size_t height = camera.nearPlane.imageHeight;
        size_t heightBoundaries[5];

        heightBoundaries[0] = 0;
        for (int i = 0, j = 4; i < 4; i++, j--) {
            heightBoundaries[i+1] = heightBoundaries[i] + (width - heightBoundaries[i]) / j;
        }


        #if IS_DEVELOPMENT
            clock_t begin_camera, begin_write, end_camera;
            std::cout << "\t" << camera.nearPlane.imageName << " (" << width << "x" << height << ")\n";
            if ((begin_camera = clock()) == -1)
                std::cout << "\t\x1b[31mClock error occurred while calculating the camera start time.\x1b[0m\n";
        #endif


        std::vector<std::thread> threads;
        unsigned char *image = new unsigned char[width * height * 3];

        for (int i = 0; i < 4; i++) {
            threads.push_back(std::thread(renderQuarter, image, width, heightBoundaries[i], heightBoundaries[i + 1], camera));
        }

        for (auto itr = threads.begin(); itr != threads.end(); ++itr) {
            (*itr).join();
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
