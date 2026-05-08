project(GD32E518C_START LANGUAGES C CXX ASM)

add_library(GD32E518C_START OBJECT
    ${DRIVERS_DIR}/BSP/GD32E518C_START/gd32e518c_start.c
    )

target_include_directories(GD32E518C_START PUBLIC
    ${DRIVERS_DIR}/BSP/GD32E518C_START
    )

target_link_libraries(GD32E518C_START PUBLIC GD32E51x_standard_peripheral)
