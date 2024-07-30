cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(laod_and_execute_model)

# Especificar el estándar de C
set(CMAKE_C_STANDARD 99)

# Establecer la ruta al directorio de los archivos fuente
set(SRC_DIR ${CMAKE_SOURCE_DIR}/C_code)

# Incluir los archivos fuente de la biblioteca
set(LIB_SOURCES
    ${SRC_DIR}/predict.c
    ${SRC_DIR}/predict.h
)

add_library(predict_lib STATIC ${LIB_SOURCES})

# Archivos fuente
set(SOURCES ${SRC_DIR}/laod_and_execute_model.c)

# Crear el ejecutable
add_executable(laod_and_execute_model ${SOURCES})

# Enlazar la biblioteca al ejecutable
target_link_libraries(laod_and_execute_model PRIVATE predict_lib)

# Crear un directorios
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/trained_models)
