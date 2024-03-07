import serial
from Crypto.Cipher import AES
import struct

ser = serial.Serial('COM3', 115200, 8, "N", 1)
print(f'Using {ser.name} serial port')

with open("aes.key") as key_file:
    aes_key = key_file.read()
    aes_key = bytes(aes_key, 'ascii')

cipher = AES.new(aes_key, AES.MODE_ECB)

try:
    while True:
        if(ser.in_waiting >= 16):
            read_bytes = ser.read(16)
            decrypted_bytes = cipher.decrypt(read_bytes)
            double_value = struct.unpack('d', decrypted_bytes[0:8])[0]
            print(f'distance: {double_value} cm')
except KeyboardInterrupt:
    pass