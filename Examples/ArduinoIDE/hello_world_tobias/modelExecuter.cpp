#include "modelExecuter.h"
#include "Arduino.h"

ModelExecuter::ModelExecuter(Stream *serial)
{
  this->serial = new Printer(serial);
  // Keep size fixed for now (near the possible maximum)
  this->tensor_arena = (uint8_t *)malloc(kTensorArenaSize);
  if (this->tensor_arena == NULL)
  {
    Serial.println("Could not get enough memory");
  }
}

void ModelExecuter::initTFLM()
{
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;
}

void ModelExecuter::setModel(const unsigned char *newModel, uint32_t modelLength)
{
  //free((void *)newModel);
  //this->model = newModel;
  this->modelLength = modelLength;
  // Set model here and build interpreter
  this->tflmModel = tflite::GetModel(newModel);
  /*if (this->tflmModel->version() != TFLITE_SCHEMA_VERSION)
  {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.\n",
                         this->tflmModel->version(), TFLITE_SCHEMA_VERSION);
  }
  */
  if (this->tflmModel->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        this->tflmModel->version(), TFLITE_SCHEMA_VERSION);
    //return;
  }
  
  //static tflite::RecordingMicroInterpreter static_interpreter(
  //    this->tflmModel, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::ops::micro::AllOpsResolver resolver;
  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      this->tflmModel, resolver, tensor_arena, kTensorArenaSize, error_reporter);
      
  interpreter = &static_interpreter;
  if (interpreter == NULL)
  {
    serial->printError("Interpreter is null");
  }
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk)
  {
    serial->printError("Allocation of tensors failed");
    return;
  }
  // Grab the heap statistics
  mbed_stats_heap_t heap_stats;
  mbed_stats_heap_get(&heap_stats);

  // Log memory stuff
  serial->println(MEMORY, "HEAP_CURRENT_SIZE", heap_stats.current_size);
  serial->println(MEMORY, "HEAP_RESERVED_SIZE", heap_stats.reserved_size);
  //serial->println(MEMORY, "kTensorAreaUsed", interpreter->arena_used_bytes());

  inputTensor = interpreter->input(0);
  outputTensor = interpreter->output(0);
  serial->printStatus("Got Model");
}

const unsigned char *ModelExecuter::getModel()
{
  return this->model;
}

uint32_t ModelExecuter::getModelLength()
{
  return this->modelLength;
}

uint32_t ModelExecuter::getUsedMemory()
{
  //return interpreter->arena_used_bytes();
}

int ModelExecuter::execute()
{

  TfLiteTensor *input = interpreter->input(0);
  //int8_t *input_data = (int8_t *)malloc(input->bytes);
  //int8_t *input_data2 = (int8_t *)malloc(input->bytes);
  //for (int i = 0; i < input->bytes; i++)
  //{
  //  input_data[i] = (uint8_t) i;
  //}

  // memcpy(input->data.int8, input_data, input->bytes);
  //input->data.int8 = input_data;

  //noInterrupts();
  if (kTfLiteOk != interpreter->Invoke())
  {
    //interrupts();
    return -1;
  }
  //interpreter->GetMicroAllocator().PrintAllocations();//sajjad
  //interrupts();
  return 0;
}

int ModelExecuter::execute2()
{

  float position = static_cast<float>(inference_count) /
                   static_cast<float>(kInferencesPerCycle);
  float x_val = position * kXrange;

   inputTensor->data.f[0] = x_val;


  if (kTfLiteOk != interpreter->Invoke())
  {
    //interrupts();
    return -1;
  }
  float y_val = outputTensor->data.f[0];

  //HandleOutput(error_reporter, x_val, y_val);

  // Do this only once
  if (!initialized) {
    // Set the LED pin to output
    pinMode(led, OUTPUT);
    initialized = true;
  }


  int brightness = (int)(127.5f * (y_val + 1));

  analogWrite(led, brightness);

  //error_reporter->Report("%d\n", brightness);
  error_reporter->Report("%d ", brightness);

  inference_count += 1;
  if (inference_count >= kInferencesPerCycle) inference_count = 0;
  //Serial.println("====end===");

  return 0;
}

/*
void ModelExecuter::execute(float *input, float *output, int inputLength, int outputSize)
{
  
  for (int i = 0; i < inputLength; i++)
  {
    int8_t x_quantized = input[i] / inputTensor->params.scale + inputTensor->params.zero_point;
    inputTensor->data.int8[i] = x_quantized;
  }

  if (kTfLiteOk != interpreter->Invoke())
  {
    // TODO: Deal with this error
  }

  for (int i = 0; i < outputSize; i++)
  {
    output[i] = (outputTensor->data.int8[0] - outputTensor->params.zero_point) * outputTensor->params.scale;
  }
  //interpreter->GetMicroAllocator().PrintAllocations();

}
*/
