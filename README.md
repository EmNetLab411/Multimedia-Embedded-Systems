# Cross-Compile
## How to cross-compile a driver to control the PCA9685 (an I2C PWM driver) for an embedded system

### 1. Cài đặt công cụ cần thiết
Để cross-compile một driver cho PCA9685, bạn cần chuẩn bị những công cụ sau:
(arm-linux-gnueabihf-gcc) và các gói hỗ trợ:
        - `sudo apt update`
        - `sudo apt install -y git bc bison flex libssl-dev make libc6-dev libncurses5-dev`
        - `sudo apt install -y crossbuild-essential-armhf`
Nếu crossbuild-essential-armhf không cài được, bạn có thể cài gói gcc-arm-linux-gnueabihf riêng:
        - `sudo apt install -y gcc-arm-linux-gnueabihf`
### 2. Tải mã nguồn Linux
Bạn cần tải kernel source phù hợp với Raspberry Pi 4 (BCM2711). Raspberry Pi sử dụng một nhánh tùy chỉnh của kernel Linux, có thể lấy từ kho GitHub của Raspberry Pi:
- `mkdir Embedded_System`
- `cd Embedded_System`
- `git clone --branch rpi-5.4.y --depth=1 https://github.com/raspberrypi/linux.git`

### 3. Cấu hình kernel 
Bật terminal (Ctrl + Alt + T) và nhập các câu lệnh sau để config Linux kernel:
- `cd linux`
- `KERNEL=kernel7l`
- `make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig`
    ![Step 1](/image/step_3_1.png)
- `make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- prepare`
    ![Step 2](/image/Step_3_2.png)
- `make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_prepare`
    ![Step 3](/image/step_3_3.png)

*Việc build kernel sẽ mất 1 ít thời gian*

### 4. Tạo thư mục driver
Tạo 1 Makefile và 1 file name.c trên máy (Vd: device_driver.c), chúng bắt buộc phải chứa cùng thư mục thì mới có thể biên dịch thành công.
- `cd ..`
- `mkdir BTN1` tạo thư mục BTN1
- `cd BTN1`
- `sudo gedit Makefile` để tạo Makefile
Copy đoạn code này vào trình soạn thảo
```obj-m += device_driver.o 

KDIR = /lib/modules/$(shell uname -r)/build 

MYY_KERNEL_DIR = "/home/loc/Embedded_System/linux"

all: 
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(shell pwd) -C $(MYY_KERNEL_DIR) modules 

clean: 
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(shell pwd) -C $(MYY_KERNEL_DIR) clean
```
- Tải folder tham khảo từ github theo link: `https://github.com/EmNetLab411/Multimedia-Embedded-Systems.git`.
Copy file `device_driver.c` vào thư mục `BTN1`.
- `sudo make` hoặc `make` để biên dịch chương trình.
![Step 4](/image/step_4.png)
### 5. Cài đặt Module Kernel trên Thiết Bị Đích
- Sau khi biên dịch thành công, bật 1 cửa sổ  terminal mới, kết nối wifi có  tên là `pix` (x ở đây là số từ 1  đến 9, tùy theo con Pi của bạn) với mật khẩu `12345678`, nhập câu lệnh `sudo ssh studentx@192.168.4.1` (x là số  thứ tự của bạn trong danh sách sinh viên) để điều khiển Raspberry Pi4 từ máy ảo của chúng ta, terminal sẽ hiện lên câu hỏi `yes/no`, nhập `yes` và ấn Enter để điều khiển, sau đó nhập mật khẩu là 1 để hoàn tất quá trình.
![Step 5](/image/ssh.png)
- Cấp quyền 
- Tạo 1 cửa sổ terminal mới, nhập lệnh `sudo scp -r /home/your/Embedded_System/BTN1 studentx@192.168.4.1:/home/studentx`để copy thư mục BTN1 từ máy ảo sang Raspberry Pi4
![Step 6](/image/scp.png)
- Trong cửa sổ terminnal `studentx@raspberrypi` nhập lệnh `sudo insmod device_driver.ko` để cài đặt chúng vào kernel của Raspberry Pi4. Sau khi nhập câu lệnh mà thấy báo lỗi: `could not insert module device_driver.ko: File exists`, nghĩa là đã có module giống như vậy cài đặt vào kernel, hãy sử dụng câu lệnh `sudo rmmod device_driver` để gỡ nó ra, và nhập lại `sudo insmod device_driver.ko` để cài đặt chúng vào kernel của Raspberry 

*Lưu ý: device_driver là tên file tùy theo người đặt*
- Nhập câu lệnh `dmesg` để kiểm tra module đã được cài đặt vào kernel hay chưa 

![Step 7](/image/dmesg.png)

---


