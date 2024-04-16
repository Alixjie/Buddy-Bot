# Voice Controlled module

## Functionality
This module enables the control of the buddybot via voice commands.



### Specific Components:
- **Audio Input:** Utilizes PulseAudio lib for audio stream input.

- **Speech Recognition:** Employs the [`stt` library](https://github.com/coqui-ai/STT) from COQUI AI to interface with a custom-trained model for keyword recognition.

- **Control Mechanism:** Based on recognized keywords, the buddybot performs specific movements.

  

## Considerations
The project uses the COQUI AI `stt` library, which is a locally running speech-to-text model. It can use either the general model provided by COQUI or a custom-trained model tailored for specific needs.

Due to performance and real-time considerations, this project is designed to recognize only a few specific keywords and execute corresponding motion patterns.



## Highlights
- **Speed:** The general model processes very slowly, so a smaller model trained with a limited amount of developer data is used to achieve faster performance.
- **Accuracy:** While the recognition speed is high, the accuracy for keyword detection is moderately reliable.

## Challenges

- The main disadvantages are the recognition limited to specific voices and severe susceptibility to noise interference.

### Unsolved Issues
- Further development is needed to address current limitations and improve robustness against noise and varied voice inputs.
- PulseAudio has some problems when running as root, however, the other modules of the project need to be root. So plan to move this module into an independent backstage program and use FIFO to communicate with the main program

## Keywords
- **Come:** The car approaches the operator using UWB and obstacle avoidance modules.
- **Left:** Turns left by 90 degrees.
- **Right:** Turns right by 90 degrees.
- **Go:** Moves forward.
- **Stop:** Stops movement.
- **Follow:** Activates follow mode (only in obstacle-free environments).

## References and Resources
This project extensively references the example code from COQUI AI's [STT](https://github.com/coqui-ai/STT)  library and the [spchcat](https://github.com/petewarden/spchcat) ` software, which is also based on COQUI's `stt`.



## File:

| Filename    | source               | function                                |
| ----------- | -------------------- | --------------------------------------- |
| audio/*     | from spchcat         | get audio stream by PulseAudio          |
| lib/*       | from stt             | stt lib files                           |
| model/*     | self-generated       | see model/buddybot_private_model_0.2.md |
| coqui-stt.h | from stt             | stt api headers                         |
| stt.h       | adapted from spchcat | module main header files                |
| stt.cpp     | adapted from spchcat | module main cpp files                   |



## Key Function:

int **initialize_stt**(ModelState** model_state, Settings* settings):	 initialzie model from model files.

static bool **process_live_input**(const Settings* settings, ModelState* model_state):	process audio stream and get keyword identification results

int **voice_control**(): module entrance.









