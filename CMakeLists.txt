cmake_minimum_required(VERSION 3.20)

project(key_checker LANGUAGES C)

add_executable(key_checker key_checker.c)
target_compile_definitions(key_checker PRIVATE UNICODE)
