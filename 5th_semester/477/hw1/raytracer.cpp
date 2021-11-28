#include <iostream>
#include <thread>
#include "ppm.h"
#include "models.h"

#define NUM_THREADS 4
#define IS_DEVELOPMENT 0

#if IS_DEVELOPMENT
    #include <chrono>
#endif


int main(int argc, char* argv[]) {


    #if IS_DEVELOPMENT
        auto begin = std::chrono::high_resolution_clock::now();
        std::cout << "\n" << argv[1] << "\n\n";
    #endif


    Scene::loadFromXml(argv[1]);

    for (auto itr = Scene::cameras.begin(); itr != Scene::cameras.end(); ++itr) {

        Camera &camera = *itr;
        size_t width = camera.nearPlane.imageWidth;
        size_t height = camera.nearPlane.imageHeight;
        size_t heightBoundaries[NUM_THREADS + 1];

        heightBoundaries[0] = 0;
        for (int i = 0, j = NUM_THREADS; i < NUM_THREADS; i++, j--) {
            heightBoundaries[i+1] = heightBoundaries[i] + (height - heightBoundaries[i]) / j;
        }


        #if IS_DEVELOPMENT
            auto begin_camera = std::chrono::high_resolution_clock::now();
            std::cout << "\t" << camera.nearPlane.imageName << " (" << width << "x" << height << ")\n";
        #endif


        std::vector<std::thread> threads;
        unsigned char *image = new unsigned char[width * height * 3];

        for (int i = 0; i < NUM_THREADS; i++) {
            threads.push_back(std::thread(renderQuarter, image, width, heightBoundaries[i], heightBoundaries[i + 1], camera));
        }

        for (auto itr = threads.begin(); itr != threads.end(); ++itr) {
            (*itr).join();
        }


        #if IS_DEVELOPMENT
            auto begin_write = std::chrono::high_resolution_clock::now();
            auto renderDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(begin_write - begin_camera);
            std::cout << "\tRendered the image in " << renderDuration.count() * 1e-9 << " seconds\n";
        #endif


        write_ppm(camera.nearPlane.imageName.c_str(), image, width, height);
        delete[] image;


        #if IS_DEVELOPMENT
            auto end_camera = std::chrono::high_resolution_clock::now();
            auto writeDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_camera - begin_write);
            auto cameraDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_camera - begin_camera);
            std::cout << "\tWrote to the file in " << writeDuration.count() * 1e-9 << " seconds\n";
            std::cout << "\t\x1b[32mCamera execution time: " << cameraDuration.count() * 1e-9 << " seconds\x1b[0m\n\n";
        #endif
    }


    #if IS_DEVELOPMENT
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        std::cout << "\x1b[42mTotal execution time: " << duration.count() * 1e-9 << " seconds\x1b[0m\n" << std::endl;
    #endif
}
