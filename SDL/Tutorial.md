# Experiment: Cross-Compiling and Running SDL on Raspberry Pi  
___  

### Objective  
This experiment provides a step-by-step guide on cross-compiling an SDL2 program on a Linux machine (or WSL on Windows) and running it on a Raspberry Pi via SSH.  

___  

## 1. Installing Required Tools on the Host Machine  
First, ensure that your system has all the necessary cross-compilation tools installed:  
```bash
sudo apt update
sudo apt install -y \
    gcc-aarch64-linux-gnu g++-aarch64-linux-gnu \
    pkg-config-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu \
    meson ninja-build \
    libx264-dev libx265-dev \
    libvpx-dev libopus-dev libmp3lame-dev \
    nasm yasm
```  
*If `pkg-config-aarch64-linux-gnu` is not available, use the default `pkg-config` and set the environment variable:*  
```bash
export PKG_CONFIG_PATH="/opt/ffmpeg-arm64/lib/pkgconfig:/opt/x264-arm64/lib/pkgconfig"
```  
---
## 2. Cross-Compiling x264 for AARCH64  
```bash
git clone --depth 1 https://code.videolan.org/videolan/x264.git
```  
```bash
cd x264
./configure \
    --prefix=/opt/x264-arm64 \
    --enable-static \
    --disable-shared \
    --host=aarch64-linux
make -j$(nproc)
sudo make install
```  

```bash
export PKG_CONFIG_PATH=/opt/x264-arm64/lib/pkgconfig
export PKG_CONFIG_SYSROOT_DIR=/
```  

## 3. Cross-Compiling FFmpeg  
```bash
git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
```  
```bash
cd ffmpeg

PKG_CONFIG_PATH="/opt/ffmpeg-arm64/lib/pkgconfig:/opt/x264-arm64/lib/pkgconfig" \
./configure \
    --prefix=/opt/ffmpeg-arm64 \
    --arch=aarch64 \
    --target-os=linux \
    --enable-cross-compile \
    --cross-prefix=aarch64-linux-gnu- \
    --pkg-config=pkg-config \
    --extra-cflags="-I/opt/ffmpeg-arm64/include -I/opt/x264-arm64/include" \
    --extra-ldflags="-L/opt/ffmpeg-arm64/lib -L/opt/x264-arm64/lib" \
    --enable-gpl --enable-libx264 --enable-static --disable-shared \
    --disable-doc --disable-programs

make -j$(nproc)
sudo make install
```  

## 4. Verifying the Libraries  
```bash
ls -lh /opt/ffmpeg-arm64/lib/
ls -lh /opt/x264-arm64/lib/
```  
If you see `libavcodec.a`, `libx264.a`, then you have successfully compiled the libraries.  
![](/image/library.png)  

## 5. Compiling the C++ Program  
Once FFmpeg and x264 are available, you can compile your program:  
```bash
aarch64-linux-gnu-g++ fast_forward_video.cpp -o fast_forward_video \
    -I/opt/ffmpeg-arm64/include -L/opt/ffmpeg-arm64/lib \
    -I/opt/x264-arm64/include -L/opt/x264-arm64/lib \
    -lavformat -lavcodec -lavutil -lswresample -lswscale -lx264 -static -lm -lpthread
```  

## 6. Running the Program on Raspberry Pi 4  
Transfer the `fast_forward_video` binary to the Raspberry Pi:  
```bash
scp fast_forward_video pi@192.168.4.1:/home/pi/
```  
Then, SSH into the Raspberry Pi and run:  
```bash
./fast_forward_video input.mp4 output.mp4
```  

## 7. Playing the Video via SSH  
```bash
cvlc --fullscreen output.mp4
```  