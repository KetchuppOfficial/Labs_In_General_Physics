cmake_minimum_required(VERSION 3.16)

project(Laba_Helper
        LANGUAGES C)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

set(CMAKE_C_STANDARD          17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ./)
endif()

add_compile_options(-Wall -Wextra -Werror -Wpedantic)

aux_source_directory(./src SRC_LIST)

add_executable(Laba_Helper ${SRC_LIST})

target_include_directories(Laba_Helper 
                           PUBLIC ./include)

target_link_libraries(Laba_Helper PRIVATE m)

install(TARGETS Laba_Helper
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
