project(GD32E513Z_EVAL LANGUAGES C CXX ASM)

add_library(GD32E513Z_EVAL OBJECT
    ${DRIVERS_DIR}/BSP/GD32E513Z_EVAL/gd32e513z_eval.c
    )

target_include_directories(GD32E513Z_EVAL PUBLIC
    ${DRIVERS_DIR}/BSP/GD32E513Z_EVAL
    )

target_link_libraries(GD32E513Z_EVAL PUBLIC GD32E51x_standard_peripheral)
