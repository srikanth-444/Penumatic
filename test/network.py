# adc_client.py
import socket
import struct

HOST = '0.0.0.0'
PORT = 8080

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        
        # Read exactly 8 bytes (64-bit)
        while True:
            data = conn.recv(8)
            if not data:
                break
            value = struct.unpack('>Q', data)[0]
            print(f"Received 64-bit value: {hex(value)}")