// Bắt đầu mã nguồn đầy đủ
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring> // For memcpy

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Cấu trúc dữ liệu ---
struct Point3D {
    float x, y, z;
};

struct Face {
    int v1, v2, v3;
};

struct PolygonMesh {
    std::vector<Point3D> vertices;
    std::vector<Point3D> normals;
    std::vector<Face> faces;
};

// --- Biến toàn cục ---
int screenWidth = 1024;
int screenHeight = 768;

// Camera
float cameraAngleX = 45.0f;
float cameraAngleY = 30.0f;
float cameraDistance = 15.0f;
int lastMouseX, lastMouseY;
int mouseButton;

// Animation
float animationTime = 0.0f;
float objectAngle = 0.0f; // Góc quay cho vật thể

// Mô hình sóng
PolygonMesh waveMesh;
const int WAVE_RESOLUTION = 30; // Độ chi tiết của lưới sóng

// Điểm điều khiển Bezier
Point3D controlPoints[4][4];
Point3D initialControlPoints[4][4];

// --- Các hàm tính toán ---

float bernstein(int i, float t) {
    if (i == 0) return (1 - t) * (1 - t) * (1 - t);
    if (i == 1) return 3 * t * (1 - t) * (1 - t);
    if (i == 2) return 3 * t * t * (1 - t);
    if (i == 3) return t * t * t;
    return 0;
}

Point3D calculateBezierPoint(float u, float v) {
    Point3D point = {0, 0, 0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float blend = bernstein(i, u) * bernstein(j, v);
            point.x += controlPoints[i][j].x * blend;
            point.y += controlPoints[i][j].y * blend;
            point.z += controlPoints[i][j].z * blend;
        }
    }
    return point;
}

Point3D crossProduct(Point3D a, Point3D b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

void normalize(Point3D& p) {
    float len = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    if (len > 1e-6) {
        p.x /= len; p.y /= len; p.z /= len;
    }
}

void createOrUpdateWaveMesh() {
    waveMesh.vertices.resize((WAVE_RESOLUTION + 1) * (WAVE_RESOLUTION + 1));
    if (waveMesh.faces.empty()) {
        waveMesh.faces.resize(WAVE_RESOLUTION * WAVE_RESOLUTION * 2);
    }
    waveMesh.normals.resize(waveMesh.vertices.size());

    // Tạo đỉnh
    for (int i = 0; i <= WAVE_RESOLUTION; i++) {
        for (int j = 0; j <= WAVE_RESOLUTION; j++) {
            float u = (float)i / WAVE_RESOLUTION;
            float v = (float)j / WAVE_RESOLUTION;
            waveMesh.vertices[i * (WAVE_RESOLUTION + 1) + j] = calculateBezierPoint(u, v);
        }
    }

    // Tạo mặt (chỉ làm một lần)
    if (waveMesh.faces[0].v1 == 0 && waveMesh.faces[0].v2 == 0) {
        int faceIndex = 0;
        for (int i = 0; i < WAVE_RESOLUTION; i++) {
            for (int j = 0; j < WAVE_RESOLUTION; j++) {
                int v0 = i * (WAVE_RESOLUTION + 1) + j;
                int v1 = v0 + 1;
                int v2 = (i + 1) * (WAVE_RESOLUTION + 1) + j;
                int v3 = v2 + 1;
                waveMesh.faces[faceIndex++] = {v0, v2, v1};
                waveMesh.faces[faceIndex++] = {v1, v2, v3};
            }
        }
    }

    // Tính toán pháp tuyến đỉnh
    for (auto& normal : waveMesh.normals) normal = {0, 0, 0};
    for (const auto& face : waveMesh.faces) {
        Point3D p1 = waveMesh.vertices[face.v1];
        Point3D p2 = waveMesh.vertices[face.v2];
        Point3D p3 = waveMesh.vertices[face.v3];
        Point3D edge1 = {p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
        Point3D edge2 = {p3.x - p1.x, p3.y - p1.y, p3.z - p1.z};
        Point3D faceNormal = crossProduct(edge1, edge2);
        waveMesh.normals[face.v1].x += faceNormal.x; waveMesh.normals[face.v1].y += faceNormal.y; waveMesh.normals[face.v1].z += faceNormal.z;
        waveMesh.normals[face.v2].x += faceNormal.x; waveMesh.normals[face.v2].y += faceNormal.y; waveMesh.normals[face.v2].z += faceNormal.z;
        waveMesh.normals[face.v3].x += faceNormal.x; waveMesh.normals[face.v3].y += faceNormal.y; waveMesh.normals[face.v3].z += faceNormal.z;
    }
    for (auto& normal : waveMesh.normals) normalize(normal);
}

void initializeControlPoints() {
    float size = 4.0f;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            initialControlPoints[i][j] = {
                (i / 3.0f) * size - size / 2.0f,
                0.0f,
                (j / 3.0f) * size - size / 2.0f
            };
        }
    }
    memcpy(controlPoints, initialControlPoints, sizeof(controlPoints));
}

// --- Hàm vẽ vật thể ---
void drawBlock(float width, float height, float depth) {
    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;

    glBegin(GL_QUADS);
        // Mặt trước
        glNormal3f(0.0, 0.0, 1.0);
        glVertex3f(-w, -h, d);
        glVertex3f(w, -h, d);
        glVertex3f(w, h, d);
        glVertex3f(-w, h, d);
        // Mặt sau
        glNormal3f(0.0, 0.0, -1.0);
        glVertex3f(-w, -h, -d);
        glVertex3f(-w, h, -d);
        glVertex3f(w, h, -d);
        glVertex3f(w, -h, -d);
        // Mặt trên
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(-w, h, d);
        glVertex3f(w, h, d);
        glVertex3f(w, h, -d);
        glVertex3f(-w, h, -d);
        // Mặt dưới
        glNormal3f(0.0, -1.0, 0.0);
        glVertex3f(-w, -h, d);
        glVertex3f(-w, -h, -d);
        glVertex3f(w, -h, -d);
        glVertex3f(w, -h, d);
        // Mặt phải
        glNormal3f(1.0, 0.0, 0.0);
        glVertex3f(w, -h, d);
        glVertex3f(w, -h, -d);
        glVertex3f(w, h, -d);
        glVertex3f(w, h, d);
        // Mặt trái
        glNormal3f(-1.0, 0.0, 0.0);
        glVertex3f(-w, -h, d);
        glVertex3f(-w, h, d);
        glVertex3f(-w, h, -d);
        glVertex3f(-w, -h, -d);
    glEnd();
}

void drawBoat() {
    glPushMatrix();
    // Thân tàu
    GLfloat hullAmbient[] = { 0.6f, 0.4f, 0.2f, 1.0f };
    GLfloat hullDiffuse[] = { 0.6f, 0.4f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, hullAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, hullDiffuse);
    drawBlock(2.0f, 0.4f, 0.8f);

    // Cabin
    GLfloat cabinAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat cabinDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, cabinAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cabinDiffuse);
    glTranslatef(0.0f, 0.45f, 0.0f); // Di chuyển cabin lên trên thân tàu
    drawBlock(0.6f, 0.5f, 0.6f);
    glPopMatrix();
}


// --- Hàm của OpenGL ---

void init() {
    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    GLfloat ambientLight[] = { 0.2f, 0.3f, 0.4f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    GLfloat diffuseLight[] = { 0.8f, 0.8f, 1.0f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    initializeControlPoints();
    createOrUpdateWaveMesh();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)screenWidth / screenHeight, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float camX = cameraDistance * sin(cameraAngleX * M_PI / 180.0) * cos(cameraAngleY * M_PI / 180.0);
    float camY = cameraDistance * sin(cameraAngleY * M_PI / 180.0);
    float camZ = cameraDistance * cos(cameraAngleX * M_PI / 180.0) * cos(cameraAngleY * M_PI / 180.0);
    gluLookAt(camX, camY, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    GLfloat lightPosition[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // --- Vẽ thuyền ---
    glPushMatrix();

    // Tính toán vị trí nhấp nhô
    // Vị trí (x,z) của thuyền trong thế giới là (0,0)
    // Tham số (u,v) tương ứng trên mặt Bezier là (0.5, 0.5)
    Point3D boatPosOnWave = calculateBezierPoint(0.5f, 0.5f);
    float boatY = boatPosOnWave.y;

    // Thiết lập vật liệu chung cho thuyền (sẽ được ghi đè trong drawBoat)
    GLfloat boatSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat boatShininess[] = { 32.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, boatSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, boatShininess);

    // Áp dụng các phép biến đổi cho thuyền
    glTranslatef(0.0f, boatY + 0.2f, 0.0f); // Di chuyển thuyền đến vị trí trên sóng
    glRotatef(objectAngle, 0.0f, 1.0f, 0.0f); // Quay thuyền quanh trục Y

    drawBoat(); // Vẽ thuyền

    glPopMatrix();

    // --- Vẽ sóng biển ---
    GLfloat waveAmbient[] = { 0.3f, 0.5f, 0.8f, 1.0f };
    GLfloat waveDiffuse[] = { 0.5f, 0.7f, 1.0f, 1.0f };
    GLfloat waveSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat waveShininess[] = { 120.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, waveAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, waveDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, waveSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, waveShininess);

    glBegin(GL_TRIANGLES);
    for (const auto& face : waveMesh.faces) {
        int v_idx[] = {face.v1, face.v2, face.v3};
        for (int i = 0; i < 3; ++i) {
            glNormal3f(waveMesh.normals[v_idx[i]].x, waveMesh.normals[v_idx[i]].y, waveMesh.normals[v_idx[i]].z);
            glVertex3f(waveMesh.vertices[v_idx[i]].x, waveMesh.vertices[v_idx[i]].y, waveMesh.vertices[v_idx[i]].z);
        }
    }
    glEnd();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    screenWidth = w;
    screenHeight = h > 0 ? h : 1;
    glViewport(0, 0, screenWidth, screenHeight);
}

void update(int value) {
    // Cập nhật sóng
    animationTime += 0.03f;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float offset = sin(animationTime + i * 0.8f + j * 0.3f) * 0.4f;
            controlPoints[i][j].y = initialControlPoints[i][j].y + offset;
        }
    }
    createOrUpdateWaveMesh();

    // Cập nhật góc quay của vật thể
    objectAngle += 0.5f;
    if (objectAngle > 360.0f) {
        objectAngle -= 360.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void mouse(int button, int state, int x, int y) {
    lastMouseX = x;
    lastMouseY = y;
    mouseButton = button;
}

void motion(int x, int y) {
    if (mouseButton == GLUT_LEFT_BUTTON) {
        cameraAngleX += (x - lastMouseX) * 0.5f;
        cameraAngleY += (y - lastMouseY) * 0.5f;
        if (cameraAngleY > 89.0f) cameraAngleY = 89.0f;
        if (cameraAngleY < -89.0f) cameraAngleY = -89.0f;
    } else if (mouseButton == GLUT_RIGHT_BUTTON) {
        cameraDistance -= (y - lastMouseY) * 0.1f;
        if (cameraDistance < 2.0f) cameraDistance = 2.0f;
    }
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("3D Wave Simulation - Nguyen Tung Lam");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(16, update, 0);

    std::cout << "Interaction:\n";
    std::cout << "- Drag Left Mouse Button: Rotate camera\n";
    std::cout << "- Drag Right Mouse Button: Zoom in/out\n";
    std::cout << "- ESC: Exit\n";

    glutMainLoop();
    return 0;
}
// Kết thúc mã nguồn
