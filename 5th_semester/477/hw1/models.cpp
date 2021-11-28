#include "models.h"


// class Scene static varibles

RGBColor Scene::backgroundColor;
real Scene::shadowRayEpsilon;
int Scene::maxRecursionDepth;
std::vector<Camera> Scene::cameras;
ColorVector Scene::ambientLight;
std::vector<PointLight> Scene::pointLights;
std::vector<Material> Scene::materials;
std::vector<Point> Scene::vertexData;
std::vector<Mesh> Scene::meshes;
std::vector<Triangle> Scene::triangles;
std::vector<Sphere> Scene::spheres;


// class Vec3i

Vec3i::Vec3i(int _x, int _y, int _z) :
    x(_x), y(_y), z(_z) { }

Vec3i::Vec3i(const parser::Vec3i &_vec3i) :
    x(_vec3i.x), y(_vec3i.y), z(_vec3i.z) { }


// class Vec3real

Vec3real::Vec3real(real _x, real _y, real _z) :
    x(_x), y(_y), z(_z) { }

Vec3real::Vec3real(const parser::Vec3f &_vec3f) :
    x(_vec3f.x), y(_vec3f.y), z(_vec3f.z) { }

Vec3real Vec3real::operator*(real scalar) const {
    return Vec3real(
        this->x * scalar,
        this->y * scalar,
        this->z * scalar
    );
}


// class Point

Point::Point(real _x, real _y, real _z) :
    Vec3real(_x, _y, _z) { }

Point::Point(const parser::Vec3f &_vec3f) :
    Vec3real(_vec3f) { }

Point Point::operator+(const Vector &vector) const {
    return Point(
        this->x + vector.x,
        this->y + vector.y,
        this->z + vector.z
    );
}

Vector Point::operator-(const Point &second) const {
    return Vector(
        this->x - second.x,
        this->y - second.y,
        this->z - second.z
    );
}


// class Vector

Vector::Vector(real _x, real _y, real _z) :
    Vec3real(_x, _y, _z) { }

Vector::Vector(const parser::Vec3f &_vec3f) :
    Vec3real(_vec3f) { }

Vector::Vector(const Point &end, const Point &start) {
    *this = end - start;
}

Vector Vector::operator+(const Vector &second) const {
    return Vector(
        this->x + second.x,
        this->y + second.y,
        this->z + second.z
    );
}

Vector Vector::operator-(const Vector &second) const {
    return Vector(
        this->x - second.x,
        this->y - second.y,
        this->z - second.z
    );
}

Vector Vector::operator*(real scalar) const {
    return Vector(
        this->x * scalar,
        this->y * scalar,
        this->z * scalar
    );
}

Vector Vector::operator/(real scalar) const {
    return Vector(
        this->x / scalar,
        this->y / scalar,
        this->z / scalar
    );
}

real Vector::dot(const Vector &second) const {
    return this->x * second.x + this->y * second.y + this->z * second.z;
}

real Vector::dotItself() const {
    return this->dot(*this);
}

Vector Vector::cross(const Vector &second) const {
    return Vector(
        this->y * second.z - this->z * second.y,
        this->z * second.x - this->x * second.z,
        this->x * second.y - this->y * second.x
    );
}

real Vector::length() const {
    return sqrt(this->dotItself());
}

Vector Vector::normalized() const {
    return *this / this->length();
}

void Vector::normalize() {
    *this = this->normalized();
}

real Vector::determinant(const Vector &col1, const Vector &col2, const Vector &col3) {
    return col1.x * (col2.y * col3.z - col2.z * col3.y) +
           col1.y * (col2.z * col3.x - col2.x * col3.z) +
           col1.z * (col2.x * col3.y - col2.y * col3.x);
}


// class Ray

Ray::Ray(const Point &_origin, const Vector &_direction,  int _recursionDepth) :
    origin(_origin), direction(_direction), recursionDepth(_recursionDepth) { }

Point Ray::operator[](real t) const {
    return origin + (direction * t);
}

real Ray::intersectWith(const Sphere &sphere) const {

    real A = this->direction.dot(this->direction);
    Vector vCO = this->origin - Scene::vertexData[sphere.centerVertexId];
    real halfB = this->direction.dot(vCO);
    real C = vCO.dot(vCO) - sphere.radius * sphere.radius;
    real delta = halfB * halfB - A * C;
    if (delta < 0) {
        return REAL_MAX;
    } else {
        return (-halfB - sqrt(delta)) / A;
    }
}

real Ray::intersectWith(const Face &face) const {

    Point A = Scene::vertexData[face.v0_id];
    Point B = Scene::vertexData[face.v1_id];
    Point C = Scene::vertexData[face.v2_id];
    Vector columns[4] = { A - B, A - C, this->direction, A - this->origin };
    real detA  = Vector::determinant(columns[0], columns[1], columns[2]);
    real beta  = Vector::determinant(columns[3], columns[1], columns[2]) / detA;
    real gamma = Vector::determinant(columns[0], columns[3], columns[2]) / detA;
    real t     = Vector::determinant(columns[0], columns[1], columns[3]) / detA;

    if (beta < 0) {
        return REAL_MAX;
    }
    if (gamma < 0) {
        return REAL_MAX;
    }
    if (beta + gamma > 1) {
        return REAL_MAX;
    }
    return t;
}

bool Ray::isInShadow(const Vector &objectNormal) const {

    const Ray &thisRay = *this;

    // intersect with itself to check whether the point is hiding behind the object
    if (thisRay.direction.dot(objectNormal) < 0) {
        return true;
    }

    // intersect with spheres
    for (long long i = 0, size = Scene::spheres.size(); i < size; i++) {
        real t = thisRay.intersectWith(Scene::spheres[i]);
        if (t < 1 && t > 0) {
            return true;
        }
    }

    // intersect with triangles
    for (long long i = 0, size = Scene::triangles.size(); i < size; i++) {
        real t = thisRay.intersectWith(Scene::triangles[i].face);
        if (t < 1 && t > 0) {
            return true;
        }
    }

    // intersect with meshes
    for (long long i = 0, size = Scene::meshes.size(); i < size; i++) {
        std::vector<Face> &faces = Scene::meshes[i].faces;

        for (long long j = 0, size = faces.size(); j < size; j++) {
            real t = thisRay.intersectWith(faces[j]);
            if (t < 1 && t > 0) {
                return true;
            }
        }
    }

    return false;
}

ColorVector Ray::computeColor(const Point &p, Material *material, const Vector &objectNormal) const {

    const Ray &thisRay = *this;
    ColorVector colorv(0.0, 0.0, 0.0);

    // calculate ambient shading
    colorv.increment(Scene::ambientLight, material->ambientReflectance);

    for (long long i = 0, size = Scene::pointLights.size(); i < size; i++) {

        PointLight &pointLight = Scene::pointLights[i];
        Vector toLight = (pointLight.position - p);
        Ray shadowRay(p, toLight);

        if (shadowRay.isInShadow(objectNormal)) {
            continue;
        }

        ColorVector irradiance = pointLight.intensity / toLight.dotItself(); // pow(toLight.length(), 2);

        // calculate diffuse shading
        real cosAngle = objectNormal.dot(toLight.normalized());
        if (cosAngle < 0) {
            cosAngle = 0;
        }
        colorv.increment(irradiance, material->diffuseReflectance * cosAngle);

        // calculate specular shading
        Vector halfVector = (toLight.normalized() - thisRay.direction).normalized();
        cosAngle = objectNormal.dot(halfVector);
        if (cosAngle < 0) {
            cosAngle = 0;
        } else {
            cosAngle = pow(cosAngle, material->phongExponent);
        }
        colorv.increment(irradiance, material->specularReflectance * cosAngle);
    }

    // calculate reflection
    if (material->isMirror && thisRay.recursionDepth < Scene::maxRecursionDepth) {
        Vector reflectionDirection = thisRay.direction - (objectNormal * (2 * thisRay.direction.dot(objectNormal)));
        Ray reflectionRay(p, reflectionDirection, thisRay.recursionDepth + 1);
        colorv.increment(reflectionRay.traceRay(), material->mirrorReflectance);
    }

    return colorv;
}

ColorVector Ray::traceRay() const {

    const Ray &thisRay = *this;
    real tClosest = REAL_MAX;
    Sphere *closestSphere = nullptr;
    Triangle *closestTriangle = nullptr;
    Mesh *closestMesh = nullptr;
    Face *closestMeshFace = nullptr;
    int closestObject = 0; // 1: Sphere, 2: Triangle, 3: Mesh and Face

    // intersect with spheres
    for (long long i = 0, size = Scene::spheres.size(); i < size; i++) {
        real t = thisRay.intersectWith(Scene::spheres[i]);
        if (t < tClosest && t >= 0) {
            tClosest = t;
            closestSphere = &(Scene::spheres[i]);
            closestObject = 1;
        }
    }

    // intersect with triangles
    for (long long i = 0, size = Scene::triangles.size(); i < size; i++) {
        real t = thisRay.intersectWith(Scene::triangles[i].face);
        if (t < tClosest && t >= 0) {
            tClosest = t;
            closestTriangle = &(Scene::triangles[i]);
            closestObject = 2;
        }
    }

    // intersect with meshes
    for (long long i = 0, size = Scene::meshes.size(); i < size; i++) {
        std::vector<Face> &faces = Scene::meshes[i].faces;

        for (long long j = 0, size = faces.size(); j < size; j++) {
            real t = thisRay.intersectWith(faces[j]);
            if (t < tClosest && t >= 0) {
                tClosest = t;
                closestMesh = &(Scene::meshes[i]);
                closestMeshFace = &(faces[j]);
                closestObject = 3;
            }
        }
    }

    // no intersection
    if (closestObject == 0) {
        if (thisRay.recursionDepth == 0) {
            // TODO: improve this
            return ColorVector(
                Scene::backgroundColor.r,
                Scene::backgroundColor.g,
                Scene::backgroundColor.b
            );
        } else {
            return ColorVector(0.0, 0.0, 0.0);
        }
    }

    // compute color
    Point p = thisRay[tClosest];
    Material *material;
    Vector closestObjectNormal;
    switch (closestObject) {
        case 1:
            material = &Scene::materials[closestSphere->materialId];
            closestObjectNormal = closestSphere->normal(p);
            break;
        case 2:
            material = &Scene::materials[closestTriangle->materialId];
            closestObjectNormal = closestTriangle->face.normal();
            break;
        case 3:
            material = &Scene::materials[closestMesh->materialId];
            closestObjectNormal = closestMeshFace->normal();
            break;
    }

    return thisRay.computeColor(p + (closestObjectNormal * Scene::shadowRayEpsilon), material, closestObjectNormal);
}


// class ColorVector

ColorVector::ColorVector(real _x, real _y, real _z) :
    Vec3real(_x, _y, _z) { }

ColorVector::ColorVector(const parser::Vec3f &_vec3f) :
    Vec3real(_vec3f) { }

void ColorVector::increment(ColorVector irradiance, Vec3real coefficient) {
    x += irradiance.x * coefficient.x;
    y += irradiance.y * coefficient.y;
    z += irradiance.z * coefficient.z;
}

ColorVector ColorVector::operator/(real scalar) const {
    return ColorVector(
        this->x / scalar,
        this->y / scalar,
        this->z / scalar
    );
}

RGBColor ColorVector::toRGBColor() const {
    unsigned char r = (unsigned char) (x + 0.5);
    unsigned char g = (unsigned char) (y + 0.5);
    unsigned char b = (unsigned char) (z + 0.5);
    if (((int) (x + 0.5)) > 255) {
        r = 255;
    }
    if (((int) (y + 0.5)) > 255) {
        g = 255;
    }
    if (((int) (z + 0.5)) > 255) {
        b = 255;
    }
    return RGBColor(r, g, b);
}


// class RGBColor

RGBColor::RGBColor(unsigned char _r, unsigned char _g, unsigned char _b) :
    r(_r), g(_g), b(_b) { }

RGBColor::RGBColor(const parser::Vec3i &_vec3i) :
    r(_vec3i.x), g(_vec3i.y), b(_vec3i.z) { }


// class ImagePlane

ImagePlane::ImagePlane() { }

ImagePlane::ImagePlane(const Camera &camera, const parser::Camera &_camera) :
    left(_camera.near_plane.x),
    right(_camera.near_plane.y),
    bottom(_camera.near_plane.z),
    top(_camera.near_plane.w),
    nearDistance(_camera.near_distance),
    imageWidth(_camera.image_width),
    imageHeight(_camera.image_height),
    imageName(_camera.image_name) {

    Point planeCenter = camera.position + (camera.gaze * this->nearDistance);
    Point positionTopLeft = planeCenter + (camera.u * this->left) + (camera.v * this->top);
    this->pixelWidth = (this->right - this->left) / this->imageWidth;
    this->pixelHeight = (this->top - this->bottom) / this->imageHeight;
    this->positionTopLeftPixel = positionTopLeft + (camera.u * (this->pixelWidth / 2)) + (camera.v * (-1 * this->pixelHeight / 2));
}


// class Camera

Camera::Camera(const parser::Camera &_camera) {
    this->position = Point(_camera.position);
    this->gaze = Vector(_camera.gaze).normalized();
    this->w = gaze * (-1.0);
    this->v = Vector(_camera.up).normalized();
    this->u = this->v.cross(this->w);
    this->nearPlane = ImagePlane(*this, _camera);
}

// TODO: float precision error: e.g. (400,400) 0.00125003 -0.00125003 -1
Ray Camera::createRay(int i, int j) const {
    Point pixel = nearPlane.positionTopLeftPixel + (u * (i * nearPlane.pixelWidth)) + (v * (-j * nearPlane.pixelHeight));
    return Ray(pixel, Vector(pixel, this->position).normalized(), 0);
}


// class PointLight

PointLight::PointLight(const parser::PointLight &_pointLight) :
    position(_pointLight.position),
    intensity(_pointLight.intensity) { }


// class Material

Material::Material(const parser::Material &_material) :
    isMirror(_material.is_mirror),
    ambientReflectance(_material.ambient),
    diffuseReflectance(_material.diffuse),
    specularReflectance(_material.specular),
    mirrorReflectance(_material.mirror),
    phongExponent(_material.phong_exponent) { }


// class Face

Face::Face(const parser::Face &_face) :
    v0_id(_face.v0_id - 1),
    v1_id(_face.v1_id - 1),
    v2_id(_face.v2_id - 1) { }

// TODO: precompute normal vectors
Vector Face::normal() const {
    Point A = Scene::vertexData[v0_id];
    Point B = Scene::vertexData[v1_id];
    Point C = Scene::vertexData[v2_id];
    return (B - A).cross(C - A).normalized();
}


// class Mesh

Mesh::Mesh(const parser::Mesh &_mesh) :
    materialId(_mesh.material_id - 1) {

    for (auto itr = _mesh.faces.begin(); itr != _mesh.faces.end(); ++itr) {
        this->faces.push_back(*itr);
    }
}


// class Triangle

Triangle::Triangle(const parser::Triangle &_triangle) :
    materialId(_triangle.material_id - 1),
    face(_triangle.indices) { }


// class Sphere

Sphere::Sphere(const parser::Sphere &_sphere) :
    materialId(_sphere.material_id - 1),
    centerVertexId(_sphere.center_vertex_id - 1),
    radius(_sphere.radius) { }

Vector Sphere::normal(const Point &point) const {
    return Vector(point, Scene::vertexData[centerVertexId]).normalized();
}


// class Scene

void Scene::loadFromXml(const std::string &filePath) {
    parser::Scene _scene;
    _scene.loadFromXml(filePath);

    Scene::backgroundColor = RGBColor(_scene.background_color);
    Scene::shadowRayEpsilon = _scene.shadow_ray_epsilon;
    Scene::maxRecursionDepth = _scene.max_recursion_depth;
    for (auto itr = _scene.cameras.begin(); itr != _scene.cameras.end(); ++itr) {
        Scene::cameras.push_back(Camera(*itr));
    }
    Scene::ambientLight = ColorVector(_scene.ambient_light);
    for (auto itr = _scene.point_lights.begin(); itr != _scene.point_lights.end(); ++itr) {
        Scene::pointLights.push_back(*itr);
    }
    for (auto itr = _scene.materials.begin(); itr != _scene.materials.end(); ++itr) {
        Scene::materials.push_back(*itr);
    }
    for (auto itr = _scene.vertex_data.begin(); itr != _scene.vertex_data.end(); ++itr) {
        Scene::vertexData.push_back(*itr);
    }
    for (auto itr = _scene.meshes.begin(); itr != _scene.meshes.end(); ++itr) {
        Scene::meshes.push_back(*itr);
    }
    for (auto itr = _scene.triangles.begin(); itr != _scene.triangles.end(); ++itr) {
        Scene::triangles.push_back(*itr);
    }
    for (auto itr = _scene.spheres.begin(); itr != _scene.spheres.end(); ++itr) {
        Scene::spheres.push_back(*itr);
    }
}


// global functions

void renderQuarter(unsigned char *image, size_t width, size_t heightStart, size_t heightEnd, const Camera &camera) {

    int index = width * heightStart * 3;

    for (int j = heightStart; j < heightEnd; ++j) {
        for (int i = 0; i < width; ++i) {

            Ray ray = camera.createRay(i, j);
            RGBColor color = ray.traceRay().toRGBColor();
            image[index++] = color.r;
            image[index++] = color.g;
            image[index++] = color.b;
        }
    }
}
