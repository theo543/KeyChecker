cmake_minimum_required(VERSION 3.20)

project(key_checker LANGUAGES C CXX)

add_executable(key_checker WIN32 key_checker.cpp keycode_names.c)
target_compile_definitions(key_checker PRIVATE UNICODE)
if(MSVC)
    target_link_options(key_checker PRIVATE /ENTRY:mainCRTStartup)
endif()
