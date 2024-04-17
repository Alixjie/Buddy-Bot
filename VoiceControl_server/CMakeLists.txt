cmake_minimum_required(VERSION 3.10)
project(voicecontrol)

set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/audio)

# Source files
set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/audio/audio_buffer.c
    ${CMAKE_SOURCE_DIR}/audio/pa_list_devices.c
    ${CMAKE_SOURCE_DIR}/audio/wav_io.c
    ${CMAKE_SOURCE_DIR}/stt.cpp
)

# Header files
set(HEADER_FILES
    ${CMAKE_SOURCE_DIR}/audio/audio_buffer.h
    ${CMAKE_SOURCE_DIR}/audio/pa_list_devices.h
    ${CMAKE_SOURCE_DIR}/audio/wav_io.h
    ${CMAKE_SOURCE_DIR}/coqui-stt.h
    ${CMAKE_SOURCE_DIR}/stt.h
)

# Libraries
link_directories(${CMAKE_SOURCE_DIR}/lib)
set(LIBRARIES
    kenlm
    stt
    portaudio
    pulse-simple
    pulse
    rt
    pthread
)

add_executable(voicecontrol ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(voicecontrol ${LIBRARIES})
