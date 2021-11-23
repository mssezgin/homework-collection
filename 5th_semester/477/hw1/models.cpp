#include "models.h"


// class Scene static varibles

RGBColor Scene::backgroundColor;
real Scene::shadowRayEpsilon;
int Scene::maxRecursionDepth;
std::vector<Camera> Scene::cameras;
Vec3real Scene::ambientLight;
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


// class Ray

Ray::Ray(const Point &_origin, const Vector &_direction) :
    origin(_origin), direction(_direction) { }

Point Ray::operator[](real t) const {
    return origin + (direction * t);
}

real Ray::intersectWith(const Sphere &sphere) const {

    real A = this->direction.dot(this->direction);
    Vector vCO = this->origin - Scene::vertexData[sphere.centerVertexId - 1];
    real halfB = this->direction.dot(vCO);
    real C = vCO.dot(vCO) - sphere.radius * sphere.radius;
    real delta = halfB * halfB - A * C;
    /* std::cout
        << " A " << A
        << " halfB " << halfB
        << " C " << C
        << " radius " << sphere.radius
        << " delta " << delta << "\t\t"; */
    if (delta < 0) {
        return DBL_MAX;
    } else {
        return (-halfB - sqrt(delta)) / A;
    }
}

RGBColor Ray::computeColor() const {

    real minT = DBL_MAX;
    int closestSphereIndex = -1;
    
    for (int i = 0, size = Scene::spheres.size(); i < size; i++) {

        real t = this->intersectWith(Scene::spheres[i]);
        // TODO: t >= 1.0 may be a problem for reflected rays (recursion)
        if (t >= 1.0 && t < minT) {
            std::cout << " currDist " << minT << " currIndex " << closestSphereIndex
                      << " newDist " << t << " newIndex " << i << "\n";
            minT = t;
            closestSphereIndex = i;
        }
    }

    if (closestSphereIndex != -1) {

        Sphere sphere = Scene::spheres[closestSphereIndex];
        Material &material = Scene::materials[sphere.materialId - 1];
        
        /* std::cout << "\t\treturn "
            << (255 * material.diffuseReflectance.x + 0.5) << " "
            << (255 * material.diffuseReflectance.y + 0.5) << " "
            << (255 * material.diffuseReflectance.z + 0.5) << "\n";
        
        return RGBColor(
            (unsigned char) (255 * material.diffuseReflectance.x + 0.5),
            (unsigned char) (255 * material.diffuseReflectance.y + 0.5),
            (unsigned char) (255 * material.diffuseReflectance.z + 0.5)
        ); */

        Point intersectionPoint = (*this)[minT];
        real diffuseScale = 0;

        for (auto itr = Scene::pointLights.begin(); itr != Scene::pointLights.end(); ++itr) {
            Vector L = ((*itr).position - intersectionPoint).normalized();
            real d = sphere.normal(intersectionPoint).dot(L);
            // Vector L = (*itr).position - intersectionPoint;
            // real d = sphere.normal(intersectionPoint).dot(L.normalized());
            if (d > 0) {
                diffuseScale += d;
            }
        }

        if (diffuseScale > 1.0) {
            diffuseScale = 1.0;
        }
        Vec3real scaledDiffuseReflectance;
        scaledDiffuseReflectance.x = material.diffuseReflectance.x * diffuseScale;
        scaledDiffuseReflectance.y = material.diffuseReflectance.y * diffuseScale;
        scaledDiffuseReflectance.z = material.diffuseReflectance.z * diffuseScale;
        
        return RGBColor(
            (unsigned char) (255 * scaledDiffuseReflectance.x + 0.5),
            (unsigned char) (255 * scaledDiffuseReflectance.y + 0.5),
            (unsigned char) (255 * scaledDiffuseReflectance.z + 0.5)
        );
    } else {
        // std::cout << "\t\treturn background\n";
        return Scene::backgroundColor;
    }
}


// class Camera

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
        Point(position),
        Vector(nearPlane.positionTopLeftPixel + (u * (i * nearPlane.pixelWidth)) + (v * (-j * nearPlane.pixelHeight)), position)
    );
}


// class PointLight

PointLight::PointLight(const parser::PointLight &_pointLight) :
    position(Point(
        _pointLight.position.x,
        _pointLight.position.y,
        _pointLight.position.z
    )),
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
    v0_id(_face.v0_id),
    v1_id(_face.v1_id),
    v2_id(_face.v2_id) { }


/* // class Object

Object::Object(int _materialId) :
    materialId(_materialId) { }


// class Mesh

Mesh::Mesh(const parser::Mesh &_mesh) :
    Object(_mesh.material_id) {
    
    for (auto itr = _mesh.faces.begin(); itr != _mesh.faces.end(); ++itr) {
        this->faces.push_back(*itr);
    }
}


// class Triangle

Triangle::Triangle(const parser::Triangle &_triangle) :
    Object(_triangle.material_id),
    indices(_triangle.indices) { }


// class Sphere

Sphere::Sphere(const parser::Sphere &_sphere) :
    Object(_sphere.material_id),
    centerVertexId(_sphere.center_vertex_id),
    radius(_sphere.radius) { } */


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
    indices(_triangle.indices) { }


// class Sphere

Sphere::Sphere(const parser::Sphere &_sphere) :
    materialId(_sphere.material_id),
    centerVertexId(_sphere.center_vertex_id),
    radius(_sphere.radius) { }

Vector Sphere::normal(const Point &point) const {
    // Vector n(point, Scene::vertexData[centerVertexId]);
    // return n.normalized();
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
    Scene::ambientLight = Vec3real(
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
