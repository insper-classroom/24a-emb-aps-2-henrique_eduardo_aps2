import serial
import uinput
import time
ser = serial.Serial('/dev/ttyACM0', 115200)

device = uinput.Device([
    uinput.KEY_A
])


try:
    # sync package
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(1)
            #breakpoint()
            if data.decode('ascii') == 'A':
                print("a chegou")
                device.emit_click(uinput.KEY_A)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()
