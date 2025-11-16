# Ghi Chú Lý Thuyết Chi Tiết - Đồ Án Mô Phỏng Sóng Biển 3D

Đây là tài liệu tổng hợp các kiến thức lý thuyết đã được áp dụng trong dự án, nhằm mục đích ôn tập và chuẩn bị cho các buổi vấn đáp.

---

### **Chương 1: Cấu Trúc Chương Trình và Các Khái Niệm Cơ Bản**

#### **1.1. Vòng Lặp Sự Kiện (Event Loop) của GLUT**

- **`glutInit(&argc, argv)`**: Khởi tạo thư viện GLUT. Đây là lệnh phải được gọi đầu tiên.
- **`glutCreateWindow(...)`**: Tạo một cửa sổ trên màn hình với tiêu đề được chỉ định.
- **`glutMainLoop()`**: Bắt đầu vòng lặp sự kiện chính. Chương trình sẽ ở trong vòng lặp này, lắng nghe các sự kiện từ người dùng (chuột, bàn phím) hoặc hệ thống (cần vẽ lại cửa sổ) và gọi các hàm xử lý tương ứng (callback) mà chúng ta đã đăng ký. Lệnh này không bao giờ kết thúc cho đến khi người dùng đóng cửa sổ.

#### **1.2. Các Hàm Callback (Hàm Gọi Lại)**

Callback là các hàm mà chúng ta viết ra và "đăng ký" với GLUT. GLUT sẽ tự động gọi các hàm này khi một sự kiện tương ứng xảy ra.
- **`glutDisplayFunc(display)`**: Đăng ký hàm `display` làm hàm vẽ chính. Nó được gọi mỗi khi cửa sổ cần được vẽ lại (ví dụ: lúc mới tạo, hoặc khi có lệnh `glutPostRedisplay`).
- **`glutReshapeFunc(reshape)`**: Được gọi khi kích thước cửa sổ thay đổi. Chúng ta dùng nó để cập nhật `glViewport` (khung nhìn) và ma trận chiếu (Projection Matrix).
- **`glutKeyboardFunc(keyboard)`**: Được gọi khi một phím trên bàn phím được nhấn.
- **`glutMouseFunc(mouse)`** và **`glutMotionFunc(motion)`**: Xử lý các sự kiện nhấn/thả chuột và di chuyển chuột khi đang nhấn.
- **`glutTimerFunc(ms, update, value)`**: Một công cụ tạo animation mạnh mẽ. Nó yêu cầu GLUT gọi hàm `update` sau một khoảng thời gian `ms` (mili-giây). Bên trong hàm `update`, chúng ta thường gọi lại chính `glutTimerFunc` để tạo một chuỗi các lần gọi lặp đi lặp lại, tạo ra chuyển động.

#### **1.3. Double Buffering và Z-Buffering**

- **Double Buffering (`GLUT_DOUBLE`)**: Kỹ thuật này sử dụng hai bộ đệm hình ảnh: một "front buffer" (đang được hiển thị trên màn hình) và một "back buffer" (nơi chúng ta đang vẽ frame tiếp theo). Khi vẽ xong, lệnh `glutSwapBuffers()` sẽ hoán đổi hai bộ đệm này. Điều này giúp loại bỏ hiện tượng "xé hình" (flickering), làm cho animation mượt mà hơn.
- **Z-Buffering / Depth-Testing (`GL_DEPTH_TEST`)**: Đây là thuật toán khử mặt khuất thuộc loại "không gian ảnh" (image-space).
    - Một "vùng đệm độ sâu" (depth buffer) có cùng kích thước với cửa sổ được tạo ra. Nó lưu giá trị độ sâu (tọa độ Z, khoảng cách tới camera) của pixel gần nhất đã được vẽ tại mỗi vị trí.
    - Khi vẽ một pixel mới, OpenGL so sánh độ sâu của nó với giá trị đang được lưu trong depth buffer.
    - Nếu pixel mới gần hơn, nó sẽ được vẽ và giá trị độ sâu mới sẽ được cập nhật vào buffer.
    - Nếu pixel mới ở xa hơn, nó sẽ bị loại bỏ.
    - Lệnh `glClear(GL_DEPTH_BUFFER_BIT)` phải được gọi ở đầu mỗi frame để xóa sạch buffer độ sâu từ frame trước.

---

### **Chương 2: Mô Hình Hóa và Animation**

#### **2.1. Mặt Cong Bezier (Bezier Surface)**

Để tạo ra một bề mặt sóng mượt mà, chúng ta sử dụng mặt cong Bezier bậc 3 (bicubic).
- **Lý thuyết**: Một điểm `P(u, v)` trên mặt cong được tính bằng cách nội suy từ một lưới các điểm điều khiển `P_i,j` (trong trường hợp của chúng ta là lưới 4x4 = 16 điểm).
- **Công thức**: `P(u, v) = Σ(i=0 to 3) Σ(j=0 to 3) [ B_i,3(u) * B_j,3(v) * P_i,j ]`
- **Đa thức Bernstein (`B_k,n(t)`)**: Đây là các hàm cơ sở (basis functions) của đường cong/mặt cong Bezier. Chúng quyết định "trọng số" hay "mức độ ảnh hưởng" của mỗi điểm điều khiển lên một điểm cụ thể trên bề mặt.
    - `B_0,3(t) = (1-t)^3`
    - `B_1,3(t) = 3t(1-t)^2`
    - `B_2,3(t) = 3t^2(1-t)`
    - `B_3,3(t) = t^3`
- **Tạo Lưới Đa Giác (Polygon Mesh)**: Bằng cách cho tham số `u` và `v` chạy từ 0 đến 1 theo các bước nhỏ (ví dụ: `1.0 / WAVE_RESOLUTION`), chúng ta tính toán ra một lưới các điểm trên bề mặt. Các điểm này sau đó được nối lại với nhau để tạo thành các mặt tứ giác (quads) hoặc tam giác (triangles), hình thành nên mô hình 3D của mặt sóng.

#### **2.2. Animation Sóng**

- **Nguyên lý**: Thay vì di chuyển toàn bộ lưới sóng, chúng ta chỉ cần thay đổi hình dạng của nó theo thời gian. Cách hiệu quả nhất là thay đổi vị trí của các điểm điều khiển.
- **Thực hiện**: Trong hàm `update`, chúng ta thay đổi tọa độ `y` của các điểm điều khiển `controlPoints[i][j]` bằng các hàm lượng giác như `sin()` và `cos()`.
- **Sóng liền mạch**: Để các mảng sóng khi xếp cạnh nhau không có "vết nối", công thức animation phải phụ thuộc vào tọa độ thế giới của điểm điều khiển, chứ không phải chỉ số `i, j` của mảng.
    - `offset = sin(time + worldX * freqX) + cos(time + worldZ * freqZ)`
    - Điều này đảm bảo rằng sóng là một hàm liên tục của không gian và thời gian.

---

### **Chương 3: Các Phép Biến Đổi và Quan Sát**

#### **3.1. Ngăn Xếp Ma Trận (Matrix Stack)**

- OpenGL sử dụng một ngăn xếp ma trận (thường là `GL_MODELVIEW`) để quản lý các phép biến đổi.
- **`glPushMatrix()`**: Sao chép ma trận ở đỉnh ngăn xếp và đẩy bản sao đó lên trên cùng. Giống như lệnh "Save".
- **`glPopMatrix()`**: Loại bỏ ma trận ở đỉnh ngăn xếp, khôi phục lại ma trận ở ngay dưới nó. Giống như lệnh "Load" hay "Undo".
- **Ứng dụng**: Khi vẽ nhiều vật thể, chúng ta dùng cặp lệnh này để đảm bảo các phép biến đổi của vật thể này (ví dụ: tịnh tiến, quay thuyền) không ảnh hưởng đến các vật thể khác (ví dụ: sóng biển, các con thuyền khác).

#### **3.2. Các Phép Biến Đổi Cơ Bản**

- **`glTranslatef(x, y, z)`**: Tịnh tiến (di chuyển) hệ tọa độ hiện tại.
- **`glRotatef(angle, x, y, z)`**: Quay hệ tọa độ hiện tại một góc `angle` quanh trục vector `(x, y, z)`. Đây là cách thực hiện "phép quay quanh một trục bất kỳ" trong OpenGL. Về mặt lý thuyết, nó tương đương với một chuỗi các phép nhân ma trận phức tạp để đưa trục về trùng với trục chính, quay, rồi lại đưa về vị trí cũ.
- **`glScalef(x, y, z)`**: Co giãn hệ tọa độ.

#### **3.3. Camera và Phép Chiếu**

- **Mô hình Camera (`gluLookAt`)**: Hàm này định nghĩa phép biến đổi quan sát (Viewing Transformation), chuyển từ hệ tọa độ thế giới (World Coordinate System) sang hệ tọa độ quan sát (Viewing Coordinate System). Nó nhận 3 tham số:
    1.  `eye`: Vị trí của camera.
    2.  `center`: Điểm mà camera đang nhìn vào.
    3.  `up`: Vector chỉ hướng "lên trên" của camera.
- **Phép Chiếu Phối Cảnh (`gluPerspective`)**: Hàm này định nghĩa phép chiếu phối cảnh (Perspective Projection), tạo ra cảm giác về chiều sâu (vật ở xa trông nhỏ hơn). Nó nhận các tham số:
    1.  `fovy`: Góc nhìn theo chiều dọc.
    2.  `aspect`: Tỷ lệ khung hình (rộng/cao).
    3.  `zNear`, `zFar`: Khoảng cách đến mặt phẳng cắt gần và mặt phẳng cắt xa. Bất cứ thứ gì ở quá gần hoặc quá xa sẽ không được vẽ.

---

### **Chương 4: Chiếu Sáng và Tô Bóng (Lighting & Shading)**

#### **4.1. Mô Hình Chiếu Sáng Phong (Phong Reflection Model)**

Đây là một mô hình cục bộ (local illumination model), tính toán màu sắc của một điểm dựa trên 3 thành phần:
1.  **Ambient (Môi trường)**: Ánh sáng nền, có cường độ như nhau ở mọi nơi, giúp các vùng trong bóng tối không bị đen hoàn toàn. `I_ambient = GlobalAmbient * MaterialAmbient`.
2.  **Diffuse (Khuếch tán)**: Ánh sáng từ một nguồn sáng bị phản xạ đồng đều theo mọi hướng. Cường độ phụ thuộc vào góc giữa vector pháp tuyến của bề mặt (`N`) và vector chỉ hướng tới nguồn sáng (`L`). `I_diffuse = LightColor * MaterialColor * max(0, N · L)`.
3.  **Specular (Phản xạ gương)**: Ánh sáng phản xạ có hướng, tạo ra các đốm sáng bóng (highlight). Cường độ phụ thuộc vào góc giữa vector quan sát (`V`) và vector phản xạ của ánh sáng (`R`). `I_specular = LightColor * MaterialSpecular * (R · V)^shininess`.

#### **4.2. Nguồn Sáng và Vật Liệu trong OpenGL**

- **Nguồn sáng (`glLightfv`)**:
    - `GL_POSITION`: Vị trí của nguồn sáng. Nếu thành phần thứ 4 (`w`) bằng 1, đó là **Point Light**. Nếu `w` bằng 0, đó là **Directional Light** (nguồn sáng ở vô cực, `(x,y,z)` trở thành vector chỉ hướng).
    - `GL_AMBIENT`, `GL_DIFFUSE`, `GL_SPECULAR`: Màu sắc của nguồn sáng cho từng thành phần.
- **Vật liệu (`glMaterialfv`)**:
    - `GL_AMBIENT`, `GL_DIFFUSE`, `GL_SPECULAR`: Màu sắc phản xạ của vật liệu cho từng thành phần.
    - `GL_SHININESS`: Độ bóng của vật liệu, ảnh hưởng đến kích thước và độ sắc nét của đốm sáng specular.

#### **4.3. Tô Bóng (Shading Models)**

- **`glShadeModel(GL_FLAT)` (Tô bóng hằng/Lambert)**: Cả một mặt đa giác (tam giác) được tô bằng một màu duy nhất. Màu này được tính toán dựa trên vector pháp tuyến của mặt đó. Kết quả là hình ảnh trông góc cạnh, từng mảng màu rõ rệt.
- **`glShadeModel(GL_SMOOTH)` (Tô bóng Gouraud)**: Đây là chế độ mặc định của OpenGL khi bật `GL_SMOOTH`.
    1.  Màu sắc được tính toán tại **mỗi đỉnh** của đa giác (dựa trên vector pháp tuyến tại đỉnh đó).
    2.  Sau đó, màu sắc của các pixel bên trong đa giác được **nội suy** từ màu của các đỉnh.
    3.  Kết quả mượt mà hơn Flat Shading, nhưng có thể bỏ lỡ các đốm sáng specular ở giữa mặt đa giác.
- **Tô bóng Phong (Phong Shading)**: *Lưu ý: OpenGL trong chế độ cũ (fixed-function pipeline) không thực sự hỗ trợ tô bóng Phong, mà chỉ hỗ trợ tô bóng Gouraud.* Tô bóng Phong thực sự (yêu cầu shader) sẽ nội suy vector pháp tuyến cho từng pixel, sau đó mới tính toán màu sắc. Tuy nhiên, bằng cách cung cấp vector pháp tuyến tại mỗi đỉnh (`glNormal3f`) và sử dụng `GL_SMOOTH`, chúng ta có được kết quả tiệm cận, mượt mà và thường được chấp nhận là "tô bóng kiểu Phong" trong bối cảnh OpenGL cũ.

---

### **Chương 5: Kỹ Thuật Mở Rộng**

#### **5.1. Tiling (Xếp Gạch)**
Để tạo ảo giác về một bề mặt vô tận, chúng ta vẽ một vật thể (mảng sóng) nhiều lần theo một lưới. Sử dụng `glPushMatrix`, `glTranslatef` và `glPopMatrix` trong một vòng lặp để đặt các bản sao của mảng sóng cạnh nhau.

#### **5.2. Skydome (Vòm Trời)**
Một kỹ thuật đơn giản để tạo bầu trời là vẽ một nửa hình cầu (hemisphere) rất lớn bao quanh toàn bộ cảnh. Bằng cách gán màu khác nhau cho các đỉnh ở các độ cao khác nhau (xanh nhạt ở đường chân trời, xanh đậm ở đỉnh), chúng ta có thể tạo ra hiệu ứng chuyển màu (gradient) cho bầu trời.

#### **5.3. Vật Thể Tự Phát Sáng**
Để vẽ một vật thể không bị ảnh hưởng bởi ánh sáng (như Mặt Trời), chúng ta có thể:
1.  Tắt tạm thời hệ thống chiếu sáng: `glDisable(GL_LIGHTING)`.
2.  Thiết lập màu cho vật thể: `glColor3f(...)`.
3.  Vẽ vật thể.
4.  Bật lại hệ thống chiếu sáng: `glEnable(GL_LIGHTING)`.
Tất cả nên được thực hiện trong một cặp `glPushMatrix`/`glPopMatrix` để không ảnh hưởng đến các đối tượng khác.
