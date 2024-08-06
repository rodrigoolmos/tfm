cmake_minimum_required(VERSION 3.10)

project(laod_and_execute_model)

set(CMAKE_C_STANDARD 99)

set(DIR_SW ${CMAKE_SOURCE_DIR}/C_code_vitis)
set(DIR_HW ${CMAKE_SOURCE_DIR}/linux_driver)

set(LIB_SOURCES
    ${DIR_SW}/predict.c
    ${DIR_SW}/predict.h
)

add_library(predict_lib STATIC ${LIB_SOURCES})

set(SOURCES_SW ${DIR_SW}/laod_and_execute_model.c)

add_executable(execute_model_sw ${SOURCES_SW})

if(UNIX AND NOT APPLE)
    add_executable(execute_model_hw ${SOURCES_HW})
    set(SOURCES_HW ${DIR_HW}/main.c)
endif()

target_link_libraries(execute_model_sw PRIVATE predict_lib)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/trained_models)
