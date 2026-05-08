project(FatFs LANGUAGES C CXX ASM)

add_library(FatFs OBJECT
    ${MIDDLEWARES_DIR}/Third_Party/FatFs/source/ff.c
    ${MIDDLEWARES_DIR}/Third_Party/FatFs/source/ffunicode.c
    )

target_include_directories(FatFs PUBLIC
    ${MIDDLEWARES_DIR}/Third_Party/FatFs/source
    ${MIDDLEWARES_DIR}/Third_Party/FatFs/Include
    ${MIDDLEWARES_DIR}/Third_Party/FatFs/Inc
    ${CMAKE_SOURCE_DIR}/Application/Core/Inc
    )

