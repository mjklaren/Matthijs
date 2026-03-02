/*
*/

#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define DYSV17F_DELAY      250     // Delay in ms after sending command
#define DYSV17F_MIN_VOLUME 0       // Minimum volume
#define DYSV17F_MAX_VOLUME 30      // Maximum volume


// Device types
typedef enum
{
  DEVICE_USB = 0x00,
  DEVICE_SD = 0x01,
  DEVICE_FLASH = 0x02
} DYSV17F_device_t;


// Calculate checksum
uint8_t DYSV17FCalculateChecksum(uint8_t* buffer, uint8_t len)
{
  uint8_t sum = 0;
  for (int i = 0; i < len; i++)
    sum += buffer[i];
  return sum;
}


// Send command to DY-SV17F
void DYSV17FSendCommand(uart_inst_t* UartID, uint8_t cmd, uint8_t* data, uint8_t data_len)
{
  uint8_t buffer[32];
  uint8_t idx=0;
  buffer[idx++]=0xAA;           // Start byte
  buffer[idx++]=cmd;            // Command
  buffer[idx++]=data_len;       // Data length
    
  // Add data bytes
  for (int i = 0; i < data_len; i++)
    buffer[idx++]=data[i];
   
  // Add checksum and send via UART
  buffer[idx++] = DYSV17FCalculateChecksum(buffer, idx);
  uart_write_blocking(UartID, buffer, idx); 
  sleep_ms(DYSV17F_DELAY);   // Wait for module to process
}


// Set volume (DYSV17F_MIN_VOLUME to DYSV17F_MAX_VOLUME)
void DYSV17FSetVolume(uart_inst_t* UartID, uint8_t volume) 
{
  if (volume > DYSV17F_MAX_VOLUME) 
    volume=DYSV17F_MAX_VOLUME;
  DYSV17FSendCommand(UartID, 0x13, &volume, 1);
}


// Play specified track by NUMBER (recommended method)
void DYSV17FPlaySpecified(uart_inst_t* UartID, uint16_t track_number)
{
  uint8_t data[2];
  data[0]=(track_number >> 8) & 0xFF;  // High byte
  data[1]=track_number & 0xFF;         // Low byte
  DYSV17FSendCommand(UartID, 0x07, data, 2);
}


// Play by path (alternative method)
void DYSV17FPplayPath(uart_inst_t* UartID, uint16_t track_number, DYSV17F_device_t device)
{
  char path[11];
  uint8_t data[12];
  snprintf(path, sizeof(path), "/%05d*MP3", track_number);  
  data[0] = device;
  for (int i = 0; i < 10; i++) 
    data[i + 1] = (uint8_t)path[i];
  DYSV17FSendCommand(UartID, 0x08, data, 11);
}


// Play
void DYSV17FPlay(uart_inst_t* UartID)
{
  DYSV17FSendCommand(UartID, 0x02, NULL, 0);
}


// Pause
void DYSV17FPause(uart_inst_t* UartID)
{
  DYSV17FSendCommand(UartID, 0x03, NULL, 0);
}


// Stop
void DYSV17FStop(uart_inst_t* UartID)
{
  DYSV17FSendCommand(UartID, 0x04, NULL, 0);
}


// Next track
void DYSV17FNext(uart_inst_t* UartID)
{
  DYSV17FSendCommand(UartID, 0x06, NULL, 0);
}


// Previous track
void DYSV17FPrevious(uart_inst_t* UartID)
{
  DYSV17FSendCommand(UartID, 0x05, NULL, 0);
}


// Get sound count
uint16_t DYSV17FGetSoundCount(uart_inst_t* UartID)
{
  uint8_t response[6];
  DYSV17FSendCommand(UartID, 0x0C, NULL, 0);
  sleep_ms(DYSV17F_DELAY);     // Wait for response
    
  // Read response: AA 0C 02 [high] [low] [checksum]
  if (uart_is_readable(UartID))
  {
    uart_read_blocking(UartID, response, 6);
    if (response[0] == 0xAA && response[1] == 0x0C)
      return (response[3] << 8) | response[4];
  }  
  return 0;
}


// Check if we're still playing music
bool DYSV17FIsPlaying(uart_inst_t* UartID)
{
  uint8_t response[5];
  DYSV17FSendCommand(UartID, 0x0D, NULL, 0);
  sleep_ms(DYSV17F_DELAY);   // Wait for response

  // Read response: AA 0D 01 [status] [checksum]
  if (uart_is_readable(UartID))
  {
    uart_read_blocking(UartID, response, 5);
    if (response[0]==0xAA && response[1]==0x0D && response[2]==0x01)
      return (response[3]==0x01?true:false);
  }  
  return false;
}


// Initialize DY-SV17F module
void DYSV17FInit(uart_inst_t* UartID, uint16_t BaudRate, uint8_t UartTXPin, uint8_t UartRxPin)
{
  // Important; it is assumed that stdio_init_all() is called elsewhere.
  // Initialize pins used for UART
  gpio_init(UartTXPin);
  gpio_set_dir(UartTXPin, GPIO_OUT);
  gpio_put(UartTXPin, 1);  // Idle state
  gpio_init(UartRxPin);
  gpio_set_dir(UartRxPin, GPIO_IN);

  // Wait before initializing the pins for UART
  sleep_ms(DYSV17F_DELAY*8);

  // Configure pins for UART function
  gpio_set_function(UartTXPin, GPIO_FUNC_UART);
  gpio_set_function(UartRxPin, GPIO_FUNC_UART);  
  uart_init(UartID, BaudRate);
  uart_set_format(UartID, 8, 1, UART_PARITY_NONE);    // Explicitly set UART format (8N1)
  sleep_ms(DYSV17F_DELAY*20);  // Wait for module to boot

  // Empty RX buffer, just in case.
  while (uart_is_readable(UartID))
    uart_getc(UartID);
}

