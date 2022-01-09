#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "tinyxml2.h"

#define ABS(a) ((a) > 0 ? (a) : -1 * (a))
#define EPSILON 0.000000001

#define INT_OR_DOUBLE 1

using namespace tinyxml2;
using namespace std;


class Camera;
class Color;
class Matrix4;
class Mesh;
class Rotation;
class Scaling;
class Scene;
class Translation;
class Triangle;
class Vec3;
class Vec4;


/*
 * Calculate cross product of Vec3 a, Vec3 b and return resulting Vec3.
 */
Vec3 crossProductVec3(const Vec3& a, const Vec3& b);

/*
 * Calculate cross product of Vec4 a, Vec4 b and return resulting Vec4.
 */
Vec4 crossProductVec4(const Vec4& a, const Vec4& b);

/*
 * Calculate dot product of Vec3 a, Vec3 b and return resulting value.
 */
double dotProductVec3(const Vec3& a, const Vec3& b);

/*
 * Calculate dot product of Vec4 a, Vec4 b and return resulting value.
 */
double dotProductVec4(const Vec4& a, const Vec4& b);

/*
 * Find length (|v|) of Vec3 v.
 */
double magnitudeOfVec3(const Vec3& v);

/*
 * Find length (|v|) of Vec4 v.
 */
double magnitudeOfVec4(const Vec4& v);

/*
 * Normalize the Vec3 to make it unit Vec3.
 */
Vec3 normalizeVec3(const Vec3& v);

/*
 * Normalize the Vec4 to make it unit Vec4.
 */
Vec4 normalizeVec4(const Vec4& v);

/*
 * Return -v (inverse of Vec3 v)
 */
Vec3 inverseVec3(const Vec3& v);

/*
 * Return -v (inverse of Vec4 v)
 */
Vec4 inverseVec4(const Vec4& v);

/*
 * Add Vec4 a to Vec4 b and return resulting Vec4 (a+b).
 */
Vec4 addVec4(const Vec4& a, const Vec4& b);

/*
 * Subtract Vec4 b from Vec4 a and return resulting Vec4 (a-b).
 */
Vec4 subtractVec4(const Vec4& a, const Vec4& b);

/*
 * Multiply each element of Vec4 by scalar.
 */
Vec4 multiplyVec4ByScalar(const Vec4& v, double c);

/*
 * Prints elements in a Vec4. Can be used for debugging purposes.
 */
void printVec4(const Vec4& v);

/*
 * Check whether Vec4 a and Vec4 b are equal.
 * In case of equality, returns 1.
 * Otherwise, returns 0.
 */
int areEqualVec4(const Vec4& a, const Vec4& b);

/*
 * Returns an identity matrix (values on the diagonal are 1, others are 0).
 */
Matrix4 getIdentityMatrix();

/*
 * Multiply matrices m1 (Matrix4) and m2 (Matrix4) and return the result matrix r (Matrix4).
 */
Matrix4 multiplyMatrixByMatrix(const Matrix4& m1, const Matrix4& m2);

/*
 * Multiply matrix m (Matrix4) by vector v (vec4) and store the result in vector r (vec4).
 */
Vec4 multiplyMatrixByVec4(const Matrix4& m, const Vec4& v);


void perspectiveDivideVec4(Vec4& vector);


Matrix4 transposeMatrix(const Matrix4& matrix);


Matrix4 getOrthonormalMatrix(const Vec3& vector);


Matrix4 multiplyMatrixTransposeByMatrix(const Matrix4& matrix1, const Matrix4& matrix2);


Color addColor(const Color& color1, const Color& color2);


Color subtractColor(const Color& color1, const Color& color2);


Color multiplyColorByScalar(const Color& color, double scalar);


std::pair<int, int> minAndMaxOfThree(int a, int b, int c);



class Vec3
{
public:
    double x, y, z;
    int colorId;

    Vec3();
    Vec3(double x, double y, double z, int colorId);
    Vec3(const Vec3& other);

    double getElementAt(int index) const;
    
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v);
};



class Vec4
{
public:
    double x, y, z, t;
    int colorId;

    Vec4();
    Vec4(double x, double y, double z, double t, int colorId);
    Vec4(const Vec4& other);
    
    double getElementAt(int index) const;

    friend std::ostream& operator<<(std::ostream& os, const Vec4& v);
};



class Matrix4
{
public:
    double val[4][4];

    Matrix4();
    Matrix4(double val[4][4]);
    Matrix4(const Matrix4& other);
    Matrix4(const Translation& translation);
    Matrix4(const Rotation& rotation);
    Matrix4(const Scaling& scaling);

    friend ostream& operator<<(ostream& os, const Matrix4& m);
};



class Camera
{
public:
    int cameraId;
    int projectionType; // 1 for perspective, 0 for orthographic
    Vec3 pos;
    Vec3 gaze;
    Vec3 u;
    Vec3 v;
    Vec3 w;
    double left, right, bottom, top;
    double near;
    double far;
    int horRes;
    int verRes;
    string outputFileName;

    Camera();
    Camera(int cameraId,
           int projectionType,
           const Vec3& pos, const Vec3& gaze,
           const Vec3& u, const Vec3& v, const Vec3& w,
           double left, double right, double bottom, double top,
           double near, double far,
           int horRes, int verRes,
           string outputFileName);
    Camera(const Camera &other);

    Matrix4 viewportTransformationMatrix() const;
    Matrix4 cameraTransformationMatrix() const;

    friend std::ostream& operator<<(std::ostream& os, const Camera& c);
};



class Color
{
public:
    double r, g, b;

    Color();
    Color(double r, double g, double b);
    Color(const Color &other);

    friend std::ostream& operator<<(std::ostream& os, const Color& c);
};



class Mesh
{
public:
    int meshId;
    int type; // 0 for wireframe, 1 for solid
    int numberOfTransformations;
    vector<int> transformationIds;
    vector<char> transformationTypes;
    int numberOfTriangles;
    vector<Triangle> triangles;

    Mesh();
    Mesh(int meshId, int type, int numberOfTransformations,
         vector<int> transformationIds,
         vector<char> transformationTypes,
         int numberOfTriangles,
         vector<Triangle> triangles);

    Matrix4 modelingTransformationMatrix(Scene* scene) const;

    friend ostream& operator<<(ostream& os, const Mesh& m);
};



class Rotation
{
public:
    int rotationId;
    double angle, ux, uy, uz;

    Rotation();
    Rotation(int rotationId, double angle, double x, double y, double z);

    friend ostream& operator<<(ostream& os, const Rotation& r);
};



class Scaling
{
public:
    int scalingId;
    double sx, sy, sz;

    Scaling();
    Scaling(int scalingId, double sx, double sy, double sz);

    friend ostream& operator<<(ostream& os, const Scaling& s);
};



class Translation
{
public:
    int translationId;
    double tx, ty, tz;

    Translation();
    Translation(int translationId, double tx, double ty, double tz);

    friend ostream& operator<<(ostream& os, const Translation& t);
};



class Triangle
{
public:
    int vertexIds[3];

    Triangle();
    Triangle(int vid1, int vid2, int vid3);
    Triangle(const Triangle &other);

    int getFirstVertexId();
    int getSecondVertexId();
    int getThirdVertexId();

    void setFirstVertexId(int vid);
    void setSecondVertexId(int vid);
    void setThirdVertexId(int vid);
};



class Scene
{
public:
    Color backgroundColor;
    bool cullingEnabled;

    vector< vector<Color> > image;
    vector< Camera* > cameras;
    vector< Vec4* > vertices;
    vector< Color* > colorsOfVertices;
    vector< Scaling* > scalings;
    vector< Rotation* > rotations;
    vector< Translation* > translations;
    vector< Mesh* > meshes;

    Scene(const char *xmlPath);
    // TODO: create destructor and delete allocated memories

    void initializeImage(Camera* camera);
    void forwardRenderingPipeline(Camera* camera);
    void rasterizeLine(const Vec4* v0, const Vec4* v1, const Color* c0, const Color* c1);
    void drawLine(int x0, int y0, int x1, int y1, int dx, int dy, bool negateRow, bool swapXY, const Color* c0, const Color* c1);
    void rasterizeTriangle(const Camera& camera, const Vec4& v0, const Vec4& v1, const Vec4& v2, const Color& c0, const Color& c1, const Color& c2);
    bool isBackFaceCulled(const Vec4& v0, const Vec4& v1, const Vec4& v2);
    bool clipLine(const Camera& camera, std::pair<Vec4, Vec4>& line, std::pair<Color, Color>& colors);
    bool isLineVisible(double den, double num, double& te, double& tl);
    int makeBetweenZeroAnd255(double value);
    void writeImageToPPMFile(Camera* camera);
    void convertPPMToPNG(string ppmFileName, int osType);
};