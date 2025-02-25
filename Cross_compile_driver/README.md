# Cross-Compile  
## How to Cross-Compile a Driver to Control the PCA9685 (an I2C PWM Driver) for an Embedded System  

### 1. Install Necessary Tools  
To cross-compile a driver for PCA9685, you need to set up the following tools:  
The cross-compiler (`arm-linux-gnueabihf-gcc`) and supporting packages:  
```bash
sudo apt update
sudo apt install -y git bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install -y crossbuild-essential-armhf
```  
If `crossbuild-essential-armhf` cannot be installed, you can install the `gcc-arm-linux-gnueabihf` package separately:  
```bash
sudo apt install -y gcc-arm-linux-gnueabihf
```  

### 2. Download the Linux Source Code  
You need to download the appropriate kernel source for Raspberry Pi 4 (BCM2711). Raspberry Pi uses a customized branch of the Linux kernel, which can be obtained from the Raspberry Pi GitHub repository:  
```bash
mkdir Embedded_System
cd Embedded_System
git clone --branch rpi-5.4.y --depth=1 https://github.com/raspberrypi/linux.git
```  

### 3. Configure the Kernel  
Open a terminal (Ctrl + Alt + T) and enter the following commands to configure the Linux kernel:  
```bash
cd linux
KERNEL=kernel7l
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
```  
![Step 1](/image/step_3_1.png)  
```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- prepare
```  
![Step 2](/image/Step_3_2.png)  
```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_prepare
```  
![Step 3](/image/step_3_3.png)  

*Building the kernel may take some time.*  

### 4. Create the Driver Directory  
Create a `Makefile` and a source file (e.g., `device_driver.c`). They must be in the same directory to compile successfully.  
```bash
cd ..
mkdir BTN1   # Create the BTN1 directory
cd BTN1
sudo gedit Makefile
```  
Copy the following code into the editor and save it:  
```make
obj-m += device_driver.o 

KDIR = /lib/modules/$(shell uname -r)/build 

MYY_KERNEL_DIR = "/home/loc/Embedded_System/linux"

all: 
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(shell pwd) -C $(MYY_KERNEL_DIR) modules 

clean: 
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(shell pwd) -C $(MYY_KERNEL_DIR) clean
```  

- Download the reference folder from GitHub:  
  ```bash
  git clone https://github.com/EmNetLab411/Multimedia-Embedded-Systems.git
  ```  
- Copy `device_driver.c` into the `BTN1` directory.  
- Compile the program using:  
  ```bash
  sudo make
  ```  
  or  
  ```bash
  make
  ```  
![Step 4](/image/step_4.png)  

### 5. Install the Kernel Module on the Target Device  
- Once the compilation is successful, open a new terminal window, connect to the Wi-Fi network named `pix` (where `x` is a number from 1 to 9, depending on your Raspberry Pi).  
- Use the following command to SSH into the Raspberry Pi from your virtual machine:  
  ```bash
  sudo ssh studentx@192.168.4.1
  ```  
  (Replace `x` with your assigned number from the student list).  
- When prompted with `yes/no`, type `yes` and press Enter, then enter the password `1` to complete the connection.  
![Step 5](/image/ssh.png)  

- Open a new terminal window and transfer the `BTN1` folder to the Raspberry Pi using:  
  ```bash
  sudo scp -r /home/your/Embedded_System/BTN1 studentx@192.168.4.1:/home/studentx
  ```  
![Step 6](/image/scp.png)  

- In the `studentx@raspberrypi` terminal window, install the kernel module:  
  ```bash
  sudo insmod device_driver.ko
  ```  
  If you encounter the error `could not insert module device_driver.ko: File exists`, it means a module with the same name is already installed. Remove it using:  
  ```bash
  sudo rmmod device_driver
  ```  
  Then reinstall it:  
  ```bash
  sudo insmod device_driver.ko
  ```  
  *Note: `device_driver` is the filename and can be changed as needed.*  

- Verify the module installation using:  
  ```bash
  dmesg
  ```  
![Step 7](/image/dmesg.png)