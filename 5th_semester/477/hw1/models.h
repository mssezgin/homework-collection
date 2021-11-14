#ifndef __MODELS_H__
#define __MODELS_H__

#include <string>
#include <vector>
#include <cmath>
#include "parser.h"


class Vec3i {
public:
    int x, y, z;

    Vec3i(int _x = 0, int _y = 0, int _z = 0);
    Vec3i(const parser::Vec3i &_vec3i);
};


class Vec3f {
public:
    float x, y, z;

    Vec3f(float _x = 0.f, float _y = 0.f, float _z = 0.f);
    Vec3f(const parser::Vec3f &_vec3f);
};


class Vector;


class Point : public Vec3f {
public:
    Point(float _x = 0.f, float _y = 0.f, float _z = 0.f);
    // Point(parser::Vec3f _vec3f);
    Vector operator-(const Point &second) const;
};


class Vector : public Vec3f {
public:
    Vector(float _x = 0.f, float _y = 0.f, float _z = 0.f);
    // Vector(parser::Vec3f _vec3f);
    Vector(Point end, Point start = Point(0.f, 0.f, 0.f));
    Vector operator+(const Vector &second) const;
    Vector operator-(const Vector &second) const;
    Vector operator*(float scalar) const;
    Vector operator/(float scalar) const;
    float dot(const Vector &second) const;
    Vector cross(const Vector &second) const;
    float length() const;
    Vector normal() const;
    void normalize();
};


class Ray {
public:
    Vector origin, direction;
};


struct RGBColor {
    char r, g, b;
};


struct ImagePlane {
    float left, right, bottom, top;
};


class Camera {
public:
    Point position;
    Vector gaze;
    Vector u, v, w;
    ImagePlane nearPlane;
    float nearDistance;
    int imageWidth, imageHeight;
    std::string imageName;

    Camera(const parser::Camera &_camera);
};


class PointLight {
public:
    Point position;
    Vec3f intensity;

    PointLight(const parser::PointLight &_pointLight);
};


class Material {
public:
    bool isMirror;
    Vec3f ambientReflectance;
    Vec3f diffuseReflectance;
    Vec3f specularReflectance;
    Vec3f mirrorReflectance;
    float phongExponent;

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
    float radius;

    Sphere(const parser::Sphere &_sphere);
};


class Scene {
public:
    RGBColor backgroundColor;
    float shadowRayEpsilon;
    int maxRecursionDepth;
    std::vector<Camera> cameras;
    Vec3f ambientLight;
    std::vector<PointLight> pointLights;
    std::vector<Material> materials;
    std::vector<Vec3f> vertexData;
    std::vector<Mesh> meshes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;

    Scene(char filePath[]);
};


#endif // __MODELS_H__
