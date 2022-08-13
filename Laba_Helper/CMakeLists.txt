cmake_minimum_required(VERSION 3.16)

project(Laba_Helper
        LANGUAGES C)

add_compile_options(-Wall -Wextra -Werror)

aux_source_directory(./src SRC_LIST)

add_executable(Laba_Helper ${SRC_LIST})

target_include_directories(Laba_Helper 
                           PUBLIC ./include)

target_link_libraries(Laba_Helper PRIVATE m)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
