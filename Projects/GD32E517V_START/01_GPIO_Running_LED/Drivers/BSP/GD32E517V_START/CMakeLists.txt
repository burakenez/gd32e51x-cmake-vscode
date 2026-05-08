project(GD32E517V_START LANGUAGES C CXX ASM)

add_library(GD32E517V_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32E517V_START/gd32e517v_start.c
    )

target_include_directories(GD32E517V_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32E517V_START
    )

target_link_libraries(GD32E517V_START PUBLIC GD32E51x_standard_peripheral)
