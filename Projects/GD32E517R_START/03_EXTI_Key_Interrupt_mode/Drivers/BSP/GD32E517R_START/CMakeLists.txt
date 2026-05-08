project(GD32E517R_START LANGUAGES C CXX ASM)

add_library(GD32E517R_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32E517R_START/gd32e517r_start.c
    )

target_include_directories(GD32E517R_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32E517R_START
    )

target_link_libraries(GD32E517R_START PUBLIC GD32E51x_standard_peripheral)
