cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(pico_fyp C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(main
  scr/main.c
  scr/stepper_control.c
  scr/PWM.c
  
)


# target_include_directories(main PRIVATE User/Joel/Pico/pico-sdk/src/common/)
target_link_libraries(main PRIVATE pico_stdlib hardware_pwm) 


pico_enable_stdio_usb(main 1)
pico_add_extra_outputs(main)
