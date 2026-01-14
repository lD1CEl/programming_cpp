 cmake_minimum_required(VERSION 3.10)
project(DiggerCPP)

set(CMAKE_CXX_STANDARD 17)

# 1. Задаем путь к локальной папке SFML
set(SFML_PATH "${CMAKE_SOURCE_DIR}/SFML-2.6.1")

# 2. Говорим CMake, где искать заголовки (.hpp) и библиотеки (.lib/.a)
include_directories("${SFML_PATH}/include")
link_directories("${SFML_PATH}/lib")

# 3. Собираем исполняемый файл из всех .cpp
file(GLOB_RECURSE SOURCES "src/*.cpp" "main.cpp")
add_executable(Digger ${SOURCES})

# 4. Привязываем модули SFML.
target_link_libraries(Digger
    debug sfml-graphics-d optimized sfml-graphics
    debug sfml-window-d   optimized sfml-window
    debug sfml-system-d   optimized sfml-system
    debug sfml-audio-d    optimized sfml-audio
)

# 5. Копируем папку images
add_custom_command(TARGET Digger POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/images" "${CMAKE_BINARY_DIR}/images"
)

# 6. Копируем папку fonts
add_custom_command(TARGET Digger POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/fonts" "${CMAKE_BINARY_DIR}/fonts"
)

# 7. НОВОЕ: Копируем файл музыки Digger.mp3 (с большой буквы)
add_custom_command(TARGET Digger POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_SOURCE_DIR}/Digger.mp3" "${CMAKE_BINARY_DIR}/Digger.mp3"

)

# 8. Копируем DLL-файлы SFML
add_custom_command(TARGET Digger POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${SFML_PATH}/bin" "${CMAKE_BINARY_DIR}/"
)