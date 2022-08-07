cmake_minimum_required(VERSION 3.16)

project(LabaCL
        LANGUAGES C)

add_compile_options(-Wall -Wextra -Werror)

aux_source_directory(./src SRC_LIST)

add_executable(LabaCL ${SRC_LIST})

target_include_directories(LabaCL 
                           PUBLIC ./include)

target_link_libraries(LabaCL PRIVATE m)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
