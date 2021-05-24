cmake_minimum_required(VERSION 2.8)

project(simul_furuta)

# Carica pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Carica Allegro
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
find_package(Allegro REQUIRED)
include_directories("${ALLEGRO_INCLUDE_DIR}")

# Compila e genera eseguibile
include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")
file(GLOB SRC "${PROJECT_SOURCE_DIR}/src/*.c" "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC})
target_compile_options(${PROJECT_NAME} PUBLIC -Werror -Wall -Wextra)
target_link_libraries(${PROJECT_NAME} Threads::Threads "${ALLEGRO_LIBRARY}" m rt)
install(TARGETS ${PROJECT_NAME} DESTINATION "${PROJECT_SOURCE_DIR}/bin")
