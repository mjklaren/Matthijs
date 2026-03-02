/*

*/

#include "WS2812b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "time.h"
#include "ws2812.pio.h"


/*
  
  Functions for writing pixels (LEDs) of the WS2812.

*/
// Write a single pixel to the WS2812 device using an RGB color code.
static inline void put_pixel(uint32_t pixel_grb)
{
  pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}


// Write a single pixel to the WS2812 device using three color codes.
static inline void write_pixel(uint8_t Red, uint8_t Green, uint8_t Blue)
{
  pio_sm_put_blocking(pio0, 0, (((uint32_t)(Red)<<16) | ((uint32_t)(Green)<<8) | (uint32_t)(Blue)) << 8u);
}


// Write the contents of the buffer to the WS2812 device.
uint16_t WS2812bWriteBuffer(WS2812bInstance *MyWS2812b)
{
  MyWS2812b->NumberOfActiveLeds=0;
  for(uint16_t counter=0; counter<MyWS2812b->NumberOfLeds; counter++)
  {
    pio_sm_put_blocking(MyWS2812b->PIO, 0, MyWS2812b->PixelBuffer[counter] << 8u);
    if((MyWS2812b->PixelBuffer[counter]&0xFFFFFF)>0)
      MyWS2812b->NumberOfActiveLeds++;
  }
  busy_wait_us(WS2812_DELAY_US);      // Delay needed for the WS2812b to display the LEDs
  return(MyWS2812b->NumberOfActiveLeds);
}


/*
  
  Functions for manipulating the pixel buffer, before it's contents is sent to the WS2812 device.

*/

// Run through the buffer and chech which pixels need to be manipulated; determined by the specified command bits.
uint16_t WS2812bProcessBuffer(WS2812bInstance *MyWS2812b)
{
  uint32_t Red,Green,Blue,counter;

  // Run through the PixelBuffer and check which actions need to be taken.
  for(counter=0;counter<MyWS2812b->NumberOfLeds;counter++)
  {
    // Is this pixel active?
    if(MyWS2812b->PixelBuffer[counter]>0)
    {
      // Do we need to process this pixel? Check the speed-value against the CycleNumber.
      if(((MyWS2812b->PixelBuffer[counter] & WS2812B_SPEED4)>>24)<MyWS2812b->CycleNumber)
      {
        // Check which command bits are set.
        // Fade out effect.
        if(MyWS2812b->PixelBuffer[counter] & WS2812B_FADE_OUT)
        {  
          if((MyWS2812b->PixelBuffer[counter] & 0xFF)<MyWS2812b->DimStep)
            Red=0;
          else
            Red=(MyWS2812b->PixelBuffer[counter] & 0xFF)-MyWS2812b->DimStep;
          if(((MyWS2812b->PixelBuffer[counter] & 0xFF00)>>8)<MyWS2812b->DimStep)
            Green=0;
          else
            Green=(MyWS2812b->PixelBuffer[counter] & 0xFF00)-(MyWS2812b->DimStep<<8);
          if(((MyWS2812b->PixelBuffer[counter] & 0xFF0000)>>16)<MyWS2812b->DimStep)
            Blue=0;
          else
            Blue=(MyWS2812b->PixelBuffer[counter] & 0xFF0000)-(MyWS2812b->DimStep<<16);
          MyWS2812b->PixelBuffer[counter]=(MyWS2812b->PixelBuffer[counter] & 0xFF000000) | Red | Green | Blue;
       }

        // Randomize effect.
        if(MyWS2812b->PixelBuffer[counter] & WS2812B_RANDOMIZE)
          MyWS2812b->PixelBuffer[counter]=(MyWS2812b->PixelBuffer[counter] & 0xFF000000) | (rand()%(MyWS2812b->MaxBrightness+1) << 16) | (rand()%(MyWS2812b->MaxBrightness+1) << 8) | rand()%(MyWS2812b->MaxBrightness+1);
      }
    }
  }

  // Count the number of active leds
  MyWS2812b->NumberOfActiveLeds=0;
  for(counter=0; counter<MyWS2812b->NumberOfLeds; counter++)
  {
    if(MyWS2812b->PixelBuffer[counter]>0)
      MyWS2812b->NumberOfActiveLeds++;
  }

  // Set the CycleCounter to the next level of speed.
  if(MyWS2812b->CycleNumber<WS2812B_SPEED_STEPS)
     MyWS2812b->CycleNumber++;
  else
     MyWS2812b->CycleNumber=1;

  // All done.
  return(MyWS2812b->NumberOfActiveLeds);
}


// Shift the contents of the buffer to left or right; shift a pixel with the specified color and options 'in'.
void WS2812bShiftBuffer(WS2812bInstance *MyWS2812b, uint8_t Direction, uint32_t ShiftInColor, uint32_t ShiftInOptions)
{
  uint16_t counter;
  if(Direction==WS2812B_SHIFT_LEFT)
  {
    for(counter=1; counter<MyWS2812b->NumberOfLeds; counter++)
      MyWS2812b->PixelBuffer[counter-1]=MyWS2812b->PixelBuffer[counter];
    MyWS2812b->PixelBuffer[MyWS2812b->NumberOfLeds-1]=ShiftInColor | ShiftInOptions;
  }
  else
  {
    for(counter=MyWS2812b->NumberOfLeds; counter>0; counter--)
      MyWS2812b->PixelBuffer[counter]=MyWS2812b->PixelBuffer[counter-1];
    MyWS2812b->PixelBuffer[0]=ShiftInColor | ShiftInOptions;
  }
  
  // Count the number of active leds
  MyWS2812b->NumberOfActiveLeds=0;
  for(counter=0; counter<MyWS2812b->NumberOfLeds; counter++)
  {
    if(MyWS2812b->PixelBuffer[counter]>0)
      MyWS2812b->NumberOfActiveLeds++;
  }
}


// Empty the buffer (set all values to 0).
void WS2812bEmptyBuffer(WS2812bInstance *MyWS2812b)
{
  memset(MyWS2812b->PixelBuffer,0,MyWS2812b->NumberOfLeds*sizeof(uint32_t));
}


// Completely fill the buffer with the specified color code.
void WS2812bFloodfillBuffer(WS2812bInstance *MyWS2812b, uint32_t Color, uint32_t Options)
{
  if(MyWS2812b->NumberOfLeds==0)
    return;

  MyWS2812b->PixelBuffer[0]=Color|Options;
  for (uint16_t counter=1;counter<MyWS2812b->NumberOfLeds;counter*=2)
    memcpy(&MyWS2812b->PixelBuffer[counter],MyWS2812b->PixelBuffer,((counter<MyWS2812b->NumberOfLeds-counter)?counter:MyWS2812b->NumberOfLeds-counter)*sizeof(uint32_t));
}


// Reduce the brightness of all pixels in the buffer by recalculating their values.
uint16_t WS2812bDimBuffer(WS2812bInstance *MyWS2812b)
{
  uint16_t Red, Green, Blue, counter;
  MyWS2812b->NumberOfActiveLeds=0;
  for(counter=0; counter<MyWS2812b->NumberOfLeds; counter++)
  {
    if(MyWS2812b->PixelBuffer[counter]>0)
    {
      Red=((MyWS2812b->PixelBuffer[counter]&0xFF)<=MyWS2812b->DimStep?0:(MyWS2812b->PixelBuffer[counter]&0xFF)-MyWS2812b->DimStep);
      Green=((MyWS2812b->PixelBuffer[counter]&0xFF00)<=(MyWS2812b->DimStep<<8)?0:(MyWS2812b->PixelBuffer[counter]&0xFF00)-(MyWS2812b->DimStep<<8));
      Blue=((MyWS2812b->PixelBuffer[counter]&0xFF0000)<=(MyWS2812b->DimStep<<16)?0:(MyWS2812b->PixelBuffer[counter]&0xFF0000)-(MyWS2812b->DimStep<<16));
      MyWS2812b->PixelBuffer[counter]=(MyWS2812b->PixelBuffer[counter]&0xFF000000) | Red | Green | Blue;
     // if((MyWS2812b->PixelBuffer[counter]&0xFFFFFF)>0)
        MyWS2812b->NumberOfActiveLeds++;
    }
  }
  return(MyWS2812b->NumberOfActiveLeds);
}


// Fill the buffer with random colored pixels (limited by the max. number of active LEDs).
void WS2812bRandomizeBuffer(WS2812bInstance *MyWS2812b)
{
  for(uint16_t counter=0; counter<MyWS2812b->MaxNumberOfActiveLeds; counter++)
    MyWS2812b->PixelBuffer[counter]=(rand()%(MyWS2812b->MaxBrightness+1) << 16) | (rand()%(MyWS2812b->MaxBrightness+1) << 8) | rand()%(MyWS2812b->MaxBrightness+1);
}


// Set a single pixel of the specified color and options in the buffer.
uint8_t WS2812bSetPixelInBuffer(WS2812bInstance *MyWS2812b, uint16_t Position, uint32_t Color, uint32_t Options)
{
  if(Position<MyWS2812b->NumberOfLeds && MyWS2812b->NumberOfActiveLeds<MyWS2812b->MaxNumberOfActiveLeds)
  {
    MyWS2812b->PixelBuffer[Position]=Color | Options;
    return(0);
  }
  return(1);
}


// Set a random, single pixel in the buffer. Color is random; options need to be specified.
uint8_t WS2812bSetRandomPixelInBuffer(WS2812bInstance *MyWS2812b, uint32_t Options)
{
  if(MyWS2812b->NumberOfActiveLeds<=MyWS2812b->MaxNumberOfActiveLeds)
  {
    uint8_t RandomPosition=rand()%(MyWS2812b->NumberOfLeds+1);
    MyWS2812b->PixelBuffer[RandomPosition]=(rand()%(MyWS2812b->MaxBrightness+1) << 16) | (rand()%(MyWS2812b->MaxBrightness+1) << 8) | rand()%(MyWS2812b->MaxBrightness+1) | Options;
    MyWS2812b->NumberOfActiveLeds++;
    return(0);
  }
  return(1);
}


// Fill the buffer with random standard colors limited by the max. number of active LEDs.
// Watch out; max brightness is ignored here!
uint8_t WS2812bSetRandomStandardColorPixelInBuffer(WS2812bInstance *MyWS2812b, uint32_t Options)
{
  if(MyWS2812b->NumberOfActiveLeds>=MyWS2812b->MaxNumberOfActiveLeds)
    return(1);

  uint32_t StandardColor=WS2812bStandardColors[rand()%WS2812B_STANDARD_COLORS];
    
  // Find an empty space in the buffer to place the pixel, so we don't overwrite.
  uint8_t RandomPosition;
  do
  {
    RandomPosition=rand()%(MyWS2812b->NumberOfLeds+1);
  }
  while ((MyWS2812b->PixelBuffer[RandomPosition]&0xFFFFFF)!=0);
  MyWS2812b->PixelBuffer[RandomPosition]=StandardColor|Options;
  MyWS2812b->NumberOfActiveLeds++;
  return(0);
}


// Initialize a new instance of the WS2812 struct.
WS2812bInstance *WS2812bInitialize(PIO PIO,uint8_t GPIO,uint16_t NumberOfLeds,uint16_t MaxNumberOfActiveLeds,uint8_t MaxBrightness,uint8_t DimStep)
{
  WS2812bInstance *Instance=malloc(sizeof(WS2812bInstance));
  if(Instance!=NULL)
  {
    Instance->PixelBuffer=malloc(NumberOfLeds*sizeof(uint32_t));
    if(Instance->PixelBuffer!=NULL)
    {
      Instance->NumberOfLeds=NumberOfLeds;
      Instance->MaxNumberOfActiveLeds=MaxNumberOfActiveLeds;
      Instance->NumberOfActiveLeds=0;
      Instance->PIO=PIO;
      Instance->GPIO=GPIO;
      Instance->MaxBrightness=MaxBrightness;
      Instance->DimStep=DimStep;
      Instance->CycleNumber=1;
    }
    else
    {
      // Init of the buffer failed.
      free(Instance);
      return(NULL);
    }
  }
  return(Instance);
}
