cmake_minimum_required(VERSION 3.20)

project(key_checker LANGUAGES C CXX)

add_executable(total_keycode_name_size total_keycode_name_size.cpp keycode_names.c)

set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_PATH "${GENERATED_DIR}/total_keycode_name_size.h")

add_custom_command(
        OUTPUT "${GENERATED_PATH}"
        COMMAND "$<TARGET_FILE:total_keycode_name_size>" "${GENERATED_PATH}" TOTAL_KEYCODE_NAME_SIZE
        COMMENT "Generating ${GENERATED_PATH}"
        DEPENDS total_keycode_name_size
        VERBATIM
)

add_executable(key_checker WIN32 key_checker.cpp keycode_names.c "${GENERATED_PATH}")
target_compile_definitions(key_checker PRIVATE UNICODE)
target_include_directories(key_checker PRIVATE "${GENERATED_DIR}")
if(MSVC)
    target_link_options(key_checker PRIVATE /ENTRY:mainCRTStartup)
endif()
option(DEBUG_DISPLAY_ALL_KEYCODES "Replace GetAsyncKeyState with true" OFF)
if(DEBUG_DISPLAY_ALL_KEYCODES)
    target_compile_definitions(key_checker PRIVATE DEBUG_DISPLAY_ALL_KEYCODES)
endif()
