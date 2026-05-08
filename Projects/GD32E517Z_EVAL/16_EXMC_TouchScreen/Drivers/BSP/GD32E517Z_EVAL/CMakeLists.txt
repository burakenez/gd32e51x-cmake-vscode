project(GD32E517Z_EVAL LANGUAGES C CXX ASM)

add_library(GD32E517Z_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32E517Z_EVAL/gd32e517z_eval.c
    ${DRIVERS_DIR}/BSP/GD32E517Z_EVAL/gd32e517z_lcd_eval.c
    )

target_include_directories(GD32E517Z_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32E517Z_EVAL
    )

target_link_libraries(GD32E517Z_EVAL PUBLIC GD32E51x_standard_peripheral)
