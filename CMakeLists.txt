cmake_minimum_required(VERSION 3.15)
project(loudest-keyboard VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  set(CURSES_NEED_NCURSES FALSE)
else()
  set(CURSES_NEED_NCURSES TRUE)
endif()
  
find_package(Curses REQUIRED)

find_program(XXD_CMD xxd)
if(NOT XXD_CMD)
  message(FATAL_ERROR "Could not find program 'xxd'. Install it.")
endif()
function(embed_file INPUT OUTPUT COMMENT_TEXT)
    get_filename_component(INPUT_DIR "${INPUT}" DIRECTORY)
    get_filename_component(INPUT_NAME "${INPUT}" NAME)
    add_custom_command(
        OUTPUT "${OUTPUT}"
        COMMAND ${XXD_CMD} -i "${INPUT_NAME}" "${OUTPUT}"
        WORKING_DIRECTORY "${INPUT_DIR}"
        DEPENDS "${INPUT}"
        COMMENT "${COMMENT_TEXT}"
        VERBATIM
    )
endfunction()
set(DICT_OUTPUT     "${CMAKE_CURRENT_BINARY_DIR}/dict.h")
set(KEY_WAV_OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/key_wav.h")
set(BIGKEY_WAV_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/bigkey_wav.h")
set(SPACE_WAV_OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/space_wav.h")
embed_file("${CMAKE_CURRENT_SOURCE_DIR}/dict/words"       "${DICT_OUTPUT}"      "Generating dict array...")
embed_file("${CMAKE_CURRENT_SOURCE_DIR}/audio/key.wav"    "${KEY_WAV_OUTPUT}"   "Generating key.wav array...")
embed_file("${CMAKE_CURRENT_SOURCE_DIR}/audio/bigkey.wav" "${BIGKEY_WAV_OUTPUT}" "Generating bigkey.wav array...")
embed_file("${CMAKE_CURRENT_SOURCE_DIR}/audio/space.wav"  "${SPACE_WAV_OUTPUT}"  "Generating space.wav array...")

# Define the executable (target) and its source files
add_executable(keyboard
  src/main.c
  src/miniaudio.c
  "${DICT_OUTPUT}"
  "${KEY_WAV_OUTPUT}"
  "${BIGKEY_WAV_OUTPUT}"
  "${SPACE_WAV_OUTPUT}"
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
