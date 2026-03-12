#ifndef LTC2498_H
#define LTC2498_H
#include "Constant.h"
#include "Serif.h"
#include <stdint.h>


//! In 2X Mode, A non offset binary 0 can be produced. This is corrected in the
//! differential code to voltage functions. To disable this correction, uncomment
//! The following #define.
//#define SKIP_EZDRIVE_2X_ZERO_CHECK

/*! @name Mode Configuration for High Speed Family
 @{
*/

#define LTC2498_KEEP_PREVIOUS_MODE              0x80
#define LTC2498_KEEP_PREVIOUS_SPEED_RESOLUTION  0x00
#define LTC2498_SPEED_1X                        0x00
#define LTC2498_SPEED_2X                        0x08
#define LTC2498_INTERNAL_TEMP                   0xC0
/*!
 @}
*/

// /*! @name Mode Configuration for EasyDrive Family
//  @{
// */
// // Select ADC source - differential input or PTAT circuit
// #define LTC2498_VIN    0b10000000
// #define LTC2498_PTAT   0b11000000

// Select rejection frequency - 50, 55, or 60Hz
#define LTC2498_R50    0b10010000
#define LTC2498_R55    0b10000000
#define LTC2498_R60    0b10100000

// Speed settings is bit 7 in the 2nd byte
#define LTC2498_SLOW   0b10000000 // slow output rate with autozero
#define LTC2498_FAST   0b10001000 // fast output rate with no autozero
/*!
 @}
*/


/*! @name Single-Ended Channel Configuration
@verbatim
Channel selection for all multi-channel, differential input ADCs, even those that only require
8 bits of configuration (no further options.) Most devices in this category require a second
byte of configuration for speed mode, temperature sensor selection, etc., but for the sake
of simplicity a single function will be used to read all devices, sending zeros in the second
configuration byte if only the channel is specified.

Applicable devices:
Easy Drive:
LTC2486, LTC2487, LTC2488, LTC2489, LTC2492, LTC2493,
LTC2494, LTC2495, LTC2496, LTC2497, LTC2498, LTC2499
First Generation Differential:
LTC2414, LTC2418, LTC2439
High Speed:
LTC2442, LTC2444, LTC2445, LTC2448, LTC2449
@endverbatim
@{ */
#define LTC2498_CH0            0xB0
#define LTC2498_CH1            0xB8
#define LTC2498_CH2            0xB1
#define LTC2498_CH3            0xB9
#define LTC2498_CH4            0xB2
#define LTC2498_CH5            0xBA
#define LTC2498_CH6            0xB3
#define LTC2498_CH7            0xBB
#define LTC2498_CH8            0xB4
#define LTC2498_CH9            0xBC
#define LTC2498_CH10           0xB5
#define LTC2498_CH11           0xBD
#define LTC2498_CH12           0xB6
#define LTC2498_CH13           0xBE
#define LTC2498_CH14           0xB7
#define LTC2498_CH15           0xBF
/*! @} */

/*! @name Differential Channel Configuration
@verbatim
See note for single-ended configuration above.

@endverbatim
@{ */
#define LTC2498_P0_N1          0xA0
#define LTC2498_P1_N0          0xA8

#define LTC2498_P2_N3          0xA1
#define LTC2498_P3_N2          0xA9

#define LTC2498_P4_N5          0xA2
#define LTC2498_P5_N4          0xAA

#define LTC2498_P6_N7          0xA3
#define LTC2498_P7_N6          0xAB

#define LTC2498_P8_N9          0xA4
#define LTC2498_P9_N8          0xAC

#define LTC2498_P10_N11        0xA5
#define LTC2498_P11_N10        0xAD

#define LTC2498_P12_N13        0xA6
#define LTC2498_P13_N12        0xAE

#define LTC2498_P14_N15        0xA7
#define LTC2498_P15_N14        0xAF
/*! @} */

/*Commands
Construct a channel / resolution control word by bitwise ORing one choice from the channel configuration
and one choice from the Oversample ratio configuration. You can also enable 2Xmode, which will increase
sample rate by a factor of 2 but introduce one cycle of latency.

Example - read channel 3 single-ended at OSR2048, with 2X mode enabled.
adc_command = (LTC2449_CH3 | LTC2449_OSR_2048) | LTC2449_SPEED_2X;
*/

/*! @name Oversample Ratio (OSR) Commands
@{ */
#define LTC2498_OSR_64         0x10
#define LTC2498_OSR_128        0x20
#define LTC2498_OSR_256        0x30
#define LTC2498_OSR_512        0x40
#define LTC2498_OSR_1024       0x50
#define LTC2498_OSR_2048       0x60
#define LTC2498_OSR_4096       0x70
#define LTC2498_OSR_8192       0x80
#define LTC2498_OSR_16384      0x90
#define LTC2498_OSR_32768      0xF0
/*! @}*/

typedef struct{
    uint8_t command[4];
    uint8_t response[4];
    Serif* serif;
    uint8_t* reading_channel;
}LTC2498;

void init_adc(LTC2498* device,spi_host_device_t bus,uint32_t clk, uint8_t mode,uint8_t cs_pin,uint8_t qsize, char type);
bool LTC2498_read(LTC2498* device);


#endif
