cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(firmware)

set(TARGET_ELF ${PROJECT_NAME}.elf)

file(GLOB HAL_FILES "src/wch/hal/*.c")
file(GLOB PERIPH_FILES "src/wch/periph/*.c")
file(GLOB CORE_FILES "src/wch/rvmsis/core_riscv.c" "src/wch/startup/startup_CH583.S")
file(GLOB SRC_FILES "src/main.c")

add_executable(${TARGET_ELF} ${HAL_FILES} ${PERIPH_FILES} ${CORE_FILES} ${SRC_FILES})

add_library(ch58xble STATIC IMPORTED)
set_target_properties(ch58xble PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/libch58xble.a)
add_library(isp583 STATIC IMPORTED)
set_target_properties(isp583 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/libisp583.a)
target_link_libraries(${TARGET_ELF} PUBLIC ch58xble isp583)

target_include_directories(${TARGET_ELF} PUBLIC ${CMAKE_SOURCE_DIR}/src/wch/ble/include)
target_include_directories(${TARGET_ELF} PUBLIC ${CMAKE_SOURCE_DIR}/src/wch/hal/include)
target_include_directories(${TARGET_ELF} PUBLIC ${CMAKE_SOURCE_DIR}/src/wch/periph/include)
target_include_directories(${TARGET_ELF} PUBLIC ${CMAKE_SOURCE_DIR}/src/wch/rvmsis/include)

enable_language(C CXX ASM)
target_compile_features(${TARGET_ELF} PRIVATE cxx_std_17)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/link.ld)
target_link_options(${TARGET_ELF} PUBLIC -T${LINKER_SCRIPT})
target_link_options(${TARGET_ELF} PUBLIC -Wl,-Map=$<TARGET_FILE:${TARGET_ELF}>.map)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/romfs.c
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/wch/rtthread/tools/mkromfs.py ${RT_DFS_ROMFS_PATH} ${CMAKE_CURRENT_BINARY_DIR}/romfs.c)
add_custom_command(TARGET ${TARGET_ELF} POST_BUILD
        COMMAND ${CROSS_OBJDUMP} "-CdSx" $<TARGET_FILE:${TARGET_ELF}> > $<TARGET_FILE:${TARGET_ELF}>.lst
        COMMENT "Generating disassembler file")
add_custom_command(TARGET ${TARGET_ELF} POST_BUILD
        COMMAND ${CROSS_OBJCOPY} "-Obinary" $<TARGET_FILE:${TARGET_ELF}> $<TARGET_FILE:${TARGET_ELF}>.bin
        COMMENT "Generating bin file")
add_custom_command(TARGET ${TARGET_ELF} POST_BUILD
        COMMAND ${CROSS_OBJCOPY} "-Oihex" $<TARGET_FILE:${TARGET_ELF}> $<TARGET_FILE:${TARGET_ELF}>.hex
        COMMENT "Generating hex file")