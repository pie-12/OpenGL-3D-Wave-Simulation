# Ghi Chú Tiếp Tục Dự Án (Tiếng Việt)

## Mục Đích
File này ghi lại toàn bộ tiến trình của dự án. Khi một phiên làm việc mới bắt đầu, chỉ cần cung cấp lại file này, tôi (Gemini CLI) sẽ đọc, ghi nhớ lại mọi thứ về trạng thái dự án và biết cần phải làm gì tiếp theo.

---

### **I. Trạng Thái Hiện Tại (Cập nhật ngày 16/11/2025)**

**Commit cuối cùng:** `4058235` - "feat: Add skydome, tiled ocean, and directional sun"

**Mô tả chi tiết các tính năng đã triển khai:**
Dự án hiện tại là một chương trình mô phỏng 3D hoàn chỉnh, thể hiện một cảnh quan đại dương sống động với các yếu tố sau:

1.  **Đại dương Vô tận:**
    *   **Kỹ thuật:** Thay vì một mảng sóng duy nhất, chương trình vẽ một lưới 5x5 các mảng sóng.
    *   **Hiệu quả:** Tạo ra ảo ảnh về một mặt biển rộng lớn, trải dài đến tận chân trời.
    *   **Cải tiến:** Công thức animation của sóng đã được sửa đổi để dựa trên tọa độ thế giới, giúp các con sóng di chuyển một cách liền mạch và tự nhiên qua các mảng lưới, không còn "vết nối".

2.  **Bầu trời và Mặt trời:**
    *   **Bầu trời (Skydome):** Một mái vòm (nửa hình cầu) khổng lồ được vẽ để bao quanh cảnh quan. Các đỉnh của mái vòm được tô màu chuyển sắc (gradient) từ xanh đậm trên đỉnh đầu xuống xanh nhạt ở đường chân trời.
    *   **Mặt trời:** Một quả cầu nhỏ màu vàng, tự phát sáng (vẽ khi đã tắt `GL_LIGHTING`) được đặt trên bầu trời để người dùng có thể thấy được vị trí của nguồn sáng chính.

3.  **Ánh sáng Chân thực:**
    *   **Nguồn sáng định hướng (Directional Light):** Nguồn sáng chính đã được chuyển từ dạng điểm (point light) sang dạng định hướng (directional light). Điều này mô phỏng chính xác hơn ánh sáng từ Mặt Trời ở rất xa, với các tia sáng song song, tạo ra hiệu ứng đổ bóng đồng đều và tự nhiên trên toàn bộ mặt biển và con thuyền.

4.  **Thuyền Nhấp Nhô:**
    *   **Mô hình:** Một con thuyền đơn giản được lắp ghép từ 2 khối hộp (thân và cabin).
    *   **Tương tác với sóng:** Vị trí theo chiều dọc (trục Y) của con thuyền được tính toán lại trong mỗi frame dựa trên chiều cao của mặt sóng ngay tại tâm. Điều này làm cho con thuyền nhấp nhô lên xuống một cách tự nhiên theo sóng biển.

### **II. Các Hướng Phát Triển Tiếp Theo**

Dựa trên các tính năng đã có và yêu cầu lý thuyết ban đầu, đây là các bước nâng cấp tiềm năng:

1.  **Nâng cao Tương tác Sóng-Thuyền: Làm Thuyền Nghiêng Theo Sóng**
    *   **Mục tiêu:** Hiện tại thuyền chỉ nhấp nhô lên xuống. Để chân thực hơn, thuyền cần phải nghiêng theo độ dốc của mặt sóng.
    *   **Lý thuyết:** Chúng ta cần tính toán vector pháp tuyến (Normal Vector) của mặt sóng ngay tại vị trí của con thuyền. Vector này đại diện cho "hướng thẳng đứng" của mặt sóng tại điểm đó.
    *   **Thực hiện:** Sử dụng vector pháp tuyến này làm vector "up" mới cho con thuyền và tính toán các phép quay cần thiết để trục Y của con thuyền luôn thẳng hàng với vector pháp tuyến của sóng. Đây là một bài toán biến đổi hình học thú vị.

2.  **Thêm Tùy Chọn Đồ Họa Cho Người Dùng**
    *   **Mục tiêu:** Cho phép người dùng tự mình so sánh các kỹ thuật đồ họa khác nhau.
    *   **Thực hiện:** Thêm các phím bấm để chuyển đổi (toggle) giữa:
        *   **Chế độ tô bóng:** `glShadeModel(GL_SMOOTH)` (Tô bóng Gouraud) vs. `glShadeModel(GL_FLAT)` (Tô bóng Lambert/Hằng số).
        *   **Chế độ chiếu:** `gluPerspective(...)` (Phối cảnh) vs. `glOrtho(...)` (Song song).

3.  **Tái Cấu Trúc Mã Nguồn (Code Refactoring)**
    *   **Vấn đề:** Toàn bộ mã nguồn đang nằm trong một file `wave_simulation.cpp`, gây khó khăn cho việc bảo trì và mở rộng.
    *   **Giải pháp:** Tách mã nguồn ra thành các file có tổ chức hơn, ví dụ:
        *   `main.cpp`: Chứa hàm `main` và các hàm callback của GLUT.
        *   `wave.h` / `wave.cpp`: Chứa các hàm liên quan đến việc tạo và vẽ sóng.
        *   `scene.h` / `scene.cpp`: Chứa các hàm vẽ vật thể phụ như thuyền, bầu trời.
        *   `camera.h` / `camera.cpp`: Chứa các logic về điều khiển camera.
    *   **Lợi ích:** Giúp mã nguồn sạch sẽ, chuyên nghiệp, dễ đọc và dễ nâng cấp hơn rất nhiều.
