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

Point::Point(parser::Vec3f _vec3f) :
    Vec3real(_vec3f) { }

Point Point::operator+(const Vector &vector) const {
    return Point(
        this->x + vector.x,
        this->y + vector.y,
        this->z + vector.z
    );
}

Vector Point::operator-(const Point &second) const {
    return Vector(this->x - second.x, this->y - second.y, this->z - second.z);
}


// class Vector

Vector::Vector(real _x, real _y, real _z) :
    Vec3real(_x, _y, _z) { }

// Vector::Vector(parser::Vec3f _vec3f) :
//     Vec3real(_vec3f) { }

Vector::Vector(Point end, Point start) {
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
    return sqrt(this->dot(*this));
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
    Vector vCO = this->origin - Scene::vertexData[sphere.centerVertexId - 1];
    real halfB = this->direction.dot(vCO);
    real C = vCO.dot(vCO) - sphere.radius * sphere.radius;
    real delta = halfB * halfB - A * C;
    if (delta < 0) {
        return DBL_MAX;
    } else {
        return (-halfB - sqrt(delta)) / A;
    }
}

real Ray::intersectWith(const Face &face) const {

    Point A = Scene::vertexData[face.v0_id - 1];
    Point B = Scene::vertexData[face.v1_id - 1];
    Point C = Scene::vertexData[face.v2_id - 1];
    Vector columns[4] = { A - B, A - C, this->direction, A - this->origin };
    real detA  = Vector::determinant(columns[0], columns[1], columns[2]);
    real beta  = Vector::determinant(columns[3], columns[1], columns[2]) / detA;
    real gamma = Vector::determinant(columns[0], columns[3], columns[2]) / detA;
    real t     = Vector::determinant(columns[0], columns[1], columns[3]) / detA;

    if (beta < 0) {
        return DBL_MAX;
    }
    if (gamma < 0) {
        return DBL_MAX;
    }
    if (beta + gamma > 1) {
        return DBL_MAX;
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

// TODO: give epsilon added p directly as argument
ColorVector Ray::computeColor(const Point &p, Material *material, const Vector &objectNormal) const {

    const Ray &thisRay = *this;
    ColorVector colorv(0.0, 0.0, 0.0);
    // calculate ambient shading
    colorv.increment(Scene::ambientLight, material->ambientReflectance);

    for (long long i = 0, size = Scene::pointLights.size(); i < size; i++) {

        PointLight &pointLight = Scene::pointLights[i];
        Vector toLight = (pointLight.position - p);
        Ray shadowRay(p + (objectNormal * Scene::shadowRayEpsilon), toLight);

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
        // TODO: optimize direction normalization
        Vector reflectionDirection = thisRay.direction.normalized() - (objectNormal * (2 * thisRay.direction.normalized().dot(objectNormal)));
        Ray reflectionRay(p + (objectNormal * Scene::shadowRayEpsilon), reflectionDirection, thisRay.recursionDepth + 1);
        colorv.increment(reflectionRay.traceRay(), material->mirrorReflectance);
    }

    return colorv;
}

ColorVector Ray::traceRay() const {

    const Ray &thisRay = *this;
    real tMin = (thisRay.recursionDepth == 0) ? 1 : 0;
    real tClosest = DBL_MAX;
    Sphere *closestSphere = nullptr;
    Triangle *closestTriangle = nullptr;
    Mesh *closestMesh = nullptr;
    Face *closestMeshFace = nullptr;
    int closestObject = 0; // 1: Sphere, 2: Triangle, 3: Mesh and Face

    // intersect with spheres
    for (long long i = 0, size = Scene::spheres.size(); i < size; i++) {
        real t = thisRay.intersectWith(Scene::spheres[i]);
        if (t < tClosest && t >= tMin) {
            tClosest = t;
            closestSphere = &(Scene::spheres[i]);
            // TODO: move this to the outside of the loop
            closestObject = 1;
        }
    }

    // intersect with triangles
    for (long long i = 0, size = Scene::triangles.size(); i < size; i++) {
        real t = thisRay.intersectWith(Scene::triangles[i].face);
        if (t < tClosest && t >= tMin) {
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
            if (t < tClosest && t >= tMin) {
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
            // return Scene::backgroundColor;
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
    // ColorVector colorv(0.0, 0.0, 0.0);
    Material *material;
    Vector closestObjectNormal;
    switch (closestObject) {
        case 1:
            material = &Scene::materials[closestSphere->materialId - 1];
            closestObjectNormal = closestSphere->normal(p);
            break;
        case 2:
            material = &Scene::materials[closestTriangle->materialId - 1];
            closestObjectNormal = closestTriangle->face.normal();
            break;
        case 3:
            material = &Scene::materials[closestMesh->materialId - 1];
            closestObjectNormal = closestMeshFace->normal();
            break;
    }

    return thisRay.computeColor(p, material, closestObjectNormal);
}


// class ColorVector

ColorVector::ColorVector(real _x, real _y, real _z) :
    Vec3real(_x, _y, _z) { }

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


// class Camera

Camera::Camera(const parser::Camera &_camera) {
    // this->position = Point(_camera.position);
    this->position = Point(
        _camera.position.x,
        _camera.position.y,
        _camera.position.z
    );
    // this->gaze = Vector(_camera.gaze);
    this->gaze = Vector(
        _camera.gaze.x,
        _camera.gaze.y,
        _camera.gaze.z
    );
    this->gaze.normalize();
    this->w = gaze * (-1.f);
    // this->v = Vector(_camera.up);
    this->v = Vector(
        _camera.up.x,
        _camera.up.y,
        _camera.up.z
    );
    this->v.normalize();
    this->u = this->v.cross(this->w);
    this->nearPlane.left   = _camera.near_plane.x;
    this->nearPlane.right  = _camera.near_plane.y;
    this->nearPlane.bottom = _camera.near_plane.z;
    this->nearPlane.top    = _camera.near_plane.w;
    this->nearDistance     = _camera.near_distance;
    this->imageWidth       = _camera.image_width;
    this->imageHeight      = _camera.image_height;
    this->imageName        = _camera.image_name;
    this->initPositionTopLeftPixel();
}

void Camera::initPositionTopLeftPixel() {
    Point imageCenter = position + (gaze * nearDistance);
    Point positionTopLeft = imageCenter + (u * nearPlane.left) + (v * nearPlane.top);
    nearPlane.pixelWidth = (nearPlane.right - nearPlane.left) / imageWidth;
    nearPlane.pixelHeight = (nearPlane.top - nearPlane.bottom) / imageHeight;
    nearPlane.positionTopLeftPixel = positionTopLeft + (u * (nearPlane.pixelWidth / 2)) + (v * (-1 * nearPlane.pixelHeight / 2));
}

// TODO: float precision error: e.g. (400,400) 0.00125003 -0.00125003 -1
Ray Camera::createRay(int i, int j) {
    return Ray(
        position,
        Vector(nearPlane.positionTopLeftPixel + (u * (i * nearPlane.pixelWidth)) + (v * (-j * nearPlane.pixelHeight)), position),
        0
    );
}


// class PointLight

PointLight::PointLight(const parser::PointLight &_pointLight) :
    position(Point(
        _pointLight.position.x,
        _pointLight.position.y,
        _pointLight.position.z
    )),
    intensity(ColorVector(
        _pointLight.intensity.x,
        _pointLight.intensity.y,
        _pointLight.intensity.z
    )) { }


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
    v0_id(_face.v0_id),
    v1_id(_face.v1_id),
    v2_id(_face.v2_id) { }

// TODO: precompute normal vectors
Vector Face::normal() const {
    Point A = Scene::vertexData[v0_id - 1];
    Point B = Scene::vertexData[v1_id - 1];
    Point C = Scene::vertexData[v2_id - 1];
    return (B - A).cross(C - A).normalized();
}


// class Mesh

Mesh::Mesh(const parser::Mesh &_mesh) :
    materialId(_mesh.material_id) {

    for (auto itr = _mesh.faces.begin(); itr != _mesh.faces.end(); ++itr) {
        this->faces.push_back(*itr);
    }
}


// class Triangle

Triangle::Triangle(const parser::Triangle &_triangle) :
    materialId(_triangle.material_id),
    face(_triangle.indices) { }


// class Sphere

Sphere::Sphere(const parser::Sphere &_sphere) :
    materialId(_sphere.material_id),
    centerVertexId(_sphere.center_vertex_id),
    radius(_sphere.radius) { }

Vector Sphere::normal(const Point &point) const {
    return Vector(point, Scene::vertexData[centerVertexId - 1]).normalized();
}


// class Scene

void Scene::loadFromXml(const std::string &filePath) {
    parser::Scene _scene;
    _scene.loadFromXml(filePath);

    Scene::backgroundColor.r = _scene.background_color.x;
    Scene::backgroundColor.g = _scene.background_color.y;
    Scene::backgroundColor.b = _scene.background_color.z;
    Scene::shadowRayEpsilon = _scene.shadow_ray_epsilon;
    Scene::maxRecursionDepth = _scene.max_recursion_depth;
    for (auto itr = _scene.cameras.begin(); itr != _scene.cameras.end(); ++itr) {
        Scene::cameras.push_back(Camera(*itr));
    }
    Scene::ambientLight = ColorVector(
        _scene.ambient_light.x,
        _scene.ambient_light.y,
        _scene.ambient_light.z
    );
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
