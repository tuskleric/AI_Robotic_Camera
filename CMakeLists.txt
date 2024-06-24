cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(AI_ROBOTIC_CAMERA C CXX ASM)


pico_sdk_init()

# # # Set the binary directory for pico_sleep library build
#  set(PICO_SLEEP_BINARY_DIR ${CMAKE_BINARY_DIR}/pico_sleep_build)

# # # Add pico_sleep subdirectory
# add_subdirectory(${PICO_EXTRAS_PATH}/src/rp2_common/pico_sleep ${PICO_SLEEP_BINARY_DIR})

# Add your executable
add_executable(main
  scr/main.c
  scr/stepper_control.c
  scr/PWM.c
  scr/battery_monitor.c
  scr/scheduler.c
  scr/tick.c
  scr/tmc2209.c
  scr/common.c
  scr/blink.c

)

# Link the required libraries
target_link_libraries(main PRIVATE pico_stdlib pico_time hardware_pwm hardware_adc hardware_uart  )

# Add include directories
target_include_directories(main PRIVATE ${PICO_SDK_PATH})


pico_enable_stdio_usb(main 1)

pico_add_extra_outputs(main)
