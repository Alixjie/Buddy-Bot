# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = voicecontrol

QT = core

HEADERS = \
   $$PWD/audio/audio_buffer.h \
   $$PWD/audio/pa_list_devices.h \
   $$PWD/audio/wav_io.h \
   $$PWD/coqui-stt.h \
   $$PWD/stt.h

SOURCES = \
   $$PWD/audio/audio_buffer.c \
   $$PWD/audio/pa_list_devices.c \
   $$PWD/audio/wav_io.c \
   $$PWD/stt.cpp

INCLUDEPATH = \
    $$PWD/. \
    $$PWD/audio

LIBS += $$PWD/lib/libkenlm.so
LIBS += $$PWD/lib/libstt.so
LIBS += -lportaudio -lpulse-simple -lpulse -lstt
#DEFINES = 

