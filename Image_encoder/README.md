
# Biên dịch chéo và mã hóa ảnh JPEG trên Rpi 4 model B

## 1. Cài đặt công cụ biên dịch chéo trên máy tính
Cài đặt công cụ biên dịch chéo Aarch64 trên ubuntu
```
sudo apt update
sudo apt install -y gcc-aarch64-linux-gnu make cmake
```
## 2. Tải và biên dịch *libjpeg-turbo* cho Raspberry Pi
`jpegoptim`cần `libjpeg-turbo` để hoạt động. Chúng ta sẽ biên dịch `libjpeg-turbo` trước.

### 2.1. Tải mã nguồn libjpeg-turbo
```
cd ~
git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git
cd libjpeg-turbo
```
### 2.2. Các gói phụ thuộc
Cài các gói phụ thuộc
```
cd 
sudo apt update
sudo apt install -y cmake nasm yasm autoconf automake libtool pkg-config
```
### 2.3. Biên dịch libjpeg-turbo

```
mkdir build && cd build
cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
sudo make install
```
Sau khi biên dịch xong, `libjpeg-turbo` sẽ được cài vào thư mục `/home/yourname/libjpeg-turbo-aarch64`.

## 3. Tải và biên dịch jpegoptim
### 3.1. Tải mã nguồn jpegoptim
```cd ~
git clone https://github.com/tjko/jpegoptim.git
cd jpegoptim
```
### 3.2. Cấu hình và biên dịch
```
make clean
make -j$(nproc) CC=aarch64-linux-gnu-gcc CFLAGS="-I/home/yourname/libjpeg-turbo-aarch64/include" LDFLAGS="-L/home/yourname/libjpeg-turbo-aarch64/lib" LIBS="-lm -ljpeg"
```
Sau khi biên dịch xong, kiểm tra tệp `jpegoptim`:
```
ls -l jpegoptim
```
## 4. Đóng gói và chuyển sang Raspberry Pi
### 4.1. Tạo tệp nén 
```
tar czf jpegoptim-arm64.tar.gz jpegoptim
```
### 4.2 Chuyển sang Raspberry Pi 
```
scp jpegoptim-arm64.tar.gz pi@192.168.4.1:/home/pi/
```
### 4.3. Giải nén trên Raspberry Pi
```
cd /home/pi
tar xzf jpegoptim-arm64.tar.gz
chmod +x jpegoptim
sudo mv jpegoptim /usr/local/bin/
```
## 5. Chạy jpegoptim trên Raspberry Pi
Giảm chất lượng ảnh xuống tối đa 80%:
```
jpegoptim --max=80 test.jpg
```
