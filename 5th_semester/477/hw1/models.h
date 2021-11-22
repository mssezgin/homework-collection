#ifndef __MODELS_H__
#define __MODELS_H__

#include <string>
#include <vector>
#include <cmath>
#include "parser.h"


typedef double real;


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


class Vector;


class Point : public Vec3real {
public:
    Point(real _x = 0.0, real _y = 0.0, real _z = 0.0);
    // Point(parser::Vec3f _vec3f);
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
    Vector normal() const;
    void normalize();
};


class Ray {
public:
    Vector origin, direction;

    Ray(const Vector &_origin, const Vector &_direction);
    Vector operator[](real t) const;
};


struct RGBColor {
    char r, g, b;
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
};


class Object {
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
};


class Scene {
public:
    RGBColor backgroundColor;
    real shadowRayEpsilon;
    int maxRecursionDepth;
    std::vector<Camera> cameras;
    Vec3real ambientLight;
    std::vector<PointLight> pointLights;
    std::vector<Material> materials;
    std::vector<Vec3real> vertexData;
    std::vector<Mesh> meshes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;

    Scene(char filePath[]);
};


#endif // __MODELS_H__
