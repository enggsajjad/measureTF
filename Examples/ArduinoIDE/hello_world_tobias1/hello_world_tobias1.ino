//Using Tobias Lib and Tobias Class/Model structures
#include "Arduino.h"
#include "controls.h"
#include "modelExecuter.h"
#include "tflmTimer.h"
//#include "model_gen.h"
#include "sine_model_data.h"
#include "printer.h"


Printer *printer;

ModelExecuter *exec = new ModelExecuter(&Serial);
void setup()
{delay(1000);
  Serial.begin(115200);
  
  pinMode(exec->led, OUTPUT);
  delay(1000);
  digitalWrite(exec->led, HIGH);
  delay(1000);
  digitalWrite(exec->led, LOW);
  printer = new Printer(&Serial);

  while (!Serial)
  {
  }
    // Test code
  Serial.println("dummy_line_prevent_strange_characters");
  Serial.println("---start---");
  //TflmTimer::startTimingService(&Serial);
  
  exec->initTFLM();
  //exec->setModel(model_gen, model_gen_len);
  exec->setModel(g_sine_model_data, g_sine_model_data_len);
  //TflmTimer::measureTime(1);
  //exec->execute();
  //TflmTimer::measureTime(1);

  // print the time*/
  //TflmTimer::printMeasurements(printer);
  // This needs to stay to indicate the end of the transmission
  Serial.println("---Complete---");
  Serial.println("");
/*
TflmTimer::startTimingService(&Serial);
delay(50);
TflmTimer::measureTime(1);
delay(50);
TflmTimer::measureTime(1);
delay(50);
TflmTimer::measureTime(2);
TflmTimer::startTimingService(&Serial);
delay(50);
TflmTimer::measureTime(3);
delay(50);
TflmTimer::printMeasurements(printer);

  uint32_t start;
uint32_t stop;
uint32_t elapsed;

// enable DWT
CoreDebug->DEMCR |= 0x01000000;

// Reset cycle counter
DWT->CYCCNT = 0;

// enable cycle counter
DWT->CTRL |= 0x1;

start = DWT->CYCCNT;
delay(50);
stop = DWT->CYCCNT;
elapsed = stop-start;
Serial.print("cycles for delay(1000) ");
Serial.println(elapsed);
// <info> app: cycles for nrf_delay_ms(1000) = 64018002
//address of the register
volatile unsigned int *DWT_CYCCNT   = (volatile unsigned int *)0xE0001004;     

//address of the register
volatile unsigned int *DWT_CONTROL  = (volatile unsigned int *)0xE0001000;     

//address of the register
volatile unsigned int *DWT_LAR      = (volatile unsigned int *)0xE0001FB0;     

//address of the register
volatile unsigned int *SCB_DEMCR    = (volatile unsigned int *)0xE000EDFC;


*DWT_LAR = 0xC5ACCE55; // unlock (CM7)
*SCB_DEMCR |= 0x01000000;
*DWT_CYCCNT = 0; // reset the counter
*DWT_CONTROL |= 1 ; // enable the counter

uint32_t x = *DWT_CYCCNT;
delay(50);
uint32_t y = *DWT_CYCCNT;
x = (y - x); // Elapsed clock ticks, at SystemCoreClock
Serial.print("cycles for delay(1000) ");
Serial.println(x);
*/
}


void loop() {
  exec->execute2();
  Serial.println("---end---");
  Serial.println(""); 
}
/*
#include <TensorFlowLite.h>

#include "main_functions.h"

#include "constants.h"
#include "output_handler.h"
#include "sine_model_data.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 2 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.
void setup() {
delay(10000);
Serial.begin(9600);
Serial.println("Starting...");
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_sine_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::ops::micro::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  inference_count = 0;
  Serial.println("====start===");
}

// The name of this function is important for Arduino compatibility.
void loop() {
  // Calculate an x value to feed into the model. We compare the current
  // inference_count to the number of inferences per cycle to determine
  // our position within the range of possible x values the model was
  // trained on, and use this to calculate a value.
  float position = static_cast<float>(inference_count) /
                   static_cast<float>(kInferencesPerCycle);
  float x_val = position * kXrange;

  // Place our calculated x value in the model's input tensor
  input->data.f[0] = x_val;

  // Run inference, and report any error
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed on x_val: %f\n",
                           static_cast<double>(x_val));
    return;
  }

  // Read the predicted y value from the model's output tensor
  float y_val = output->data.f[0];


  
  // Output the results. A custom HandleOutput function can be implemented
  // for each supported hardware target.
  HandleOutput(error_reporter, x_val, y_val);

  // Increment the inference_counter, and reset it if we have reached
  // the total number per cycle
  inference_count += 1;
  if (inference_count >= kInferencesPerCycle) inference_count = 0;
  Serial.println("====end===");
}
*/
