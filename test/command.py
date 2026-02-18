import struct

class CommandPacket:
    def __init__(self):
        # 8-bit values
        self.right_cmd = 0
        self.left_cmd =0
        self.pressure_cmd = 0

        # Actuator bits (1 or 0)
        self.r = [0]*8   # r1..r8
        self.l = [0]*8   # l1..l8

    def encode(self):
        """
        Packs all fields into a 64-bit command integer (same as your C code).
        """

        command = 0

        # ---------------------------------------------------
        # Put differential and pressure into top bytes
        # Same as:
        # differential_sensor.command[0] = (command >> 56)
        # pressure_sensor.command[0]     = (command >> 48)
        # ---------------------------------------------------
        command |= (self.right_cmd & 0xFF) << 56
        command |= (self.left_cmd     & 0xFF) << 48
        command |= (self.pressure_cmd &0xFF)<<40
        for i in range(8):
            bit_value = (self.r[i] & 1)
            shift = 15 - i
            command |= bit_value << shift
        for i in range(8):
            bit_value = (self.l[i] & 1)
            shift = 7 - i
            command |= bit_value << shift

        return command

    def encode_bytes(self):
        """Return packed 8-byte value for TCP sending."""
        return struct.pack('<Q', self.encode()),self.encode()
