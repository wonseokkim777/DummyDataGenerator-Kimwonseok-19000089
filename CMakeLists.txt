cmake_minimum_required(VERSION 3.16)
project(DummyDataGenerator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(DummyDataGenerator
    src/main.cpp
    src/SimpleJsonParser.cpp
    src/DummyDataGenerator.cpp
)

target_include_directories(DummyDataGenerator PRIVATE src)

# 한국어 문자열 리터럴을 위한 UTF-8 컴파일 옵션 (MSVC 전용)
if(MSVC)
    target_compile_options(DummyDataGenerator PRIVATE /utf-8)
endif()
