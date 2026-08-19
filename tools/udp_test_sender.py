import socket
import struct
import time
import math
import random

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
angle = 0

print("UDP test verisi gonderiliyor... Durdurmak icin Ctrl+C")

try:
    while True:
        x = random.uniform(50, 250)
        y = random.uniform(50, 250)

        data = struct.pack('ff', x, y)
        s.sendto(data, ('127.0.0.1', 25001))

        angle += 0.1
        time.sleep(1)
except KeyboardInterrupt:
    print("\nDurduruldu.")