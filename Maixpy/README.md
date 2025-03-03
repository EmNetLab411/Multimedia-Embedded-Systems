# **Cài đặt mô hình AI find_face trên vi điều khiển K210 (Sipeed Maix)**
___
### **Bước 1: Chuẩn bị phần cứng**
Bạn cần có:
- Bo mạch Sipeed Maix K210
- Camera
- Màn hinh LCD
___
### **Bước 2: Chuẩn bị phần mềm**
1. Cài đặt Cross platform GUI wrapper for kflash.py (download(/burn) tool for k210)
    - Kho lưu trữ này sử dụng các mô-đun con. --recursiveNên sử dụng để tải xuống các mô-đun con cần thiết.
        ```
        git clone  --recursive https://github.com/sipeed/kflash_gui.git
        cd kflash_gui
        ```
    - kflash_gui có thể được chạy từ tệp nguồn như sau:
    `python3 kflash_gui.py`
2. Cài đặt MaixPy IDE để lập trình cho Maix Dock kit
    - Tải về packet maixpy-ide-linux-x86_64-0.2.5.run từ kho mã nguồn được cung cấp.
    - Chạy lệnh `chmod +x maixpy-ide-linux-x86_64-0.2.5.run` để cấp quyền thực thi cho tệp.
    - Chạy tệp `./maixpy-ide-linux-x86_64-0.2.5.run`
___
### **Bước 3: Cài đặt firmware  và mô hình AI**
1. Cài đặt firmware Maix K210
- Tải về firmware Maix K210 `maixpy_v0.3.2_full.bin` từ kho lưu trữ được cung cấp.
- Tải về mô hình AI nhận diện khuôn mặt `face_model_at_0x300000.kfpkg` từ kho lưu trữ được cung cấp.
- Chạy lệnh `kflash_gui.py` để mở phần mềm nạp firmware cho bo K210 từ tập tin `kflash_gui` vừa tải bên trên.
- Cấu hình `kflash` như hình và nạp
![](/image/kflash.png)
___
### **Bước 4: Lập trình và chạy ứng dụng nhận diện khuôn mặt**
1. Mở IDE MaixPy và tạo một dự án mới
2. Tải về thư viện nhận diện khuôn mặt `find_face.py` từ kho lưu trữ.
3. Kết nối với boad.
4. Chạy ứng dụng nhận diện khuôn mặt.
___
