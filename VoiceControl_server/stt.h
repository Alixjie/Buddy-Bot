/*
 * File: stt.h
 * Version: 1.0.0
 * Author: Charles Chen
 * license: apache 2.0
*/

#ifndef STT_H
#define STT_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "coqui-stt.h"



enum class Operation {
    COME,
    LEFT,
    RIGHT,
    STOP,
    FOLLOW,
    GO,
    NONE  //no operation
};



typedef struct SettingsStruct {
    char* source;
    std::string models_dir;
    int mode;
    std::string model;
    std::string scorer;
    int source_buffer_size;
    int beam_width;
    bool show_times;
    int stream_size;
    const char* hot_words;
    const char* stream_capture_file;
    int stream_capture_duration;
} Settings;




const std::vector<std::string> KEYWORDS = {"come","left", "right", "stop", "follow", "go"};

//
/*
path of the model and scorer
constexpr char MODE1_MODEL_PATH[] = "/path/to/model1";
constexpr char MODE1_SCORER_PATH[] = "/path/to/scorer1";
constexpr char MODE2_MODEL_PATH[] = "/path/to/model2";
constexpr char MODE2_SCORER_PATH[] = "/path/to/scorer2";
*/

//void set_defaults(Settings* settings);
//int initialize_stt(ModelState** model, Settings* settings);
//bool process_live_input(Settings* settings, ModelState* model_state);
int voice_control();
#endif // STT_H
