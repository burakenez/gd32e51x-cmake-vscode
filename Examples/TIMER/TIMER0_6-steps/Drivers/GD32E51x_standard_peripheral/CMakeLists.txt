project(GD32E51x_standard_peripheral LANGUAGES C CXX ASM)

# Comment-out unused source files.
add_library(GD32E51x_standard_peripheral OBJECT
	${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_adc.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_bkp.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_can.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_cmp.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_crc.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_ctc.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_dac.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_dbg.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_dma.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_enet.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_exmc.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_exti.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_fmc.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_fwdgt.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_gpio.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_i2c.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_misc.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_pmu.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_rcu.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_rtc.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_sdio.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_shrtimer.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_spi.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_sqpi.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_timer.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_tmu.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_usart.c
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Source/gd32e51x_wwdgt.c
    )

target_include_directories(GD32E51x_standard_peripheral PUBLIC
    ${DRIVERS_DIR}/GD32E51x_standard_peripheral/Include
    )

# CMSIS header only library is linked.
target_link_libraries(GD32E51x_standard_peripheral PUBLIC CMSIS)
