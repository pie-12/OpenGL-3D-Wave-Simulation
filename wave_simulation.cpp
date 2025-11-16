#include <GL/glut.h>
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Struct để lưu trữ một điểm hoặc vector 3D
struct Vec3 {
    float x, y, z;
};

// ===================================================================
// PHẦN 1: CÁC HÀM TÍNH TOÁN BỀ MẶT BEZIER
// ===================================================================

// 16 điểm điều khiển (Control Points) cho mặt cong Bezier bậc 3
Vec3 controlPoints[4][4];

// Mảng lưu trữ các đỉnh và pháp tuyến của lưới
const int DIVISIONS = 40; // Độ chi tiết của lưới
Vec3 vertices[DIVISIONS + 1][DIVISIONS + 1];
Vec3 normals[DIVISIONS + 1][DIVISIONS + 1];

// Hàm tính toán đa thức Bernstein cơ sở
// B_i,n(t) = C(n,i) * t^i * (1-t)^(n-i)
float bernstein(int i, int n, float t) {
    if (n == 3) {
        switch (i) {
            case 0: return (1 - t) * (1 - t) * (1 - t);
            case 1: return 3 * t * (1 - t) * (1 - t);
            case 2: return 3 * t * t * (1 - t);
            case 3: return t * t * t;
        }
    }
    if (n == 2) {
        switch (i) {
            case 0: return (1 - t) * (1 - t);
            case 1: return 2 * t * (1 - t);
            case 2: return t * t;
        }
    }
    return 0;
}

/*
 * 1. Hàm evalBezierSurface()
 * --------------------------
 * Tính toán tọa độ của một điểm P(u,v) trên bề mặt Bezier
 * dựa trên các điểm điều khiển và đa thức Bernstein.
 */
void evalBezierSurface(float u, float v, Vec3& point) {
    point = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float blend = bernstein(i, 3, u) * bernstein(j, 3, v);
            point.x += controlPoints[i][j].x * blend;
            point.y += controlPoints[i][j].y * blend;
            point.z += controlPoints[i][j].z * blend;
        }
    }
}

// Tính toán vector đạo hàm riêng dP/du
void eval_dU(float u, float v, Vec3& dU) {
    dU = {0.0f, 0.0f, 0.0f};
    // d/du [P(u,v)] = Sum_i Sum_j P_ij * B'_i,3(u) * B_j,3(v)
    // B'_i,n(t) = n * (B_i-1,n-1(t) - B_i,n-1(t))
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float blend = (3.0f * (bernstein(i - 1, 2, u) - bernstein(i, 2, u))) * bernstein(j, 3, v);
            dU.x += controlPoints[i][j].x * blend;
            dU.y += controlPoints[i][j].y * blend;
            dU.z += controlPoints[i][j].z * blend;
        }
    }
}

// Tính toán vector đạo hàm riêng dP/dv
void eval_dV(float u, float v, Vec3& dV) {
    dV = {0.0f, 0.0f, 0.0f};
    // d/dv [P(u,v)] = Sum_i Sum_j P_ij * B_i,3(u) * B'_j,3(v)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float blend = bernstein(i, 3, u) * (3.0f * (bernstein(j - 1, 2, v) - bernstein(j, 2, v)));
            dV.x += controlPoints[i][j].x * blend;
            dV.y += controlPoints[i][j].y * blend;
            dV.z += controlPoints[i][j].z * blend;
        }
    }
}

// Hàm tính tích có hướng (code "tay")
Vec3 crossProduct(const Vec3& a, const Vec3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

// Hàm chuẩn hóa vector (code "tay")
void normalize(Vec3& v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length > 1e-6) {
        v.x /= length;
        v.y /= length;
        v.z /= length;
    }
}

// ===================================================================
// PHẦN 2: CÁC HÀM VẼ VÀ CẬP NHẬT
// ===================================================================

/*
 * 2. Hàm drawBezierSurface()
 * --------------------------
 * Tính toán lại toàn bộ các đỉnh và pháp tuyến cho lưới, sau đó vẽ ra màn hình.
 * Pháp tuyến được tính bằng Tích có hướng của hai vector Đạo hàm riêng.
 */
void drawBezierSurface() {
    // Bước 1: Tính toán lại toàn bộ các đỉnh và pháp tuyến
    for (int i = 0; i <= DIVISIONS; ++i) {
        for (int j = 0; j <= DIVISIONS; ++j) {
            float u = (float)i / DIVISIONS;
            float v = (float)j / DIVISIONS;

            // Tính tọa độ đỉnh
            evalBezierSurface(u, v, vertices[i][j]);

            // Tính 2 vector đạo hàm riêng
            Vec3 dU, dV;
            eval_dU(u, v, dU);
            eval_dV(u, v, dV);

            // Tính pháp tuyến bằng tích có hướng và chuẩn hóa
            normals[i][j] = crossProduct(dU, dV);
            normalize(normals[i][j]);
        }
    }

    // Bước 2: Vẽ lưới bằng GL_TRIANGLE_STRIP
    for (int i = 0; i < DIVISIONS; ++i) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= DIVISIONS; ++j) {
            // Cung cấp pháp tuyến và đỉnh cho dải tam giác
            glNormal3f(normals[i][j].x, normals[i][j].y, normals[i][j].z);
            glVertex3f(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);

            glNormal3f(normals[i + 1][j].x, normals[i + 1][j].y, normals[i + 1][j].z);
            glVertex3f(vertices[i + 1][j].x, vertices[i + 1][j].y, vertices[i + 1][j].z);
        }
        glEnd();
    }
}

/*
 * 3. Hàm update()
 * --------------------------
 * Cập nhật trạng thái của animation.
 * Tọa độ Y của các điểm điều khiển được thay đổi theo hàm sin(time).
 */
void update() {
    static float time = 0.0f;
    time += 0.02f; // Tốc độ animation

    // Cập nhật chiều cao (Y) của các điểm điều khiển
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float offset = sin(time + i * 0.5f + j * 0.3f) * 0.3f;
            controlPoints[i][j].y = offset;
        }
    }

    glutPostRedisplay(); // Yêu cầu GLUT vẽ lại màn hình
}

// ===================================================================
// PHẦN 3: CÁC HÀM CỦA GLUT
// ===================================================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Thiết lập camera
    gluLookAt(5.0, 4.0, 8.0,  // Vị trí mắt
              0.0, 0.0, 0.0,  // Nhìn vào tâm
              0.0, 1.0, 0.0); // Vector hướng lên

    // Thiết lập vị trí nguồn sáng
    GLfloat light_pos[] = {5.0, 10.0, 5.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Vẽ bề mặt sóng
    drawBezierSurface();

    glutSwapBuffers();
}

/*
 * 4. Hàm init()
 * --------------------------
 * Thiết lập trạng thái ban đầu của OpenGL.
 * Bật Z-buffer, ánh sáng, và các thành phần GL_SPECULAR, GL_SHININESS.
 */
void init() {
    glClearColor(0.05f, 0.1f, 0.2f, 1.0f); // Màu nền xanh đậm

    // Bật Z-buffer
    glEnable(GL_DEPTH_TEST);

    // --- Thiết lập Ánh sáng và Vật liệu ---
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH); // Bật tô bóng mượt (Gouraud)

    // Thiết lập thuộc tính nguồn sáng
    GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat diffuse[] = {0.8, 0.8, 1.0, 1.0};
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0}; // Ánh sáng phản xạ lấp lánh màu trắng
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Thiết lập thuộc tính vật liệu cho mặt nước
    GLfloat mat_ambient[] = {0.1f, 0.3f, 0.5f, 1.0f};
    GLfloat mat_diffuse[] = {0.3f, 0.5f, 0.8f, 1.0f};
    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Phản xạ lấp lánh màu trắng
    GLfloat mat_shininess[] = {120.0f}; // Độ bóng cao để highlight sắc nét
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // Khởi tạo vị trí ban đầu của các điểm điều khiển
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            controlPoints[i][j] = {
                (float)i * 1.5f - 2.25f,
                0.0f,
                (float)j * 1.5f - 2.25f
            };
        }
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (h == 0) h = 1;
    gluPerspective(45.0, (float)w / h, 1.0, 100.0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // Phím ESC
        exit(0);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Advanced 3D Wave Simulation (Bezier Surface with Specular)");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(update); // Sử dụng glutIdleFunc cho animation

    glutMainLoop();
    return 0;
}
