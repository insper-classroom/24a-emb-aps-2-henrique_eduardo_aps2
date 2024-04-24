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
    uinput.KEY_VOLUMEDOWN,
    uinput.KEY_UP,
    uinput.KEY_DOWN,
    uinput.KEY_H,
    uinput.KEY_ENTER
])

button = [uinput.KEY_VOLUMEUP, uinput.KEY_VOLUMEDOWN, uinput.KEY_UP, uinput.KEY_DOWN, uinput.KEY_H, uinput.KEY_L, uinput.KEY_K, uinput.KEY_J, uinput.KEY_S, uinput.KEY_A]

def byte_para_bits(byte):
    # Converte o byte para um valor inteiro
    byte_int = int.from_bytes(byte, byteorder='big')

    # Inicializa uma lista vazia para armazenar os bits
    bits = []

    # Itera sobre cada bit no byte, da posição mais significativa para a menos significativa
    for i in range(15, -1, -1):
        # Verifica se o bit na posição i está definido (1) ou não (0)
        bit = (byte_int >> i) & 1
        # Adiciona o bit à lista de bits
        bits.append(bit)

    return bits

try:
    # sync packagei
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(2)
            bits = byte_para_bits(data)
            i = 5
            while (i < 16):
                if abs((15 - i)) == 10:
                    if bits[i] == 1:
                        device.emit_combo([uinput.KEY_ENTER, uinput.KEY_DOWN, uinput.KEY_A])
                else:
                    o = abs(6 - i)
                    device.emit(button[o], bits[i])
                i += 1

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()