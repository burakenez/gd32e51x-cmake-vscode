project(GD32E51x_usbhs_library LANGUAGES C CXX ASM)

add_library(GD32E51x_usbhs_library OBJECT
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/class/msc/Source/usbd_msc_bbb.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/class/msc/Source/usbd_msc_core.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/class/msc/Source/usbd_msc_scsi.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/core/Source/usbd_core.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/core/Source/usbd_enum.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/core/Source/usbd_transc.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/driver/Source/drv_usb_core.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/driver/Source/drv_usb_dev.c
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/driver/Source/drv_usbd_int.c
    )

target_include_directories(GD32E51x_usbhs_library PUBLIC
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/driver/Include
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/core/Include
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/class/msc/Include
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/ustd/common
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/ustd/class/msc
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/class/msc/Inc
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/device/core/Inc
    ${DRIVERS_DIR}/GD32E51x_usbhs_library/driver/Inc
    ${CMAKE_SOURCE_DIR}/Application/Core/Inc
    )

target_link_libraries(GD32E51x_usbhs_library PUBLIC GD32E517Z_EVAL)
