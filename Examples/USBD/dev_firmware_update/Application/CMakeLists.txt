project(Application LANGUAGES C CXX ASM)

add_executable(Application)

set(TARGET_SRC
	# Core
    Core/Src/exmc_nandflash.c
    Core/Src/gd25qxx.c
    Core/Src/gd32e51x_it.c
    Core/Src/gd32e51x_usbd_hw.c
    Core/Src/inter_flash_if.c
    Core/Src/main.c
    Core/Src/nand_flash_if.c
    Core/Src/nor_flash_if.c
    Core/Src/systick.c
    Core/Src/system_gd32e51x.c
	
    # Startup
    Startup/startup_gd32e51x_cl.s

    # User
    User/syscalls.c
    )

target_sources(Application PRIVATE ${TARGET_SRC})

set(TARGET_INC_DIR
	${CMAKE_SOURCE_DIR}/Application/Core/Inc
    )

target_include_directories(Application PRIVATE ${TARGET_INC_DIR})

target_link_options(Application PRIVATE
	-T${CMAKE_SOURCE_DIR}/gd32e51x_flash.ld -Xlinker
    -L${CMAKE_SOURCE_DIR}
	)

target_link_options(Application PRIVATE
	-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.map
	)

target_link_libraries(Application PRIVATE CMSIS)
target_link_libraries(Application PRIVATE GD32E51x_standard_peripheral)
target_link_libraries(Application PRIVATE GD32E513Z_EVAL)
target_link_libraries(Application PRIVATE GD32E51x_usbd_library)

add_custom_command(TARGET Application
    POST_BUILD
    COMMAND echo -- Running Post Build Commands
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:Application> ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:Application> ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:Application>
    COMMAND ${CMAKE_OBJDUMP} -h -S $<TARGET_FILE:Application> > ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.list
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:Application> > ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.bsz
    COMMAND ${CMAKE_SIZE} --format=sysv -x $<TARGET_FILE:Application> > ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_NAME:Application>.ssz
    )
