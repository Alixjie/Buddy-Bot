#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <sndfile.h>

#include "coqui-stt.h"

std::vector<short> loadAudioFile(const std::string& filename) {
    SF_INFO sfinfo;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (file == NULL) {
        std::cerr << "Could not open audio file: " << filename << std::endl;
        return std::vector<short>();
    }

    std::vector<short> buffer(sfinfo.frames);
    sf_read_short(file, buffer.data(), sfinfo.frames);
    sf_close(file);

    return buffer;
}



int main(int argc, char **argv) {

    if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <audio_path>\n";
    return 1;
    }

  std::string audio_path = argv[1];

    struct timeval t1,t2;
    double timeuse;
    // 创建一个ModelState对象
    std::cout << "cc1" << std::endl;


    ModelState* model = nullptr;
    std::string model_buffer;

    // 尝试从文件创建模型
    int result = STT_CreateModel("/home/group3/work/code/stt-test/model/digits/model_quantized.tflite", &model);
    std::cout << "cc1.1" << std::endl;
    // 如果从文件创建模型失败，尝试从内存缓冲区创建模型
    if (result != 0) {
        // 读取模型文件到内存缓冲区
        std::cout << "cc1.2" << std::endl;
        std::ifstream is_model("/home/group3/work/code/stt-test/model/large/model.tflite", std::ios::binary);
        std::stringstream buffer_model;
        buffer_model << is_model.rdbuf();
        model_buffer = buffer_model.str();
        std::cout << "cc1.3" << std::endl;
        // 从内存缓冲区创建模型
        result = STT_CreateModelFromBuffer(model_buffer.c_str(), model_buffer.size(), &model);
        std::cout << "cc1.4" << std::endl;
    }

    // 检查是否有错误
    if (result != 0) {
        std::cerr << "Error during model creation." << std::endl;
        return 1;
    }

    std::cout << "cc2" << std::endl;
    // 检查是否有错误
    if (result != 0) {
        std::cerr << "Error during model creation." << std::endl;
        return 1;
    }

   // result = STT_SetModelBeamWidth(model,10);
    int ModelBeamWidth = STT_GetModelBeamWidth(model);
    std::cout << ModelBeamWidth << std::endl;
    int ModelSampleRate= STT_GetModelSampleRate(model);
    std::cout << ModelSampleRate << std::endl;
    std::cout << "cc3" << std::endl;


    int status = STT_EnableExternalScorer(model, "/home/group3/work/code/stt-test/model/digits/digits.scorer");
    if (status != STT_ERR_OK) {
      char* error = STT_ErrorCodeToErrorMessage(status);
      fprintf(stderr, "Could not enable external scorer: %s\n", error);
      STT_FreeString(error);
      return 1;
    }
  // sphinx-doc: c_ref_model_2_end
    // 加载音频文件
    std::vector<short> audioBuffer = loadAudioFile(audio_path);
    std::cout << "cc4" << std::endl;
    gettimeofday(&t1,NULL);

    char* text = nullptr;
    // 进行语音到文本转换
        text = STT_SpeechToText(model, audioBuffer.data(), audioBuffer.size());

    // 检查是否有错误
    if (text == NULL) {
        std::cerr << "Error during speech to text conversion." << std::endl;
        return 1;
    }
    gettimeofday(&t2,NULL);
    timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;

    std::cout << "time = " << timeuse << std::endl;  //输出时间（单位：ｓ）
    std::cout << "cc5" << std::endl;
    // 输出转换后的文本
    std::cout << text << std::endl;

    // 释放资源
    STT_FreeString(text);

    return 0;
}
