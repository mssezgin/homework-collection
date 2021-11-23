#include <iostream>
#include "ppm.h"
#include "models.h"

#define IS_DEVELOPMENT 1

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
        int width = camera.imageWidth;
        int height = camera.imageHeight;


        #if IS_DEVELOPMENT
            clock_t begin_camera, begin_write, end_camera;
            std::cout << "\t" << camera.imageName << " (" << width << "x" << height << ")\n";
            if ((begin_camera = clock()) == -1)
                std::cout << "\t\x1b[31mClock error occurred while calculating the camera start time.\x1b[0m\n";
        #endif


        unsigned char *image = new unsigned char[width * height * 3];
        int index = 0;

        /* Sphere &sphere = Scene::spheres[0];
        std::cout
            << "[0] centerID " << sphere.centerVertexId
            << " materialID " << sphere.materialId
            << " radius " << sphere.radius << "\n\n"; */

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {

                /* std::cout << "i " << i << " j " << j << "\t\t\t"; */
                Ray ray = camera.createRay(i, j);
                RGBColor color = ray.computeColor();
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


        write_ppm(camera.imageName.c_str(), image, width, height);
        delete[] image;


        #if IS_DEVELOPMENT
            if ((end_camera = clock()) == -1)
                std::cout << "\t\x1b[31mClock error occurred while calculating the camera end time.\x1b[0m\n";
            std::cout << "\tWrote to the file in " << (double) (end_camera - begin_write) / CLOCKS_PER_SEC << " seconds\n";
            std::cout << "\t\x1b[32mCamera execution time: " << (double) (end_camera - begin_camera) / CLOCKS_PER_SEC << " seconds\x1b[0m\n\n";
        #endif
    }

    /* std::cout
        << (int) Scene::backgroundColor.r << " "
        << (int) Scene::backgroundColor.g << " "
        << (int) Scene::backgroundColor.b << " "
        << Scene::shadowRayEpsilon << "\n"; */


    #if IS_DEVELOPMENT
        if ((end = clock()) == -1)
            std::cout << "\x1b[31mClock error occurred while calculating the total end time.\x1b[0m\n";
        std::cout << "\x1b[42mTotal execution time: " << (double) (end - begin) / CLOCKS_PER_SEC << " seconds\x1b[0m\n" << std::endl;
    #endif
}

/*
#include <iostream>
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.
    //
    // Normally, you would be running your ray tracing
    // code here to produce the desired image.

    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255,   0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };

    int width = 640, height = 480;
    int columnWidth = width / 8;

    unsigned char* image = new unsigned char [width * height * 3];

    int i = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int colIdx = x / columnWidth;
            image[i++] = BAR_COLOR[colIdx][0];
            image[i++] = BAR_COLOR[colIdx][1];
            image[i++] = BAR_COLOR[colIdx][2];
        }
    }

    write_ppm("test.ppm", image, width, height);

}
*/
