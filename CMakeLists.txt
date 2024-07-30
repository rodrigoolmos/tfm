cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(laod_and_execute_model)

# Especificar el estándar de C
set(CMAKE_C_STANDARD 99)

# Archivos fuente
set(SOURCES C-code/laod_and_execute_model.c)

# Crear el ejecutable
add_executable(laod_and_execute_model ${SOURCES})
