# Prompt Khôi phục Bối cảnh cho Dự án Sóng Biển 3D

### 1. 👤 Bối cảnh & Vai trò của AI
*   **Người dùng:** Sinh viên, đang học lại các khái niệm đồ họa máy tính cơ bản.
*   **Vai trò của AI:** Chuyên gia lập trình đồ họa OpenGL Cổ điển, người hướng dẫn và thực thi dự án theo yêu cầu.

### 2. 🎓 Mục tiêu Dự án
*   **Sóng biển 3D:** Xây dựng một chương trình C++/GLUT mô phỏng bề mặt sóng biển 3D, tuân thủ nghiêm ngặt các kiến thức trong chương trình học.
*   **Yêu cầu chất lượng:** Tạo ra một bề mặt nước mượt mà, chuyển động liên tục, và có hiệu ứng ánh sáng **"lấp lánh" (specular highlights)** chân thực.

### 3. ⚙️ Tình trạng Hiện tại của Dự án

#### Bối cảnh:
Sau khi xây dựng các tính năng mở rộng (thuyền, đại dương, bầu trời), người dùng nhận thấy chất lượng của bề mặt sóng vẫn còn "khá xấu" và chưa đạt được hiệu ứng lấp lánh như mong muốn. Chúng ta đã quyết định **tái cấu trúc (refactor)** toàn bộ dự án để tập trung vào yêu cầu cốt lõi: chất lượng bề mặt sóng và hiệu ứng ánh sáng.

#### Hành động vừa hoàn thành:
1.  **Tái cấu trúc mã nguồn:** Toàn bộ file `wave_simulation.cpp` đã được viết lại. Các tính năng phụ (thuyền, bầu trời, lưới đại dương) đã bị loại bỏ.
2.  **Nâng cấp Vector Pháp tuyến:** Triển khai phương pháp tính vector pháp tuyến "chuẩn" cho mặt Bezier bằng cách lấy **tích có hướng của hai vector đạo hàm riêng (∂P/∂u × ∂P/∂v)**. Đây là thay đổi quan trọng nhất để cải thiện chất lượng chiếu sáng.
3.  **Tinh chỉnh Ánh sáng:** Tăng giá trị `GL_SHININESS` lên `120.0f` để làm cho các đốm sáng specular trở nên sắc nét hơn.
4.  **Cập nhật Animation:** Chuyển từ `glutTimerFunc` sang `glutIdleFunc` theo yêu cầu.
5.  **Cập nhật Tài liệu:** Cả hai file `NOTES.md` và `CONTINUATION_PROMPT.md` đều đã được cập nhật để phản ánh những thay đổi lớn này. Toàn bộ tiến trình đã được commit và push lên GitHub.

### 4. 🚀 Bước Tiếp theo Ngay Bây Giờ

**Tình trạng hiện tại:**
Chúng ta đã có một nền tảng vững chắc: một chương trình mô phỏng một mảng sóng duy nhất với chất lượng đồ họa và hiệu ứng ánh sáng cao, đúng như yêu cầu cốt lõi.

**Nhiệm vụ:**
Dựa trên nền tảng chất lượng cao này, chúng ta sẽ quyết định hướng phát triển tiếp theo để làm cho đồ án vừa đẹp, vừa đầy đủ tính năng.

**Hành động trước mắt:**
Thảo luận và lựa chọn một trong các hướng đi sau (hoặc một hướng đi mới).

**Câu hỏi để bắt đầu phiên làm việc tới:**
"Chúng ta đã hoàn thành việc tái cấu trúc để có được bề mặt sóng chất lượng cao với hiệu ứng lấp lánh như ý. Bây giờ, bạn muốn chúng ta:
1.  Xây dựng lại tính năng **"đại dương vô tận" và "bầu trời"** trên nền tảng mới này?
2.  Thêm lại **"con thuyền"** và làm nó **nghiêng theo độ dốc của sóng** (một thử thách mới và thú vị)?
3.  Hay bạn muốn khám phá một kỹ thuật nào khác trong danh sách lý thuyết ban đầu?"
