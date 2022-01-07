#include "Bundle.h"

////////////////////////////////
// Global Functions
////////////////////////////////

Vec3 crossProductVec3(const Vec3& a, const Vec3& b)
{
    Vec3 result;
    result.x = a.y * b.z - b.y * a.z;
    result.y = b.x * a.z - a.x * b.z;
    result.z = a.x * b.y - b.x * a.y;
    return result;
}

Vec4 crossProductVec4(const Vec4& a, const Vec4& b)
{
    Vec4 result;
    result.x = a.y * b.z - b.y * a.z;
    result.y = b.x * a.z - a.x * b.z;
    result.z = a.x * b.y - b.x * a.y;
    result.t = a.t;
    return result;
}

double dotProductVec3(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double dotProductVec4(const Vec4& a, const Vec4& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double magnitudeOfVec3(const Vec3& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

double magnitudeOfVec4(const Vec4& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 normalizeVec3(const Vec3& v)
{
    Vec3 result;
    double inv_d = 1 / magnitudeOfVec3(v);
    result.x = v.x * inv_d;
    result.y = v.y * inv_d;
    result.z = v.z * inv_d;
    // TODO: result.colorId = v.colorId;
    return result;
}

Vec4 normalizeVec4(const Vec4& v)
{
    Vec4 result;
    double inv_d = 1 / magnitudeOfVec4(v);
    result.x = v.x * inv_d;
    result.y = v.y * inv_d;
    result.z = v.z * inv_d;
    result.t = v.t;
    // TODO: result.colorId = v.colorId;
    return result;
}

Vec3 inverseVec3(const Vec3& v)
{
    Vec3 result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    // TODO: result.colorId = v.colorId;
    return result;
}

Vec4 inverseVec4(const Vec4& v)
{
    Vec4 result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    result.t = v.t;
    // TODO: result.colorId = v.colorId;
    return result;
}

Vec4 addVec4(const Vec4& a, const Vec4& b)
{
    Vec4 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.t = a.t;
    return result;
}

Vec4 subtractVec4(const Vec4& a, const Vec4& b)
{
    Vec4 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.t = a.t;
    return result;
}

Vec4 multiplyVec4ByScalar(const Vec4& v, double c)
{
    Vec4 result;
    result.x = v.x * c;
    result.y = v.y * c;
    result.z = v.z * c;
    result.t = v.t;
    // TODO: result.colorId = v.colorId;
    return result;
}

void printVec4(const Vec4& v)
{
    cout << "(" << v.x << "," << v.y << "," << v.z << ", " << v.t << ")" << endl;
}

int areEqualVec4(const Vec4& a, const Vec4& b)
{
    // if x difference, y difference and z difference is smaller than threshold, then they are equal
    if ((ABS((a.x - b.x)) < EPSILON) && (ABS((a.y - b.y)) < EPSILON) && (ABS((a.z - b.z)) < EPSILON))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

Matrix4 getIdentityMatrix()
{
    Matrix4 result;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                result.val[i][j] = 1.0;
            }
            else
            {
                result.val[i][j] = 0.0;
            }
        }
    }

    return result;
}

Matrix4 multiplyMatrixByMatrix(const Matrix4& m1, const Matrix4& m2)
{
    Matrix4 result;
    double total;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            total = 0;
            for (int k = 0; k < 4; k++)
            {
                total += m1.val[i][k] * m2.val[k][j];
            }

            result.val[i][j] = total;
        }
    }

    return result;
}

Vec4 multiplyMatrixByVec4(const Matrix4& m, const Vec4& v)
{
    double values[4];
    double total;

    for (int i = 0; i < 4; i++)
    {
        total = 0;
        for (int j = 0; j < 4; j++)
        {
            total += m.val[i][j] * v.getElementAt(j);
        }
        values[i] = total;
    }

    return Vec4(values[0], values[1], values[2], values[3], v.colorId);
}

Matrix4 transposeMatrix(const Matrix4& matrix)
{
    Matrix4 M;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            M.val[i][j] = matrix.val[j][i];

    return M;
}

Matrix4 getOrthonormalMatrix(const Vec3& vector)
{
    Vec3 u = normalizeVec3(vector);
    Vec3 v = normalizeVec3(Vec3(-vector.y, vector.x, 0.0, -1));
    Vec3 w = crossProductVec3(u, v);
    Matrix4 M;
    M.val[0][0] = u.x; M.val[0][1] = u.y; M.val[0][2] = u.z;
    M.val[1][0] = v.x; M.val[1][1] = v.y; M.val[1][2] = v.z;
    M.val[2][0] = w.x; M.val[2][1] = w.y; M.val[2][2] = w.z;
    M.val[3][3] = 1;
    return M;
}

Matrix4 multiplyMatrixTransposeByMatrix(const Matrix4& matrix1, const Matrix4& matrix2)
{
    Matrix4 result;
    double total;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            total = 0;
            for (int k = 0; k < 4; k++)
                total += matrix1.val[k][i] * matrix2.val[k][j];
            result.val[i][j] = total;
        }
    }
    return result;
}

////////////////////////////////
// Vec3
////////////////////////////////

Vec3::Vec3()
{
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
    this->colorId = -1;
}

Vec3::Vec3(double x, double y, double z, int colorId)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->colorId = colorId;
}

Vec3::Vec3(const Vec3& other)
{
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->colorId = other.colorId;
}

double Vec3::getElementAt(int index) const
{
    switch (index)
    {
        case 0:
            return this->x;

        case 1:
            return this->y;

        case 2:
            return this->z;

        default:
            return this->z;
    }
}

ostream& operator<<(ostream& os, const Vec3& v)
{
    os << fixed << setprecision(6) << "[" << v.x << ", " << v.y << ", " << v.z << "]";

    return os;
}

////////////////////////////////
// Vec4
////////////////////////////////

Vec4::Vec4()
{
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
    this->t = 0.0;
    this->colorId = -1;
}

Vec4::Vec4(double x, double y, double z, double t, int colorId)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->t = t;
    this->colorId = colorId;
}

Vec4::Vec4(const Vec4& other)
{
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->t = other.t;
    this->colorId = other.colorId;
}

double Vec4::getElementAt(int index) const
{
    switch (index)
    {
        case 0:
            return this->x;

        case 1:
            return this->y;

        case 2:
            return this->z;

        case 3:
            return this->t;

        default:
            return this->t;
    }
}

ostream& operator<<(ostream& os, const Vec4& v)
{
    os << fixed << setprecision(6) << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.t << "]";

    return os;
}

////////////////////////////////
// Matrix4
////////////////////////////////

Matrix4::Matrix4()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->val[i][j] = 0;
        }
    }
}

Matrix4::Matrix4(double val[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->val[i][j] = val[i][j];
        }
    }
}

Matrix4::Matrix4(const Matrix4& other)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            this->val[i][j] = other.val[i][j];
        }
    }
}

Matrix4::Matrix4(const Translation& translation)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            this->val[i][j] = 0;

    this->val[0][0] = 1;
    this->val[0][3] = translation.tx;
    this->val[1][1] = 1;
    this->val[1][3] = translation.ty;
    this->val[2][2] = 1;
    this->val[2][3] = translation.tz;
    this->val[3][3] = 1;
}

Matrix4::Matrix4(const Rotation& rotation)
{
    Matrix4 M = getOrthonormalMatrix(Vec3(rotation.ux, rotation.uy, rotation.uz, -1));
    Matrix4 R;
    double angle = rotation.angle / 180 * M_PI;
    double c = cos(angle);
    double s = sin(angle);
    R.val[0][0] = 1;
    R.val[1][1] = c;
    R.val[1][2] = -s;
    R.val[2][1] = s;
    R.val[2][2] = c;
    R.val[3][3] = 1;

    *this = multiplyMatrixTransposeByMatrix(M, multiplyMatrixByMatrix(R, M));
}

Matrix4::Matrix4(const Scaling& scaling)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            this->val[i][j] = 0;

    this->val[0][0] = scaling.sx;
    this->val[1][1] = scaling.sy;
    this->val[2][2] = scaling.sz;
    this->val[3][3] = 1;
}

ostream& operator<<(ostream& os, const Matrix4& m)
{
    os << fixed << setprecision(6) << "|" << m.val[0][0] << "|" << m.val[0][1] << "|" << m.val[0][2] << "|" << m.val[0][3] << "|"
       << endl
       << "|" << m.val[1][0] << "|" << m.val[1][1] << "|" << m.val[1][2] << "|" << m.val[1][3] << "|"
       << endl
       << "|" << m.val[2][0] << "|" << m.val[2][1] << "|" << m.val[2][2] << "|" << m.val[2][3] << "|"
       << endl
       << "|" << m.val[3][0] << "|" << m.val[3][1] << "|" << m.val[3][2] << "|" << m.val[3][3] << "|";

    return os;
}

////////////////////////////////
// Camera
////////////////////////////////

Camera::Camera() {}

Camera::Camera(int cameraId,
               int projectionType,
               const Vec3& pos, const Vec3& gaze,
               const Vec3& u, const Vec3& v, const Vec3& w,
               double left, double right, double bottom, double top,
               double near, double far,
               int horRes, int verRes,
               string outputFileName)
{
    this->cameraId = cameraId;
    this->projectionType = projectionType;
    this->pos = pos;
    this->gaze = gaze;
    this->u = u;
    this->v = v;
    this->w = w;
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
    this->near = near;
    this->far = far;
    this->horRes = horRes;
    this->verRes = verRes;
    this->outputFileName = outputFileName;
}

Camera::Camera(const Camera &other)
{
    this->cameraId = other.cameraId;
    this->projectionType = other.projectionType;
    this->pos = other.pos;
    this->gaze = other.gaze;
    this->u = other.u;
    this->v = other.v;
    this->w = other.w;
    this->left = other.left;
    this->right = other.right;
    this->bottom = other.bottom;
    this->top = other.top;
    this->near = other.near;
    this->far = other.far;
    this->horRes = other.horRes;
    this->verRes = other.verRes;
    this->outputFileName = other.outputFileName;
}

Matrix4 Camera::viewportTransformationMatrix() const
{
    Matrix4 M;
    double nx_2 = this->horRes * 0.5;
    double ny_2 = this->verRes * 0.5;
    M.val[0][0] = nx_2;
    M.val[0][3] = nx_2 - 0.5;
    M.val[1][1] = ny_2;
    M.val[1][3] = ny_2 - 0.5;
    M.val[2][2] = 0.5;
    M.val[2][3] = 0.5;
    M.val[3][3] = 1.0;
    return M;
}

Matrix4 Camera::cameraTransformationMatrix() const
{
    Matrix4 M;
    double inv_dx = 1.0 / (right - left);
    double inv_dy = 1.0 / (top - bottom);
    double inv_dz = 1.0 / (far - near);
    if (projectionType == 0) // orthographic
    {
        M.val[0][0] = 2.0 * inv_dx;
        M.val[0][3] = -(right + left) * inv_dx;
        M.val[1][1] = 2.0 * inv_dy;
        M.val[1][3] = -(top + bottom) * inv_dy;
        M.val[2][2] = -2.0 * inv_dz;
        M.val[2][3] = -(far + near) * inv_dz;
        M.val[3][3] = 1.0;
    }
    else // perspective
    {
        M.val[0][0] = 2.0 * near * inv_dx;
        M.val[0][2] = (right + left) * inv_dx;
        M.val[1][1] = 2.0 * near * inv_dy;
        M.val[1][2] = (top + bottom) * inv_dy;
        M.val[2][2] = -(far + near) * inv_dz;
        M.val[2][3] = -2.0 * far * near * inv_dz;
        M.val[3][2] = -1.0;
    }
    return M;
}

ostream& operator<<(ostream& os, const Camera& c)
{
    const char *camType = c.projectionType ? "perspective" : "orthographic";

    os << fixed << setprecision(6) << "Camera " << c.cameraId << " (" << camType << ") => pos: " << c.pos << " gaze: " << c.gaze << endl
       << "\tu: " << c.u << " v: " << c.v << " w: " << c.w << endl
       << fixed << setprecision(3) << "\tleft: " << c.left << " right: " << c.right << " bottom: " << c.bottom << " top: " << c.top << endl
       << "\tnear: " << c.near << " far: " << c.far << " resolutions: " << c.horRes << "x" << c.verRes << " fileName: " << c.outputFileName;

    return os;
}

////////////////////////////////
// Color
////////////////////////////////

Color::Color() {}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(const Color &other)
{
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

ostream& operator<<(ostream& os, const Color& c)
{
    os << fixed << setprecision(0) << "rgb(" << c.r << ", " << c.g << ", " << c.b << ")";
    return os;
}

////////////////////////////////
// Mesh
////////////////////////////////

Mesh::Mesh() {}

Mesh::Mesh(int meshId, int type, int numberOfTransformations,
             vector<int> transformationIds,
             vector<char> transformationTypes,
             int numberOfTriangles,
             vector<Triangle> triangles)
{
    this->meshId = meshId;
    this->type = type;
    this->numberOfTransformations = numberOfTransformations;
    this->numberOfTriangles = numberOfTriangles;

    this->transformationIds = transformationIds;
    this->transformationTypes = transformationTypes;
    this->triangles = triangles;
}

Matrix4 Mesh::modelingTransformationMatrix(Scene* scene) const
{
    Matrix4 M = getIdentityMatrix();
    for (int i = 0; i < numberOfTransformations; i++)
    {
        switch (transformationTypes[i])
        {
            case 't':
                M = multiplyMatrixByMatrix(*scene->translations[transformationIds[i] - 1], M);
                break;

            case 'r':
                M = multiplyMatrixByMatrix(*scene->rotations[transformationIds[i] - 1], M);
                break;

            case 's':
                M = multiplyMatrixByMatrix(*scene->scalings[transformationIds[i] - 1], M);
                break;

            default:
                break;
        }
    }
    return M;
}

ostream& operator<<(ostream& os, const Mesh& m)
{
    os << "Mesh " << m.meshId;

    if (m.type == 0)
    {
        os << " wireframe(0) with ";
    }
    else
    {
        os << " solid(1) with ";
    }

    os << fixed << setprecision(3) << m.numberOfTransformations << " transformations and " << m.numberOfTriangles << " triangles"
       << endl << "\tTriangles are:" << endl << fixed << setprecision(0);

    for (int i = 0; i < m.triangles.size(); i++)
    {
        os << "\t\t" << m.triangles[i].vertexIds[0] << " " << m.triangles[i].vertexIds[1] << " " << m.triangles[i].vertexIds[2] << endl;
    }

    return os;
}

////////////////////////////////
// Rotation
////////////////////////////////

Rotation::Rotation() {}

Rotation::Rotation(int rotationId, double angle, double x, double y, double z)
{
    this->rotationId = rotationId;
    this->angle = angle;
    this->ux = x;
    this->uy = y;
    this->uz = z;
}

ostream& operator<<(ostream& os, const Rotation& r)
{
    os << fixed << setprecision(3) << "Rotation " << r.rotationId << " => [angle=" << r.angle << ", " << r.ux << ", " << r.uy << ", " << r.uz << "]";

    return os;
}

////////////////////////////////
// Scaling
////////////////////////////////

Scaling::Scaling() {}

Scaling::Scaling(int scalingId, double sx, double sy, double sz)
{
    this->scalingId = scalingId;
    this->sx = sx;
    this->sy = sy;
    this->sz = sz;
}

ostream& operator<<(ostream& os, const Scaling& s)
{
    os << fixed << setprecision(3) << "Scaling " << s.scalingId << " => [" << s.sx << ", " << s.sy << ", " << s.sz << "]";

    return os;
}

////////////////////////////////
// Translation
////////////////////////////////

Translation::Translation()
{
    this->translationId = -1;
    this->tx = 0.0;
    this->ty = 0.0;
    this->tz = 0.0;
}

Translation::Translation(int translationId, double tx, double ty, double tz)
{
    this->translationId = translationId;
    this->tx = tx;
    this->ty = ty;
    this->tz = tz;
}

ostream& operator<<(ostream& os, const Translation& t)
{
    os << fixed << setprecision(3) << "Translation " << t.translationId << " => [" << t.tx << ", " << t.ty << ", " << t.tz << "]";

    return os;
}

////////////////////////////////
// Triangle
////////////////////////////////

Triangle::Triangle()
{
    this->vertexIds[0] = -1;
    this->vertexIds[1] = -1;
    this->vertexIds[2] = -1;
}

Triangle::Triangle(int vid1, int vid2, int vid3)
{
    this->vertexIds[0] = vid1;
    this->vertexIds[1] = vid2;
    this->vertexIds[2] = vid3;
}

Triangle::Triangle(const Triangle &other)
{
    this->vertexIds[0] = other.vertexIds[0];
    this->vertexIds[1] = other.vertexIds[1];
    this->vertexIds[2] = other.vertexIds[2];
}

// getters
int Triangle::getFirstVertexId()
{
    return this->vertexIds[0];
}

int Triangle::getSecondVertexId()
{
    return this->vertexIds[1];
}

int Triangle::getThirdVertexId()
{
    return this->vertexIds[2];
}

// setters
void Triangle::setFirstVertexId(int vid)
{
    this->vertexIds[0] = vid;
}

void Triangle::setSecondVertexId(int vid)
{
    this->vertexIds[1] = vid;
}

void Triangle::setThirdVertexId(int vid)
{
    this->vertexIds[2] = vid;
}

////////////////////////////////
// Scene
////////////////////////////////

/*
    Transformations, clipping, culling, rasterization are done here.
    You may define helper functions.
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
    Matrix4 M_vp = camera->viewportTransformationMatrix();
    Matrix4 M_cam = camera->cameraTransformationMatrix();

    for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
    {
        Mesh& mesh = **itr;
        Matrix4 M_model = mesh.modelingTransformationMatrix(this);
        Matrix4 M = multiplyMatrixByMatrix(M_cam, M_model);

        for (auto itr = mesh.triangles.begin(); itr != mesh.triangles.end(); ++itr)
        {
            Triangle& triangle = *itr;

            // projection, camera and modeling transformation
            Vec4 v0 = multiplyMatrixByVec4(M, *this->vertices[triangle.vertexIds[0] - 1]);
            Vec4 v1 = multiplyMatrixByVec4(M, *this->vertices[triangle.vertexIds[1] - 1]);
            Vec4 v2 = multiplyMatrixByVec4(M, *this->vertices[triangle.vertexIds[2] - 1]);

            // TODO: normal transformation

            // TODO: back-face culling

            // TODO: clipping

            // TODO: perspective divide

            // TODO: viewport transformation

            // TODO: rasterization

            // TODO: fragment processing
        }
    }
}

/*
    Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
    const char *str;
    XMLDocument xmlDoc;
    XMLElement *pElement;

    xmlDoc.LoadFile(xmlPath);

    XMLNode *pRoot = xmlDoc.FirstChild();

    // read background color
    pElement = pRoot->FirstChildElement("BackgroundColor");
    str = pElement->GetText();
    sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

    // read culling
    pElement = pRoot->FirstChildElement("Culling");
    if (pElement != NULL) {
        str = pElement->GetText();
        
        if (strcmp(str, "enabled") == 0) {
            cullingEnabled = true;
        }
        else {
            cullingEnabled = false;
        }
    }

    // read cameras
    pElement = pRoot->FirstChildElement("Cameras");
    XMLElement *pCamera = pElement->FirstChildElement("Camera");
    XMLElement *camElement;
    while (pCamera != NULL)
    {
        Camera *cam = new Camera();

        pCamera->QueryIntAttribute("id", &cam->cameraId);

        // read projection type
        str = pCamera->Attribute("type");

        if (strcmp(str, "orthographic") == 0) {
            cam->projectionType = 0;
        }
        else {
            cam->projectionType = 1;
        }

        camElement = pCamera->FirstChildElement("Position");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf", &cam->pos.x, &cam->pos.y, &cam->pos.z);

        camElement = pCamera->FirstChildElement("Gaze");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf", &cam->gaze.x, &cam->gaze.y, &cam->gaze.z);

        camElement = pCamera->FirstChildElement("Up");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf", &cam->v.x, &cam->v.y, &cam->v.z);

        cam->gaze = normalizeVec3(cam->gaze);
        cam->u = crossProductVec3(cam->gaze, cam->v);
        cam->u = normalizeVec3(cam->u);

        cam->w = inverseVec3(cam->gaze);
        cam->v = crossProductVec3(cam->u, cam->gaze);
        cam->v = normalizeVec3(cam->v);

        camElement = pCamera->FirstChildElement("ImagePlane");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
               &cam->left, &cam->right, &cam->bottom, &cam->top,
               &cam->near, &cam->far, &cam->horRes, &cam->verRes);

        camElement = pCamera->FirstChildElement("OutputName");
        str = camElement->GetText();
        cam->outputFileName = string(str);

        cameras.push_back(cam);

        pCamera = pCamera->NextSiblingElement("Camera");
    }

    // read vertices
    pElement = pRoot->FirstChildElement("Vertices");
    XMLElement *pVertex = pElement->FirstChildElement("Vertex");
    int vertexId = 1;

    while (pVertex != NULL)
    {
        Vec4 *vertex = new Vec4();
        Color *color = new Color();

        vertex->colorId = vertexId;

        str = pVertex->Attribute("position");
        sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);
        vertex->t = 1;

        str = pVertex->Attribute("color");
        sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

        vertices.push_back(vertex);
        colorsOfVertices.push_back(color);

        pVertex = pVertex->NextSiblingElement("Vertex");

        vertexId++;
    }

    // read translations
    pElement = pRoot->FirstChildElement("Translations");
    XMLElement *pTranslation = pElement->FirstChildElement("Translation");
    while (pTranslation != NULL)
    {
        Translation *translation = new Translation();

        pTranslation->QueryIntAttribute("id", &translation->translationId);

        str = pTranslation->Attribute("value");
        sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

        translations.push_back(translation);

        pTranslation = pTranslation->NextSiblingElement("Translation");
    }

    // read scalings
    pElement = pRoot->FirstChildElement("Scalings");
    XMLElement *pScaling = pElement->FirstChildElement("Scaling");
    while (pScaling != NULL)
    {
        Scaling *scaling = new Scaling();

        pScaling->QueryIntAttribute("id", &scaling->scalingId);
        str = pScaling->Attribute("value");
        sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

        scalings.push_back(scaling);

        pScaling = pScaling->NextSiblingElement("Scaling");
    }

    // read rotations
    pElement = pRoot->FirstChildElement("Rotations");
    XMLElement *pRotation = pElement->FirstChildElement("Rotation");
    while (pRotation != NULL)
    {
        Rotation *rotation = new Rotation();

        pRotation->QueryIntAttribute("id", &rotation->rotationId);
        str = pRotation->Attribute("value");
        sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

        rotations.push_back(rotation);

        pRotation = pRotation->NextSiblingElement("Rotation");
    }

    // read meshes
    pElement = pRoot->FirstChildElement("Meshes");

    XMLElement *pMesh = pElement->FirstChildElement("Mesh");
    XMLElement *meshElement;
    while (pMesh != NULL)
    {
        Mesh *mesh = new Mesh();

        pMesh->QueryIntAttribute("id", &mesh->meshId);

        // read projection type
        str = pMesh->Attribute("type");

        if (strcmp(str, "wireframe") == 0) {
            mesh->type = 0;
        }
        else {
            mesh->type = 1;
        }

        // read mesh transformations
        XMLElement *pTransformations = pMesh->FirstChildElement("Transformations");
        XMLElement *pTransformation = pTransformations->FirstChildElement("Transformation");

        while (pTransformation != NULL)
        {
            char transformationType;
            int transformationId;

            str = pTransformation->GetText();
            sscanf(str, "%c %d", &transformationType, &transformationId);

            mesh->transformationTypes.push_back(transformationType);
            mesh->transformationIds.push_back(transformationId);

            pTransformation = pTransformation->NextSiblingElement("Transformation");
        }

        mesh->numberOfTransformations = mesh->transformationIds.size();

        // read mesh faces
        char *row;
        char *clone_str;
        int v1, v2, v3;
        XMLElement *pFaces = pMesh->FirstChildElement("Faces");
        str = pFaces->GetText();
        clone_str = strdup(str);

        row = strtok(clone_str, "\n");
        while (row != NULL)
        {
            int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);
            
            if (result != EOF) {
                mesh->triangles.push_back(Triangle(v1, v2, v3));
            }
            row = strtok(NULL, "\n");
        }
        mesh->numberOfTriangles = mesh->triangles.size();
        meshes.push_back(mesh);

        pMesh = pMesh->NextSiblingElement("Mesh");
    }
}

/*
    Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
    if (this->image.empty())
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            vector<Color> rowOfColors;

            for (int j = 0; j < camera->verRes; j++)
            {
                rowOfColors.push_back(this->backgroundColor);
            }

            this->image.push_back(rowOfColors);
        }
    }
    else
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            for (int j = 0; j < camera->verRes; j++)
            {
                this->image[i][j].r = this->backgroundColor.r;
                this->image[i][j].g = this->backgroundColor.g;
                this->image[i][j].b = this->backgroundColor.b;
            }
        }
    }
}

/*
    If given value is less than 0, converts value to 0.
    If given value is more than 255, converts value to 255.
    Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
    if (value >= 255.0)
        return 255;
    if (value <= 0.0)
        return 0;
    return (int)(value);
}

/*
    Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
    ofstream fout;

    fout.open(camera->outputFileName.c_str());

    fout << "P3" << endl;
    fout << "# " << camera->outputFileName << endl;
    fout << camera->horRes << " " << camera->verRes << endl;
    fout << "255" << endl;

    for (int j = camera->verRes - 1; j >= 0; j--)
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
                 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
                 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
        }
        fout << endl;
    }
    fout.close();
}

/*
    Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
    os_type == 1 		-> Ubuntu
    os_type == 2 		-> Windows
    os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
    string command;

    // call command on Ubuntu
    if (osType == 1)
    {
        command = "convert " + ppmFileName + " " + ppmFileName + ".png";
        system(command.c_str());
    }

    // call command on Windows
    else if (osType == 2)
    {
        command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
        system(command.c_str());
    }

    // default action - don't do conversion
    else
    {
    }
}