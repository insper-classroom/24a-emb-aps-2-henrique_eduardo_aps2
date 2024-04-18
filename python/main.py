import serial
import uinput
import time
ser = serial.Serial('/dev/rfcomm0', 115200)

device = uinput.Device([
    uinput.KEY_A,
    uinput.KEY_K,
    uinput.KEY_L,
    uinput.KEY_S,
    uinput.KEY_J,
    uinput.KEY_VOLUMEUP,
    uinput.KEY_VOLUMEDOWN
])


try:
    # sync package
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(2)
            print(ascii(data))
            button = data[0]
            print(chr(button))
            up_down = data[1]
            print(up_down)
            if chr(button) == 'A':
                print("a chegou")
                #device.emit_click(uinput.KEY_A)
                device.emit(uinput.KEY_A, up_down)
            if chr(button) == 'K':
                print("K chegou")
                #device.emit_click(uinput.KEY_K)
                device.emit(uinput.KEY_K, up_down)
            if chr(button) == 'J':
                print("J chegou")
                #device.emit_click(uinput.KEY_J)
                device.emit(uinput.KEY_J, up_down)
            if chr(button) == 'L':
                print("L chegou")
                #device.emit_click(uinput.KEY_L)
                device.emit(uinput.KEY_L, up_down)
            if chr(button) == 'S':
                print("S chegou")
                #device.emit_click(uinput.KEY_S)
                device.emit(uinput.KEY_S, up_down)
            if chr(button) == 'M':
                print("M chegou")
                #device.emit_click(uinput.KEY_S)
                if (up_down == 255):
                    device.emit_click(uinput.KEY_VOLUMEDOWN)
                if (up_down == 1):
                    device.emit_click(uinput.KEY_VOLUMEUP)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()