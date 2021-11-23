#ifndef __MODELS_H__
#define __MODELS_H__

#include <string>
#include <vector>
#include <cmath>
#include <cfloat>
#include <iostream>
#include "parser.h"


typedef double real;
class Vec3i;
class Vec3real;
class Point;
class Vector;
class Ray;
class RGBColor;
struct ImagePlane;
class Camera;
class PointLight;
class Material;
class Face;
class Object;
class Mesh;
class Triangle;
class Sphere;
class Scene;


class Vec3i {
public:
    int x, y, z;

    Vec3i(int _x = 0, int _y = 0, int _z = 0);
    Vec3i(const parser::Vec3i &_vec3i);
};


class Vec3real {
public:
    real x, y, z;

    Vec3real(real _x = 0.0, real _y = 0.0, real _z = 0.0);
    Vec3real(const parser::Vec3f &_vec3f);
};


class Point : public Vec3real {
public:
    Point(real _x = 0.0, real _y = 0.0, real _z = 0.0);
    Point(parser::Vec3f _vec3f);
    Point operator+(const Vector &vector) const;
    Vector operator-(const Point &second) const;
};


class Vector : public Vec3real {
public:
    Vector(real _x = 0.f, real _y = 0.f, real _z = 0.f);
    // Vector(parser::Vec3f _vec3f);
    Vector(Point end, Point start = Point(0.f, 0.f, 0.f));
    Vector operator+(const Vector &second) const;
    Vector operator-(const Vector &second) const;
    Vector operator*(real scalar) const;
    Vector operator/(real scalar) const;
    real dot(const Vector &second) const;
    Vector cross(const Vector &second) const;
    real length() const;
    Vector normalized() const;
    void normalize();
    static real determinant(const Vector &col1, const Vector &col2, const Vector &col3);
};


class Ray {
public:
    Point origin;
    Vector direction;

    Ray(const Point &_origin, const Vector &_direction);
    Point operator[](real t) const;
    real intersectWith(const Sphere &sphere) const;
    real intersectWith(const Face &face) const;
    RGBColor computeColor() const;
};


class RGBColor {
public:
    unsigned char r, g, b;

    RGBColor(unsigned char _r = 255U, unsigned char _g = 255U, unsigned char _b = 255U);
};


struct ImagePlane {
    real left, right, bottom, top;
    real pixelWidth, pixelHeight;
    Point positionTopLeftPixel;
};


class Camera {
public:
    Point position;
    Vector gaze;
    Vector u, v, w;
    ImagePlane nearPlane;
    real nearDistance;
    int imageWidth, imageHeight;
    std::string imageName;

    Camera(const parser::Camera &_camera);
    void initPositionTopLeftPixel();
    Ray createRay(int i, int j);
};


class PointLight {
public:
    Point position;
    Vec3real intensity;

    PointLight(const parser::PointLight &_pointLight);
};


class Material {
public:
    bool isMirror;
    Vec3real ambientReflectance;
    Vec3real diffuseReflectance;
    Vec3real specularReflectance;
    Vec3real mirrorReflectance;
    real phongExponent;

    Material(const parser::Material &_material);
};


class Face {
public:
    int v0_id;
    int v1_id;
    int v2_id;

    Face(const parser::Face &_face);
    Vector normal() const;
};


/* class Object {
public:
    int materialId;

    Object(int _materialId);
};


class Mesh : public Object {
public:
    std::vector<Face> faces;

    Mesh(const parser::Mesh &_mesh);
};


class Triangle : public Object {
public:
    Face indices;

    Triangle(const parser::Triangle &_triangle);
};


class Sphere : public Object {
public:
    int centerVertexId;
    real radius;

    Sphere(const parser::Sphere &_sphere);
}; */


class Mesh {
public:
    int materialId;
    std::vector<Face> faces;

    Mesh(const parser::Mesh &_mesh);
};


class Triangle {
public:
    int materialId;
    Face indices;

    Triangle(const parser::Triangle &_triangle);
};


class Sphere {
public:
    int materialId;
    int centerVertexId;
    real radius;

    Sphere(const parser::Sphere &_sphere);
    Vector normal(const Point &point) const;
};


class Scene {
public:
    static RGBColor backgroundColor;
    static real shadowRayEpsilon;
    static int maxRecursionDepth;
    static std::vector<Camera> cameras;
    static Vec3real ambientLight;
    static std::vector<PointLight> pointLights;
    static std::vector<Material> materials;
    static std::vector<Point> vertexData;
    static std::vector<Mesh> meshes;
    static std::vector<Triangle> triangles;
    static std::vector<Sphere> spheres;

    static void loadFromXml(const std::string &filePath);
};


// global functions


#endif // __MODELS_H__
