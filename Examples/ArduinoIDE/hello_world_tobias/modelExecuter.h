#ifndef modelExecuter_H
#define modelExecuter_H

#include "Arduino.h"
///////////////////////
/*
//tobias libs
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/recording_micro_allocator.h"
#include "tensorflow/lite/micro/recording_micro_allocator.h"
#include "tensorflow/lite/micro/recording_micro_allocator.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
*/
//////////////////////sajjad
#include <TensorFlowLite.h>
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "mbed.h"
#include "mbed_mem_trace.h"
#include "printer.h"


//static tflite::AllOpsResolver resolver;

class ModelExecuter {

  public:
    ModelExecuter(Stream *serial);
    void setModel(const unsigned char *model, uint32_t modelLength);
    const unsigned char* getModel();
    uint32_t getModelLength();
    int execute();
    int execute2();
    void initTFLM();
    uint32_t getUsedMemory();
    
const int kInferencesPerCycle = 10000;
int led = LED_BUILTIN;
const float kXrange = 2.f * 3.14159265359f;
bool initialized = false;
int inference_count = 0;

  private:
    const unsigned char *model;
    uint32_t modelLength;
    Printer *serial;

    // TFLM stuff
    //tflite::ErrorReporter* error_reporter = nullptr;
    //tflite::RecordingMicroInterpreter* interpreter = nullptr;
    //const tflite::Model* tflmModel = nullptr;
    //TfLiteTensor* inputTensor = nullptr;
    //TfLiteTensor* outputTensor = nullptr;
    uint8_t *tensor_arena;
    //int kTensorArenaSize = 1024*38;
    int kTensorArenaSize = 1024*3;
    //sajjad
    tflite::ErrorReporter* error_reporter = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    const tflite::Model* tflmModel = nullptr;
    TfLiteTensor* inputTensor = nullptr;
    TfLiteTensor* outputTensor = nullptr;

};

#endif
