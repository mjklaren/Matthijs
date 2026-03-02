cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
include_directories(~/Develop/microcontroller/MyHardware ~/Develop/microcontroller/TISM)
project(Matthijs C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(Matthijs main.c 
~/Develop/microcontroller/TISM/TISM.c
~/Develop/microcontroller/TISM/TISM_EventLogger.c
~/Develop/microcontroller/TISM/TISM_Console.c
~/Develop/microcontroller/TISM/TISM_IRQHandler.c
~/Develop/microcontroller/TISM/TISM_NetworkManager.c
~/Develop/microcontroller/TISM/TISM_Postman.c
~/Develop/microcontroller/TISM/TISM_Scheduler.c
~/Develop/microcontroller/TISM/TISM_SoftwareTimer.c
~/Develop/microcontroller/TISM/TISM_UartMX.c
~/Develop/microcontroller/TISM/TISM_Watchdog.c
~/Develop/microcontroller/MyHardware/pca9685.c
~/Develop/microcontroller/MyHardware/WS2812b.c
)
pico_enable_stdio_usb(Matthijs 1)
pico_enable_stdio_uart(Matthijs 0)
target_link_libraries(Matthijs pico_stdlib pico_multicore hardware_pwm hardware_uart hardware_gpio hardware_irq hardware_i2c hardware_adc hardware_pio)
pico_add_extra_outputs(Matthijs)
