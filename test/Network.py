import socket
import struct
import time
import threading
from flask import Flask, render_template
from flask_socketio import SocketIO
from command import CommandPacket
from controller import Controller
from limb import Limb

RECV_HOST = '0.0.0.0'
RECV_PORT = 8080

SEND_HOST = '0.0.0.0'
SEND_PORT = 5000

LTC2498_CH0= 0xB0
LTC2498_CH1= 0xB8
LTC2498_CH2= 0xB1
LTC2498_CH3= 0xB9
LTC2498_CH4= 0xB2
LTC2498_CH5= 0xBA
LTC2498_CH6= 0xB3
LTC2498_CH7= 0xBB
LTC2498_CH8= 0xB4
LTC2498_CH9= 0xBC
LTC2498_CH10=0xB5
LTC2498_CH11=0xBD
LTC2498_CH12=0xB6
LTC2498_CH13=0xBE
LTC2498_CH14=0xB7
LTC2498_CH15=0xBF
app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")  # allows browser to connect
command = CommandPacket()
controller=Controller()
limbs = [Limb() for _ in range(8)]




# -------------------------------------------------------
# LTC2498 DECODER  (unchanged)
# -------------------------------------------------------
def ltc2498_decode(raw32, vref):
    eoc  = (raw32 >> 31) & 1
    dmy  = (raw32 >> 30) & 1
    sig  = (raw32 >> 29) & 1
    msb  = (raw32 >> 28) & 1

    code24 = (raw32 >> 5) & 0xFFFFFF
    sub_lsb = raw32 & 0x1F

    overrange   = (sig == 1 and msb == 1)
    underrange  = (sig == 0 and msb == 0)

    signed_code = code24 - 0x800000
    v_fs = (signed_code / (2**23)) * (vref / 2)

    return {
        "voltage": v_fs,
        "sub_lsb": sub_lsb,
        "code24": signed_code,
        "overrange": overrange,
        "underrange": underrange,
    }


# -------------------------------------------------------
# RECEIVE THREAD (receives 64-bit raw values at port 8080)
# -------------------------------------------------------
def receive_thread():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((RECV_HOST, RECV_PORT))
        s.listen(1)
        print(f"[RECV] Listening on {RECV_HOST}:{RECV_PORT}")

        conn, addr = s.accept()
        with conn:
            print(f"[RECV] Connected by {addr}")

            while True:
                data = conn.recv(8)
                if not data:
                    print("[RECV] Connection closed")
                    break

                raw_value = struct.unpack('<Q', data)[0]
                firstbyte = (raw_value >> 56) & 0xFF
                type_byte = (raw_value >> 40) & 0xFF
                print(chr(type_byte),end='')
                print(hex(firstbyte))
                last32 = raw_value & 0xFFFFFFFF
                decoded = ltc2498_decode(last32, 2.5)
                if chr(type_byte)=="l":
                        if firstbyte==LTC2498_CH0:
                            limbs[0].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte==LTC2498_CH1:
                            limbs[0].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH2:
                            limbs[0].tactile_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH3:
                            limbs[1].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH4:
                            limbs[1].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH5:
                            limbs[1].tactile_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH6:
                            limbs[2].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH7:
                            limbs[2].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH8:
                            limbs[2].tactile_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH9:
                            limbs[3].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH10:
                            limbs[3].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH11:
                            limbs[3].tactile_resistance.append(decoded['voltage'])
                        else:
                            print(f"not this channels{chr(type_byte)}")
                if chr(type_byte)=="r":
                        if firstbyte== LTC2498_CH0:
                            limbs[4].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH1:
                            limbs[4].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH2:
                            limbs[4].tactile_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH3:
                            limbs[5].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH4:
                            limbs[5].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH5:
                            limbs[5].tactile_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH6:
                            limbs[6].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH7:
                            limbs[6].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH8:
                            limbs[6].tactile_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH9:
                            limbs[7].verticle_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH10:
                            limbs[7].horizontal_resistance.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH11:
                            limbs[7].tactile_resistance.append(decoded['voltage'])
                        else:
                            print(f"not this channels{chr(type_byte)}")
                if chr(type_byte)=="p":
                  
                        if firstbyte== LTC2498_CH0:
                            limbs[0].pressure_reading.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH1:
                            limbs[1].pressure_reading.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH2:
                            limbs[2].pressure_reading.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH3:
                            limbs[3].pressure_reading.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH4:
                            limbs[4].pressure_reading.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH5:
                            limbs[5].pressure_reading.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH6:
                            limbs[6].pressure_reading.append(decoded['voltage'])
                        elif firstbyte== LTC2498_CH7:
                            limbs[7].pressure_reading.append(decoded['voltage'])
                        else:
                            print(f"not this channels{chr(type_byte)}")
                
                # print(f"[RECV] Voltage = {decoded['voltage']:.6f} V   Code = {decoded['code24']}")


# -------------------------------------------------------
# SEND THREAD (sends 64-bit values every 1 sec on port 5000)
# -------------------------------------------------------
def send_thread():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((SEND_HOST, SEND_PORT))
        s.listen(1)
        print(f"[SEND] Listening on {SEND_HOST}:{SEND_PORT}")

        conn, addr = s.accept()
        with conn:
            print(f"[SEND] Connected by {addr}")

            while True:
                
                data,c = command.encode_bytes()
                conn.sendall(data)
                # print(f"[SEND] Sent {c>>56}")
                time.sleep(0.1)


@app.route('/')
def index():
    return render_template('index.html')
def controll_thread():
     # Main control loop
    while True:
        step = controller.update()
        command.right_cmd    = step["right_adc"]
        command.left_cmd     = step["left_adc"]
        command.pressure_cmd = step["pressure_adc"]
        command.r            = step['r']
        command.l            = step['l']

        # Flatten all limb readings
        voltages=[]
        for limb in limbs:
            voltages =voltages+limb.flatten()
        print(voltages)
        # Send live voltages to browser
        # voltages=[]
        socketio.emit('voltage_update', {'voltages': voltages})
        time.sleep(0.1) 
    
# -------------------------------------------------------
# MAIN (start both threads)
# -------------------------------------------------------
if __name__ == "__main__":
    
    t_recv = threading.Thread(target=receive_thread, daemon=True)
    t_send = threading.Thread(target=send_thread, daemon=True)
    

    t_recv.start()
    t_send.start()
    socketio.start_background_task(controll_thread)
    socketio.run(app, host='0.0.0.0', port=8000)
     # 10 Hz update

    

       
