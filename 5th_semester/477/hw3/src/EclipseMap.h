#ifndef ECLIPSEMAP_H
#define ECLIPSEMAP_H

#include <vector>
#include <GL/glew.h>
#include <iostream>
#include "glm/glm/ext.hpp"
#include "Shader.h"
#include <vector>
#include "glm/glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <jpeglib.h>
#include <GL/glew.h>

#define PI 3.14159265359
using namespace std;


class EclipseMap {
private:
    float heightFactor = 80;
    float textureOffset = 0;
    float orbitDegree = 0;
    glm::vec3 lightPos = glm::vec3(0, 4000, 0);
    bool pKeyPressed = false;
    // DISPLAY SETTINGS
    enum displayFormatOptions {
        windowed = 1, fullScreen = 0
    };
    const char *windowName = "Ceng477 - HW3";
    int defaultScreenWidth = 1000;
    int defaultScreenHeight = 1000;
    int screenWidth = defaultScreenWidth;
    int screenHeight = defaultScreenHeight;
    int displayFormat = displayFormatOptions::windowed;
    // CAMERA SETTINGS
    float projectionAngle = 45;
    float aspectRatio = 1;
    float near = 0.1;
    float far = 10000;
    float startPitch = 180;
    float startYaw = 90;
    float startSpeed = 0;
    float pitch = startPitch;
    float yaw = startYaw;
    float speed = startSpeed;
    glm::vec3 cameraStartPosition = glm::vec3(0, 4000, 4000);
    glm::vec3 cameraStartDirection = glm::vec3(0, -1, -1);
    glm::vec3 cameraStartUp = glm::vec3(0, 0, 1);
    glm::vec3 cameraUp = cameraStartUp;
    glm::vec3 cameraPosition = cameraStartPosition;
    glm::vec3 cameraDirection = cameraStartDirection;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

public:
    int horizontalSplitCount = 250;
    int verticalSplitCount = 125;

    unsigned int worldTextureColor;
    unsigned int worldTextureGrey;
    unsigned int worldVAO;
    unsigned int worldVBO, worldEBO;
    float imageHeight;
    float imageWidth;
    glm::vec3 worldCenter = glm::vec3(0, 0, 0);
    float worldRadius = 600;
    float worldRotationAngle = 0.0f;
    glm::mat4 worldModelMatrix;

    unsigned int moonTextureColor;
    unsigned int moonVAO;
    unsigned int moonVBO, moonEBO;
    float moonImageHeight;
    float moonImageWidth;
    glm::vec3 moonCenter = glm::vec3(0, 2600, 0);
    float moonRadius = 162;
    float moonRotationAngle = 0.0f;
    glm::mat4 moonModelMatrix;

    vector<float> worldVertices;
    unsigned int worldVertexSize;
    vector<unsigned int> worldIndices;
    unsigned int worldIndexSize;

    vector<float> moonVertices;
    unsigned int moonVertexSize;
    vector<unsigned int> moonIndices;
    unsigned int moonIndexSize;

    GLFWwindow *openWindow(const char *windowName, int width, int height);

    void Render(const char *coloredTexturePath, const char *greyTexturePath, const char *moonTexturePath);

    void handleKeyPress(GLFWwindow *window);

    void initColoredTexture(const char *filename, GLuint shader);

    void initGreyTexture(const char *filename, GLuint shader);

    void initMoonColoredTexture(const char *filename, GLuint shader);

    void setUnitSphereVertices(std::vector<float>& vertices, unsigned int& vertexSize, std::vector<unsigned int>& indices, unsigned int& indexSize);

    void initVertexBuffers(std::vector<float>& vertices, std::vector<unsigned int>& indices, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO);

    void updateCamera();

    void drawWorld(GLuint worldShaderID);

    void drawMoon(GLuint worldShaderID);
};

#endif
