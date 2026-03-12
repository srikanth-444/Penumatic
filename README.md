# _Peumatic Backpack_
This is a Penumatic System that senses and actuates the multiple soft actuators while communicating with the remote pc 

## _Hardware_ 
To control and sense the robotic actuator system, developed using KiCad 8, this repository will build the Gerber files of the system upon pushing the action artifacts specific to JLCPCB 


| Component     | Function            | Notes                                                    |
| ------------- | ------------------- | -------------------------------------------------------- |
| **ESP-32 S3** | Microcontroller     | Handles system control, communication, and overall logic |
| **LTC2498**   | 24-bit ADC          | Senses actuator position with high precision             |
| **LTC6906**   | External oscillator | Provides clock signal for ADC operation                  |
| **MCZ33996**  | Solenoid driver     | Actuates solenoidal valves based on control signals      |



## _Firmware_ 
Developed in C using FreeRTOS for increased concurrency. Code is developed using VSCode ESP-IDF extension


### Images
<p align="center">
  <img src="images/top/top.png" width="400"/>
  <img src="images/top/bottom.png" width="400"/>
</p>
<p align="center">
  <img src="images/bottom/top.png" width="400"/>
  <img src="images/bottom/bottom.png" width="400"/>
</p>
rendered with [kicad-render](https://github.com/linalinn/kicad-render)
