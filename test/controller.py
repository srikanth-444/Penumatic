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


class Controller:
    def __init__(self) -> None:
       self.left_adc=[LTC2498_CH0,LTC2498_CH1,LTC2498_CH2,LTC2498_CH3,LTC2498_CH4,LTC2498_CH5,LTC2498_CH6,LTC2498_CH7,LTC2498_CH8,LTC2498_CH9,LTC2498_CH10,LTC2498_CH11,LTC2498_CH12,LTC2498_CH13,LTC2498_CH14,LTC2498_CH15]
       self.pressure_adc=[LTC2498_CH0,LTC2498_CH1,LTC2498_CH2,LTC2498_CH3,LTC2498_CH4,LTC2498_CH5,LTC2498_CH6,LTC2498_CH7,LTC2498_CH8,LTC2498_CH9,LTC2498_CH10,LTC2498_CH11,LTC2498_CH12,LTC2498_CH13,LTC2498_CH14,LTC2498_CH15]
       self.right_adc=[LTC2498_CH0,LTC2498_CH1,LTC2498_CH2,LTC2498_CH3,LTC2498_CH4,LTC2498_CH5,LTC2498_CH6,LTC2498_CH7,LTC2498_CH8,LTC2498_CH9,LTC2498_CH10,LTC2498_CH11,LTC2498_CH12,LTC2498_CH13,LTC2498_CH14,LTC2498_CH15]
       self.i=0
       self.j=0
       self.r=[0]*8
       self.l=[0]*8
    def update(self):
        self.r[self.j-1]=0
        self.r[self.j]=1
        self.l[self.j-1]=0
        self.l[self.j]=1
        self.i=(self.i+1)%12
        self.j=(self.j+1)%8
        return {
            "left_adc":self.left_adc[self.i],
            "right_adc":self.right_adc[self.i],
            "pressure_adc":self.pressure_adc[self.j],
            'r':self.r,
            'l':self.l,
        }