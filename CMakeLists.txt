cmake_minimum_required(VERSION 3.31)
project(CppTextSearch)
set(CMAKE_CXX_STANDARD 20)

find_package(FLEX REQUIRED)

set(LEX_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer/irtokengenerator.ll)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/lexgenerated)


set(GENERATED_LEX_CPP "${CMAKE_CURRENT_SOURCE_DIR}/src/lexgenerated/irtokengenerator.cpp")

add_custom_command(
        OUTPUT ${GENERATED_LEX_CPP}
        COMMAND ${FLEX_EXECUTABLE} -o ${GENERATED_LEX_CPP} ${LEX_INPUT}
        DEPENDS ${LEX_INPUT}
        COMMENT "Running flex to generate irtokengenerator.cpp"
        VERBATIM
)

add_executable(CppTextSearch
        src/main.cpp
        src/IRText.cpp
        src/utils.cpp
        ${GENERATED_LEX_CPP}
)

target_include_directories(CppTextSearch
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

message(STATUS "Generated file: ${GENERATED_LEX_CPP}")