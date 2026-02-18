class Limb:
    def __init__(self) -> None:
        self.pressure_reading=[]
        self.verticle_resistance=[]
        self.horizontal_resistance=[]
        self.tactile_resistance=[]
    def flatten(self):
        self.pressure_reading=self.pressure_reading[-10:]
        self.horizontal_resistance=self.horizontal_resistance[-10:]
        self.tactile_resistance=self.tactile_resistance[-10:]
        self.verticle_resistance=self.verticle_resistance[-10:]
        return (
            [self.pressure_reading,
            self.verticle_resistance,
            self.horizontal_resistance,
            self.tactile_resistance]
        )