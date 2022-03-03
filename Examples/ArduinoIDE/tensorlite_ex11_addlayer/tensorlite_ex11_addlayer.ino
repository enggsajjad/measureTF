//Added a fourth layer to see the variations in layers in the code, arduino lib


// Import TensorFlow stuff
#include "TensorFlowLite.h"
#include "tensorflow/lite/experimental/micro/kernels/micro_ops.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h" //sajjad

//C:\Users\Sajjad\Documents\Arduino\libraries\Arduino_TensorFlowLite\src\tensorflow\lite\experimental\microfrontend\lib
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
//#include "tensorflow/lite/experimental/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/version.h"

// Our model
#include "sine_model.h"

// Figure out what's going on in our model
#define DEBUG 1

// Some settings
constexpr int led_pin = LED_BUILTIN;//2;
constexpr float pi = 3.14159265;                  // Some pi
constexpr float freq = 0.5;                       // Frequency (Hz) of sinewave
constexpr float period = (1 / freq) * (1000000);  // Period (microseconds)

// TFLite globals, used for compatibility with Arduino-style sketches
namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;
  TfLiteTensor* model_output = nullptr;
  int inference_count = 0;
  
  // Create an area of memory to use for input, output, and other TensorFlow
  // arrays. You'll need to adjust this by combiling, running, and looking
  // for errors.
  constexpr int kTensorArenaSize = 2 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setup() {
  delay(5000);
  Serial.begin(115200);
  // Wait for Serial to connect
  #if DEBUG
  while(!Serial);
  #endif
  Serial.println("Starting... ");
  
  // Let's make an LED vary in brightness
  pinMode(led_pin, OUTPUT);

  // Set up logging (will report to Serial, even within TFLite functions)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure
  //model = tflite::GetModel(sine_model);
  //model = tflite::GetModel(sine_model1);//working from hellow_world
  //model = tflite::GetModel(sine_model2);
  //model = tflite::GetModel(sine_model3);//from sajjad google colab, works //self generated from google colab using 3 layers tflite_sinewave_training.ipynb
  model = tflite::GetModel(sine_model4);//from sajjad google colab, works //self generated from google colab using 4 layers tflite_sinewave_training_addlayer.ipynb

  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report("Model version does not match Schema");
    //while(1);
    return;
  }

  // Pull in only needed operations (should match NN layers)
  // Available ops:
  //  https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/kernels/micro_ops.h
  //static tflite::ops::micro::AllOpsResolver micro_mutable_op_resolver;//this also works
  static tflite::MicroMutableOpResolver micro_mutable_op_resolver; //sajjad
  micro_mutable_op_resolver.AddBuiltin(
    tflite::BuiltinOperator_FULLY_CONNECTED,
    tflite::ops::micro::Register_FULLY_CONNECTED(),
    1, 3);

  // Build an interpreter to run the model
  static tflite::MicroInterpreter static_interpreter(
    model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize,
    error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    //while(1);
    return;
  }

  // Assign model input and output buffers (tensors) to pointers
  model_input = interpreter->input(0);
  model_output = interpreter->output(0);

  // Get information about the memory area to use for the model's input
  // Supported data types:
  // https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/c/common.h#L226
  #if DEBUG
  Serial.print("Number of dimensions: ");
  Serial.println(model_input->dims->size);
  Serial.print("Dim 1 size: ");
  Serial.println(model_input->dims->data[0]);
  Serial.print("Dim 2 size: ");
  Serial.println(model_input->dims->data[1]);
  Serial.print("Input type: ");
  Serial.println(model_input->type);
  #endif
  inference_count = 0;
  Serial.println("===start===");

}

  const int kInferencesPerCycle = 10000;
  const float kXrange = 2.f * 3.14159265359f;

void loop() 
{

  #if DEBUG
  unsigned long start_timestamp = micros();
  #endif

  // Get current timestamp and modulo with period
  unsigned long timestamp = micros();
  timestamp = timestamp % (unsigned long)period;
  
  // Calculate x value to feed to the model
  float x_val = ((float)timestamp * 2 * pi) / period;
    float position = static_cast<float>(inference_count) /
                   static_cast<float>(kInferencesPerCycle);
  //float x_val = position * kXrange;
  

  // Copy value to input buffer (tensor)
  model_input->data.f[0] = x_val;

  // Run inference
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed on input: %f\n", x_val); 
    return;
  }

  // Read predicted y value from output buffer (tensor)
  float y_val = model_output->data.f[0];

  // Translate to a PWM LED brightness
  int brightness = (int)(255 * y_val);
  analogWrite(led_pin, brightness);

  // Print value
  //Serial.println(y_val);
  error_reporter->Report("%d\n", brightness);

  inference_count += 1;
  if (inference_count >= 1000000) inference_count = 0;
  
  #if DEBUG
  Serial.print("Time for inference (us): ");
  Serial.println(micros() - start_timestamp);
  #endif
  Serial.println("===end===");

}
