"""
Khai báo các thư viện cần thiết
"""
import sensor, image, lcd, time
import KPU as kpu
import gc, sys
from fpioa_manager import fm
from machine import UART

"""
Khai báo chân TX, RX cho UART
"""
pin_tx = 10
pin_rx = 11

fm.register(pin_tx, fm.fpioa.UART1_TX, force=True)
fm.register(pin_rx, fm.fpioa.UART1_RX, force=True)

"""
Thiết lập UART với baudrate 9600, 8-bit, không parity, 1 stop bit
"""
baudrate = 9600
bits = 8
uart_A = UART(UART.UART1, baudrate, bits, None, 1, timeout=1000, read_buf_len=4096)

time.sleep_ms(100)  # Đợi UART sẵn sàng

"""
Hàm hiển thị lỗi lên LCD
"""
def lcd_show_except(e):
    import uio
    err_str = uio.StringIO()
    sys.print_exception(e, err_str)
    err_str = err_str.getvalue()
    
    # Khai báo kích thước ảnh
    w, h = 320, 240
    
    img = image.Image(size=(w, h))
    img.draw_string(0, 10, err_str, scale=1, color=(0xff, 0x00, 0x00))
    lcd.display(img)

"""
Hàm chính của chương trình
"""
def main(model_addr=0x300000, lcd_rotation=0, sensor_hmirror=False, sensor_vflip=False):
    try:
        sensor.reset()
    except Exception as e:
        raise Exception("Sensor reset fail, please check hardware connection! err: {}".format(e))

    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.set_hmirror(sensor_hmirror)
    sensor.set_vflip(sensor_vflip)
    sensor.run(1)

    # Khởi tạo LCD
    lcd.init(type=1)
    lcd.rotation(lcd_rotation)
    lcd.clear(lcd.WHITE)

    """
    Cấu hình mô hình nhận diện khuôn mặt YOLO2
    """
    anchors = (1.889, 2.5245, 2.9465, 3.94056, 3.99987, 5.3658, 5.155437, 6.92275, 6.718375, 9.01025)
    
    try:
        task = kpu.load(model_addr)
        kpu.init_yolo2(task, 0.5, 0.3, 5, anchors)

        face_count = 0  # Biến đếm số lần phát hiện khuôn mặt
        
        while True:
            img = sensor.snapshot()
            t = time.ticks_ms()
            objects = kpu.run_yolo2(task, img)
            t = time.ticks_ms() - t

            if objects:
                for obj in objects:
                    img.draw_rectangle(obj.rect())

                    # Gửi thông tin nhận diện qua UART
                    uart_A.write(b'Phat hien guong mat sinh vien Nguyen Van A - 20172222 - Khoa K62\n')

                    face_count += 1  # Tăng số lần nhận diện khuôn mặt
                    break  # Chỉ gửi một lần cho mỗi khung hình

            # Hiển thị số lần nhận diện trên LCD
            img.draw_string(0, 200, "Faces detected: %d" % face_count, scale=2, color=(255, 0, 0))
            img.draw_string(0, 220, "Processing time: %dms" % t, scale=2, color=(0, 255, 0))
            lcd.display(img)

    except Exception as e:
        raise e
    finally:
        if task:
            kpu.deinit(task)

"""
Chạy chương trình chính
"""
if __name__ == "__main__":
    try:
        main(model_addr=0x300000, lcd_rotation=0, sensor_hmirror=False, sensor_vflip=False)
    except Exception as e:
        sys.print_exception(e)
        lcd_show_except(e)
    finally:
        gc.collect()

    """
    Kiểm tra UART có dữ liệu đến hay không
    """
    while True:
        if uart_A.any():
            read_data = uart_A.read()
            print("recv =", read_data)  # In dữ liệu nhận được từ UART
            break  # Thoát khỏi vòng lặp sau khi nhận được dữ liệu

        time.sleep_ms(10)  # Đợi xử lý sự kiện khác

    uart_A.deinit()
    del uart_A

