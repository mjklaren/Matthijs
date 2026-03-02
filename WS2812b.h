/*
*/
 
//#include "pico/stdlib.h"
#include "hardware/pio.h"
//#include "hardware/clocks.h"
//#include "time.h"
#include "ws2812.pio.h"


#ifndef WS2812B_H
#define WS2812B_H

// Available commands for processing, last 8 bits in the Pixelbuffer
// Pixel on and blink indicator - 2 bits
#define WS2812B_PIXEL_ON        0x80000000
#define WS2812B_BLINK           0x40000000

// Reserved - 2 bits

// Special effects - 2 bits
#define WS2812B_FADE_IN         0xC000000
#define WS2812B_FADE_OUT        0x8000000
#define WS2812B_RANDOMIZE       0x4000000
#define WS2812B_NONE            0x0000000  // Default

// Speed of the special effect
#define WS2812B_SPEED_STEPS     4
#define WS2812B_SPEED4          0x3000000  // Slowest
#define WS2812B_SPEED3          0x2000000
#define WS2812B_SPEED2          0x1000000
#define WS2812B_SPEED1          0x0000000  // Default; fastest

// Available standard colors - 24 bits
#define WS2812B_BLACK           0x000000
#define WS2812B_GREY            0x808080
#define WS2812B_RED             0xFF0000
#define WS2812B_YELLOW          0xFFFF00
#define WS2812B_MAGENTA         0xFF00FF
#define WS2812B_PURPLE          0x800080
#define WS2812B_CYAN            0x00FFFF
#define WS2812B_GREEN           0x00FF00
#define WS2812B_LIGHTGREEN      0x99CC00
#define WS2812B_ORANGE          0xFF9900
#define WS2812B_BLUE            0x0000FF
#define WS2812B_WHITE           0xFFFFFF
#define WS2812B_STANDARD_COLORS 11        // Number of standard colors available (not including black).
static const uint32_t WS2812bStandardColors[]={WS2812B_WHITE,WS2812B_BLUE,WS2812B_ORANGE,WS2812B_LIGHTGREEN,WS2812B_GREEN,WS2812B_CYAN,WS2812B_PURPLE,WS2812B_MAGENTA,WS2812B_YELLOW,WS2812B_RED,WS2812B_GREY};

// Directions for shifting
#define WS2812B_SHIFT_LEFT      0
#define WS2812B_SHIFT_RIGHT     1

// General definitions
#define WS2812B_IS_RGBW         false      // Is not supported, always false.
#define WS2812_DELAY_US         300        // Delay after writing buffer, needed for the device to set the colors, in usec.

// Struct for the WS2812b instance
typedef struct 
{
  /*
    Format of the PixelBuffer (MSB to LSB):
    bit 31    - New pixel indicator
    bit 30-24 - Commands for processing
    bit 23-16 - Color code RED
    bit 15-8  - Color code GREEN
    bit 7-0   - Color code BLUE
  */
  uint32_t *PixelBuffer;
  uint16_t NumberOfLeds, MaxNumberOfActiveLeds, NumberOfActiveLeds;       // Max. 65535 LEDs
  uint8_t GPIO, MaxBrightness, DimStep, CycleNumber;
  PIO PIO;
} WS2812bInstance;


/*
  
  Functions for writing pixels (LEDs) of the WS2812.

*/
// Write a single pixel to the WS2812 device using an RGB color code.
static inline void put_pixel(uint32_t pixel_grb);

// Write a single pixel to the WS2812 device using three color codes.
static inline void write_pixel(uint8_t Red, uint8_t Green, uint8_t Blue);

// Write the contents of the buffer to the WS2812 device.
uint16_t WS2812bWriteBuffer(WS2812bInstance *MyWS2812b);


/*
  
  Functions for manipulating the pixel buffer, before it's contents is sent to the WS2812 device.

*/

// Run through the buffer and chech which pixels need to be manipulated; determined by the specified command bits.
uint16_t WS2812bProcessBuffer(WS2812bInstance *MyWS2812b);

// Shift the contents of the buffer to left or right; shift a pixel with the specified color and options 'in'.
void WS2812bShiftBuffer(WS2812bInstance *MyWS2812b, uint8_t Direction, uint32_t ShiftInColor, uint32_t ShiftInOptions);

// Empty the buffer (set all values to 0).
void WS2812bEmptyBuffer(WS2812bInstance *MyWS2812b);

// Completely fill the buffer with the specified color code.
void WS2812bFloodfillBuffer(WS2812bInstance *MyWS2812b, uint32_t Color, uint32_t Options);

// Reduce the brightness of all pixels in the buffer by recalculating their values.
uint16_t WS2812bDimBuffer(WS2812bInstance *MyWS2812b);

// Fill the buffer with random colored pixels (limited by the max. number of active LEDs).
void WS2812bRandomizeBuffer(WS2812bInstance *MyWS2812b);

// Set a single pixel of the specified color and options in the buffer.
uint8_t WS2812bSetPixelInBuffer(WS2812bInstance *MyWS2812b, uint16_t Position, uint32_t Color, uint32_t Options);

// Set a random, single pixel in the buffer. Color is random; options need to be specified.
uint8_t WS2812bSetRandomPixelInBuffer(WS2812bInstance *MyWS2812b, uint32_t Options);

// Fill the buffer with random standard colors limited by the max. number of active LEDs.
uint8_t WS2812bSetRandomStandardColorPixelInBuffer(WS2812bInstance *MyWS2812b, uint32_t Options);

// Initialize a new instance of the WS2812 struct.
WS2812bInstance *WS2812bInitialize(PIO PIO,uint8_t GPIO,uint16_t NumberOfLeds,uint16_t MaxNumberOfActiveLeds,uint8_t MaxBrightness,uint8_t DimStep);


#endif /* WS2812B_H */
