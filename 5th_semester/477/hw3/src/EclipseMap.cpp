#include "EclipseMap.h"

using namespace std;

struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture;

    vertex() {}

    vertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texture) : position(position),
                                                                                           normal(normal),
                                                                                           texture(texture) {}
};

struct triangle {
    int vertex1;
    int vertex2;
    int vertex3;

    triangle() {}

    triangle(const int &vertex1, const int &vertex2, const int &vertex3) : vertex1(vertex1), vertex2(vertex2),
                                                                           vertex3(vertex3) {}
};

void EclipseMap::Render(const char *coloredTexturePath, const char *greyTexturePath, const char *moonTexturePath) {
    // Open window
    GLFWwindow *window = openWindow(windowName, screenWidth, screenHeight);

    // Moon commands
    // Load shaders
    GLuint moonShaderID = initShaders("moonShader.vert", "moonShader.frag");

    initMoonColoredTexture(moonTexturePath, moonShaderID);

    // TODO: Set moonVertices
    setUnitSphereVertices(moonVertices, moonVertexSize, moonIndices, moonIndexSize);

    // TODO: Configure Buffers
    initVertexBuffers(moonVertices, moonIndices, moonVAO, moonVBO, moonEBO);


    // World commands
    // Load shaders
    GLuint worldShaderID = initShaders("worldShader.vert", "worldShader.frag");

    initColoredTexture(coloredTexturePath, worldShaderID);
    initGreyTexture(greyTexturePath, worldShaderID);

    // TODO: Set worldVertices
    setUnitSphereVertices(worldVertices, worldVertexSize, worldIndices, worldIndexSize);

    // TODO: Configure Buffers
    initVertexBuffers(worldVertices, worldIndices, worldVAO, worldVBO, worldEBO);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Main rendering loop
    do {
        glViewport(0, 0, screenWidth, screenHeight);

        glClearStencil(0);
        glClearDepth(1.0f);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        // TODO: Handle key presses
        handleKeyPress(window);

        // TODO: Manipulate rotation variables
        static float rotationAngle = 0.0;
        rotationAngle += 0.02;


        updateCamera();

        drawMoon(moonShaderID);

        drawWorld(worldShaderID);


        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (!glfwWindowShouldClose(window));

    // Delete buffers
    glDeleteBuffers(1, &moonVAO);
    glDeleteBuffers(1, &moonVBO);
    glDeleteBuffers(1, &moonEBO);


    // Delete buffers
    glDeleteBuffers(1, &worldVAO);
    glDeleteBuffers(1, &worldVBO);
    glDeleteBuffers(1, &worldEBO);

    glDeleteProgram(moonShaderID);
    glDeleteProgram(worldShaderID);

    // Close window
    glfwTerminate();
}

void EclipseMap::handleKeyPress(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // static unsigned int count = 0;
        // std::cout << "W pressed " << count++ << "\n";
        worldRotationAngle += 0.02;
        moonRotationAngle += 0.02;
    }
}

GLFWwindow *EclipseMap::openWindow(const char *windowName, int width, int height) {
    if (!glfwInit()) {
        getchar();
        return 0;
    }

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(width, height, windowName, NULL, NULL);
    glfwSetWindowMonitor(window, NULL, 1, 31, screenWidth, screenHeight, mode->refreshRate);

    if (window == NULL) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        getchar();
        glfwTerminate();
        return 0;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0, 0, 0, 0);

    return window;
}


void EclipseMap::initColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &worldTextureColor);
    cout << shader << endl;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, worldTextureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);


    imageWidth = width;
    imageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexColor"), 0);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initGreyTexture(const char *filename, GLuint shader) {

    glGenTextures(1, &worldTextureGrey);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, worldTextureGrey);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height;

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);




    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "TexGrey"), 1);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::initMoonColoredTexture(const char *filename, GLuint shader) {
    int width, height;
    glGenTextures(1, &moonTextureColor);
    cout << shader << endl;
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, moonTextureColor);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *raw_image = NULL;
    int bytes_per_pixel = 3;   /* or 1 for GRACYSCALE images */
    int color_space = JCS_RGB; /* or JCS_GRAYSCALE for grayscale images */

    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen(filename, "rb");
    unsigned long location = 0;
    int i = 0, j = 0;

    if (!infile) {
        printf("Error opening jpeg file %s\n!", filename);
        return;
    }
    printf("Texture filename = %s\n", filename);

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, infile);
    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, TRUE);
    /* Start decompression jpeg here */
    jpeg_start_decompress(&cinfo);

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char *) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    while (cinfo.output_scanline < cinfo.image_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
    }

    height = cinfo.image_height;
    width = cinfo.image_width;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_image);


    moonImageWidth = width;
    moonImageHeight = height;

    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shader); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:

    glUniform1i(glGetUniformLocation(shader, "MoonTexColor"), 2);
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(row_pointer[0]);
    free(raw_image);
    fclose(infile);

}

void EclipseMap::setUnitSphereVertices(std::vector<float>& vertices, unsigned int& vertexSize, std::vector<unsigned int>& indices, unsigned int& indexSize) {

    unsigned int vSize = (verticalSplitCount - 1) * horizontalSplitCount + 2;
    unsigned int tSize = (verticalSplitCount - 1) * horizontalSplitCount * 2;
    vertex* v = new vertex[vSize];
    triangle* t = new triangle[tSize];
    int index;


    // set vertices
    index = 0;

    // north pole
    v[index++] = vertex(glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec2(0.5, 0)); // or glm::vec2(0, 0)

    // latitudes and longitudes
    for (int verticalStep = 1; verticalStep < verticalSplitCount; verticalStep++) {
        float verticalRatio = (float) verticalStep / (float) verticalSplitCount;
        float beta = PI * verticalRatio;
        float sinBeta = sin(beta);
        float cosBeta = cos(beta);

        for (int horizontalStep = 0; horizontalStep < horizontalSplitCount; horizontalStep++) {
            float horizontalRatio = (float) horizontalStep / (float) horizontalSplitCount;
            float alpha = 2 * PI * horizontalRatio;
            glm::vec3 p(sinBeta * cos(alpha), sinBeta * sin(alpha), cosBeta);
            v[index++] = vertex(p, p, glm::vec2(horizontalRatio, verticalRatio));
        }
    }

    // south pole
    v[index++] = vertex(glm::vec3(0, 0, -1), glm::vec3(0, 0, -1), glm::vec2(0.5, 1)); // or glm::vec2(0, 1)


    // set indices
    index = 0;

    // north pole
    for (int i = 1; i < horizontalSplitCount; i++) {
        t[index++] = triangle(0, i, i + 1);
    }
    t[index++] = triangle(0, horizontalSplitCount, 1);

    // latitude and longitude triangle vertex indices
    for (int j = 1; j < verticalSplitCount - 1; j++) {
        int m = (j - 1) * horizontalSplitCount; // -1 + index of first vertex of above latitude
        int n = j * horizontalSplitCount; // -1 + index of first vertex of bottom latitude

        for (int i = 1; i < horizontalSplitCount; i++) { // i = 1 tolerates the offset (-1) of m and n
            t[index++] = triangle(m + i, n + i, n + i + 1);
            t[index++] = triangle(n + i + 1, m + i + 1, m + i);
        }
        t[index++] = triangle(m + horizontalSplitCount, n + horizontalSplitCount, n + 1);
        t[index++] = triangle(n + 1, m + 1, m + horizontalSplitCount);
    }

    // south pole
    int s = vSize - 1 - horizontalSplitCount;
    for (int i = 1; i < horizontalSplitCount; i++) {
        t[index++] = triangle(s + i, vSize - 1, s + i + 1);
    }
    t[index++] = triangle(s + horizontalSplitCount, vSize - 1, s + 1);


    // copy v into vertices
    for (int i = 0; i < vSize; i++) {
        vertices.push_back(v[i].position.x);
        vertices.push_back(v[i].position.y);
        vertices.push_back(v[i].position.z);
        vertices.push_back(v[i].normal.x);
        vertices.push_back(v[i].normal.y);
        vertices.push_back(v[i].normal.z);
        vertices.push_back(v[i].texture.x);
        vertices.push_back(v[i].texture.y);
    }

    // copy t into indices
    for (int i = 0; i < tSize; i++) {
        indices.push_back(t[i].vertex1);
        indices.push_back(t[i].vertex2);
        indices.push_back(t[i].vertex3);
    }

    std::cout << "vSize = " << vSize << ", index = " << index << std::endl;
    std::cout << "vertices.size() = " << vertices.size() << std::endl;
    std::cout << "tSize = " << tSize << ", index = " << index << std::endl;
    std::cout << "indices.size() = " << indices.size() << std::endl;
    vertexSize = vertices.size();
    indexSize = indices.size();
    delete[] v;
    delete[] t;
}

void EclipseMap::initVertexBuffers(std::vector<float>& vertices, std::vector<unsigned int>& indices, unsigned int& VAO, unsigned int& VBO, unsigned int& EBO) {

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) (6 * sizeof(float)));

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    vertices.clear();
    indices.clear();
}

void EclipseMap::updateCamera() {

    // TODO: Update camera at every frame
    projectionMatrix = glm::perspective((float) (projectionAngle / 180.0 * PI), aspectRatio, near, far);
    viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
}

void EclipseMap::drawWorld(GLuint worldShaderID) {

    // TODO: Manipulate rotation variables
    // worldRotationAngle += 0.02;

    // TODO: Use worldShaderID program
    glUseProgram(worldShaderID);

    // TODO: Bind textures
    // glClientActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, worldTextureColor);

    // TODO: Update uniform variables at every frame
    worldModelMatrix = glm::translate(glm::mat4(1.0f), worldCenter);
    worldModelMatrix = glm::rotate(worldModelMatrix, worldRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    worldModelMatrix = glm::scale(worldModelMatrix, glm::vec3(worldRadius, worldRadius, worldRadius));

    glUniformMatrix4fv(glGetUniformLocation(worldShaderID, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(worldShaderID, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(worldShaderID, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(worldModelMatrix));

    // TODO: Bind world vertex array
    glBindVertexArray(worldVAO);
    glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, worldEBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) (6 * sizeof(float)));

    // TODO: Draw world object
    glDrawElements(GL_TRIANGLES, worldIndexSize, GL_UNSIGNED_INT, 0);
}

void EclipseMap::drawMoon(GLuint moonShaderID) {

    // TODO: Manipulate rotation variables
    // moonRotationAngle += 0.02;

    // TODO: Use moonShaderID program
    glUseProgram(moonShaderID);

    // TODO: Bind textures
    // glClientActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_2D, moonTextureColor);

    // TODO: Update uniform variables at every frame
    moonModelMatrix = glm::translate(glm::mat4(1.0f), moonCenter);
    moonModelMatrix = glm::rotate(moonModelMatrix, moonRotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    moonModelMatrix = glm::scale(moonModelMatrix, glm::vec3(moonRadius, moonRadius, moonRadius));

    glUniformMatrix4fv(glGetUniformLocation(moonShaderID, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(moonShaderID, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(moonShaderID, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(moonModelMatrix));

    // TODO: Bind moon vertex array
    glBindVertexArray(moonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moonEBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) (3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*) (6 * sizeof(float)));

    // TODO: Draw moon object
    glDrawElements(GL_TRIANGLES, moonIndexSize, GL_UNSIGNED_INT, 0);
}
