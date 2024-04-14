## Model details

- Developer: Charles Chen
- Model language: English / English / `en`
- Model date: April 12, 2024
- Model type: `wake-up words`/ `Speech-to-Text`
- Model version: `v0.0.1`
- Compatible with 🐸 STT version: `v1.4.0`
- License: Apache 2.0
- Contact email: sduchenchao@gmail.com
- Sound sample sources: self-record

## Intended use

Wake-up words for the English Language on 16kHz, single-channel audio.

Can only recognize the developer team

Trained from the basis of [Coqui STT](https://github.com/coqui-ai/STT) project



## Word list

**come go left right stop follow**



## File list

| Filename                    | File type                               | Source                    |
| --------------------------- | --------------------------------------- | ------------------------- |
| buddybot_private.tflite     | model file with few wake up words       | self-generated            |
| model_quantized.tflite      | general words model file                | from Coqui ai STT release |
| buddybot_general.scorer     | scorer file for model_quantized.tflite  | self-generated            |
| buddybot_private.scorer     | scorer file for buddybot_private.tflite | self-generated            |
| buddybot-0.2-checkpoint.zip | checkpoint file                         | self-generated            |



## How to Guide

1. generate model:

follow the guides of Coqui AI STT documents:

https://github.com/coqui-ai/STT/blob/main/notebooks/train_your_first_coqui_STT_model.ipynb

2. generate scorer file:

follow the guides of Coqui AI STT documents. general scorer uses the checkpoints from STT V1.4.0,  private scorer uses the checkpoints from self training model buddybot_private.tflite 

https://stt.readthedocs.io/en/latest/LANGUAGE_MODEL.html

