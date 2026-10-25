cmake_minimum_required(VERSION 3.15)
project(loudest-keyboard VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CURSES_NEED_NCURSES TRUE)

find_package(Curses REQUIRED)
find_program(XXD_CMD xxd)
if(NOT XXD_CMD)
  message(FATAL_ERROR "Could not find program 'xxd'. Install it.")
endif()
set(DICT_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/dict/words")
set(DICT_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/dict.h")
add_custom_command(
    OUTPUT "${DICT_OUTPUT}"
    COMMAND ${XXD_CMD} -i "${DICT_INPUT}" "${DICT_OUTPUT}"
    DEPENDS "${DICT_INPUT}"
    COMMENT "Generating dict array..."
    VERBATIM
)

# Define the executable (target) and its source files
add_executable(keyboard
  src/main.c
  src/miniaudio.c
  "${DICT_OUTPUT}"
)

target_include_directories(keyboard PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(keyboard PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

if(MSVC)
  target_compile_options(keyboard PRIVATE /W4)
  set_source_files_properties(src/miniaudio.c
    PROPERTIES COMPILE_FLAGS "/W0"
  )
else()
  target_compile_options(keyboard PRIVATE -Wall -Wextra)
  set_source_files_properties(src/miniaudio.c
    PROPERTIES COMPILE_FLAGS "-w"
  )
endif()

target_include_directories(keyboard PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(keyboard PRIVATE ${CURSES_LIBRARIES})
