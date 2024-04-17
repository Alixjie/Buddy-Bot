/*
 * File: stt.cpp
 * Version: 1.0.0
 * Author: Charles Chen
 * license: apache 2.0
 * 
 * This file contains the implementation of speech-to-text (STT) functionality using the DeepSpeech model.
 * It includes functions for processing audio data, converting it into text.
 * 
 * Please note that a significant portion of the code in this file is adapted from examples provided by Coqui STT and spchacat.
 * The original code can be found at: https://github.com/coqui-ai/STT/tree/main/native_client
 *                                    https://github.com/petewarden/spchcat
 */
#include "stt.h" 

#include <algorithm>
#include <vector>
#include <iterator>
#include <cstring>
#include <iostream>

#include <pulse/simple.h>
#include <pulse/error.h>

#include "coqui-stt.h"

extern "C" {
#include "audio/audio_buffer.h"
#include "audio/wav_io.h"
}

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>



#define SHM_NAME "/shm_vc"
#define SEM_NAME "/sem_vc"

static Operation* send_operation;
static sem_t* sem;


/*
std:c:vector<short> loadAudioFile(const std::string& filename) {
    SF_INFO sfinfo;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (file == NULL) {
        std::cerr << "Could not open audio file: " << filename << std::endl;
        return std::vector<short>();
    }

    std::vector<short> buffer(sfinfo.frames);
    sf_reaccd_short(file, buffer.data(), sfinfo.frames);
    sf_close(file);

    return buffer;
}
*/
int read_mode_from_file(const std::string& mode_dir) {
    std::ifstream file(mode_dir);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + mode_dir);
    }

    int mode_value;
    file >> mode_value;
    if (mode_value != 0 && mode_value != 1) {
        throw std::runtime_error("Invalid mode value in file: " + mode_dir);
    }

    return mode_value;
}

void set_defaults(Settings* settings) {

  settings->source = const_cast<char*>("mic");
  settings->models_dir = "/home/group3/models/";
  //settings->models_dir = "/home/cc/work/code/Buddy-Bot/VoiceControl_server/model/";
  std::string mode_dir = std::string(settings->models_dir) + "mode";
  settings->mode = read_mode_from_file(mode_dir);
  if (settings->mode == 0) {
    settings->model = std::string(settings->models_dir) + "buddybot_private.tflite";
    settings->scorer = std::string(settings->models_dir) + "buddybot_private.scorer";
  } else {
    settings->model = std::string(settings->models_dir) + "model_quantized.tflite";
    settings->scorer = std::string(settings->models_dir) + "buddybot_general.scorer";
  }
  settings->source_buffer_size = 160 * 4;
  settings->beam_width = 10;
  settings->show_times = false;
  settings->stream_size = 0;
  settings->hot_words = NULL;
  settings->stream_capture_file = NULL;
  settings->stream_capture_duration = 16000;
}


int initialize_stt(ModelState** model_state, Settings* settings) {
    std::string model_buffer;

    //create model from file
    // interface from coqui-stt
    int status = STT_CreateModel(settings->model.c_str(), model_state);
    /*
    // if failed to create model from file, try to create model from buffer
    if (status != 0) {
        std::ifstream is_model(settings->model.c_str(), std::ios::binary);
        std::stringstream buffer_model;
        buffer_model << is_model.rdbuf();
        model_buffer = buffer_model.str();
        status = STT_CreateModelFromBuffer(model_buffer.c_str(), model_buffer.size(), model_state);
        if (status != 0) {
            throw std::runtime_error("Error during model creation.");
        }
    }*/
    if (status != 0) {
        throw std::runtime_error("Error during model creation.");
    }

    status = STT_EnableExternalScorer(*model_state, settings->scorer.c_str());
    if (status != STT_ERR_OK) {
        throw std::runtime_error("Could not enable external scorer");
    }

    status = STT_SetModelBeamWidth(*model_state,settings->beam_width);
    if (status != STT_ERR_OK) {
        throw std::runtime_error("Error during setting beam width.");
    }

    //To do: add hot words
    //status = STT_SetHotWords(model, settings->hot_words);

    return 0;
}



Operation keyword_to_operation(const std::string& keyword) {
    if (keyword == "come") {
        return Operation::COME;
    } else if (keyword == "left") {
        return Operation::LEFT;
    } else if (keyword == "right") {
        return Operation::RIGHT;
    } else if (keyword == "stop") {
        return Operation::STOP;
    } else if (keyword == "follow") {
        return Operation::FOLLOW;
    } else if (keyword == "go") {
        return Operation::GO;
    } else {
        return Operation::NONE;
    }
}

std::string get_last_keyword(const std::string& text) {
    std::vector<std::string> keywords = KEYWORDS;
    
    std::istringstream iss(text);
    std::vector<std::string> words((std::istream_iterator<std::string>(iss)),
                                    std::istream_iterator<std::string>());

    for (auto it = words.rbegin(); it != words.rend(); ++it) {
        if (std::find(keywords.begin(), keywords.end(), *it) != keywords.end()) {
            return *it;
        }
    }

    return "";
}

Operation get_operation(const std::string& text) {
    if (text.empty()) {
        return Operation::NONE;
    }

    std::string keyword = get_last_keyword(text.c_str());
    printf("Keyword: %s  ", keyword.c_str());
    Operation operation = keyword_to_operation(keyword);

    return operation;
}


std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

static std::string print_changed_lines(const std::string& current_text, const std::string& previous_text) {
    // Has anything changed since last time?
    if (previous_text.empty() || (current_text == previous_text)) {
        return "";
    }

    std::ostringstream output;

    std::vector<std::string> current_lines = split(current_text, '\n');
    std::vector<std::string> previous_lines = split(previous_text, '\n');

    if (current_lines.size() > previous_lines.size()) {
        size_t start_index = (previous_lines.size() > 0) ? (previous_lines.size() - 1) : 0;
        for (size_t i = start_index; i < current_lines.size() - 1; ++i) {
            output << "\r" << current_lines[i] << "\n";
        }
    }

    output << "\r" << current_lines.back() << "        ";

    return output.str();
}

static std::string plain_text_from_transcript(const CandidateTranscript* transcript) {
    std::string result = "";
    float previous_time = 0.0f;
    for (size_t i = 0; i < transcript->num_tokens; ++i) {
        const TokenMetadata* token = &transcript->tokens[i];
        const float current_time = token->start_time;
        const float time_since_previous = current_time - previous_time;
        if (time_since_previous > 1.0f) {
            if (!result.empty() && result.back() == ' ') {
                result.back() = '\n';
            }
            else {
                result += "\n";
            }
            if (strcmp(token->text, " ") != 0) {
                result += token->text;
            }
        }
        else {
            result += token->text;
        }
        previous_time = current_time;
    }
    return result;
}

static Operation output_streaming_transcript(const Metadata* current_metadata,
    const Metadata* previous_metadata) {
    const CandidateTranscript* current_transcript =
        &current_metadata->transcripts[0];
    std::string current_text = plain_text_from_transcript(current_transcript);
    std::string previous_text;
    if (previous_metadata == NULL) {
        previous_text = "";
    }
    else {
        const CandidateTranscript* previous_transcript =
            &previous_metadata->transcripts[0];
        previous_text = plain_text_from_transcript(previous_transcript);
    }

    std::string result = print_changed_lines(current_text, previous_text);
    Operation operation = get_operation(result);

    return operation;
}



void handle_Operation(Operation operation) {

    static Operation lastOperation;
    static auto lastOperationTime = std::chrono::steady_clock::now();
    static auto lastModificationTime = std::chrono::steady_clock::now();

    if (operation == lastOperation) {
        auto now = std::chrono::steady_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - lastOperationTime).count();
        if (elapsedSeconds < 2) {
            return;
        }
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - lastModificationTime).count();
    if (elapsedSeconds < 1) {
        return;
    }

    *send_operation = operation;


        
    int sem_value;
    do {
        sem_getvalue(sem, &sem_value);
        if (sem_value > 0) {
            sem_trywait(sem);
        }
    } while (sem_value > 0);

    
    sem_post(sem);

    lastOperation = operation;
    lastOperationTime = std::chrono::steady_clock::now();
    lastModificationTime = std::chrono::steady_clock::now();
}

//This function is adapted from the sochcat example code
static bool process_live_input(const Settings* settings, ModelState* model_state) {
  const char* device_name = NULL;

  const uint32_t model_rate = STT_GetModelSampleRate(model_state);
  const pa_sample_spec sample_spec = { PA_SAMPLE_S16LE, model_rate, 1 };
  int pa_error;
  pa_simple* source_stream = pa_simple_new(
    NULL, "buddybot", PA_STREAM_RECORD, device_name, "buddybot",
    &sample_spec, NULL, NULL, &pa_error);
  if (source_stream == NULL) {
    if (device_name == NULL) {
      fprintf(stderr, "Unable to open default audio input device.\n");
    }
    else {
      fprintf(stderr,
        "Unable to open audio input device named '%s', from source '%s'.\n",
        device_name, settings->source);
    }
    fprintf(stderr, "The command 'pactl list sources' will show available devices.\n");
    fprintf(stderr, "You can use the contents of the 'Name:' field as the '--source' argument to specify one.\n");
    return false;
  }

  StreamingState* streaming_state = NULL;
  const int stream_error = STT_CreateStream(model_state, &streaming_state);
  if (stream_error != STT_ERR_OK) {
    const char* error_message = STT_ErrorCodeToErrorMessage(stream_error);
    fprintf(stderr, "STT_CreateStream() failed with '%s'\n", error_message);
    pa_simple_free(source_stream);
    return false;
  }

  const size_t source_buffer_byte_count = settings->source_buffer_size * 2;
  int16_t* source_buffer = new int16_t[source_buffer_byte_count / sizeof(int16_t)];

  AudioBuffer* capture_buffer = NULL;
  if (settings->stream_capture_file != NULL) {
    capture_buffer = audio_buffer_alloc(model_rate, settings->stream_capture_duration, 1);
  }
  int stream_capture_offset = 0;

    Metadata* previous_metadata = NULL;
    Operation operation = Operation::NONE;
  while (true) {
    int read_error;
    const int read_result = pa_simple_read(source_stream, source_buffer,
      source_buffer_byte_count, &read_error);
    if (read_result < 0) {
      fprintf(stderr, "pa_simple_read() failed with '%s'.\n",
        pa_strerror(read_error));
      break;
    }
    if (capture_buffer != NULL) {
      if ((stream_capture_offset + settings->source_buffer_size) > settings->stream_capture_duration) {
        break;
      }
      int16_t* current_capture = capture_buffer->data + stream_capture_offset;
      memcpy(current_capture, source_buffer, source_buffer_byte_count);
      stream_capture_offset += settings->source_buffer_size;
    }

    STT_FeedAudioContent(streaming_state, source_buffer,
      settings->source_buffer_size);
    Metadata* current_metadata = STT_IntermediateDecodeWithMetadata(streaming_state, 1);

    operation = output_streaming_transcript(current_metadata, previous_metadata);
    if (operation != Operation::NONE) {
        handle_Operation(operation);
    }


    if (previous_metadata != NULL) {
      STT_FreeMetadata(previous_metadata);
    }
    previous_metadata = current_metadata;
  }

  if (capture_buffer != NULL) {
    wav_io_save(settings->stream_capture_file, capture_buffer);
    audio_buffer_free(capture_buffer);
  }

  if (previous_metadata != NULL) {
    STT_FreeMetadata(previous_metadata);
  }
  pa_simple_free(source_stream);
  delete[] source_buffer;
  return true;
}

int voice_control() {
    Settings settings;
    set_defaults(&settings);

    ModelState* model_state = NULL;
    initialize_stt(&model_state, &settings);

    bool success = process_live_input(&settings, model_state);
    if (!success) {
        STT_FreeModel(model_state);
        return 1; //exit with error
    }

    STT_FreeModel(model_state);

    return 0;
}


int main() {


    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(Operation));
    send_operation = (Operation*)mmap(0, sizeof(Operation), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);


    voice_control();

    munmap(send_operation, sizeof(int));
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);

    return 0;
}  

