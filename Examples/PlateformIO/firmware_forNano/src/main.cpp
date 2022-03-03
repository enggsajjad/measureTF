#include "Arduino.h"
#include "controls.h"
#include "modelExecuter.h"
#include "tflmTimer.h"
#include "model_gen.h"
#include "printer.h"

// #define SERIAL_MODE

int modelLength = 0;
int inputLength = 0;

float dummyModelOutput[] = {1.5, 2.5, 3.5};

float *inputBuffer;

Printer *printer;

ModelExecuter *exec = new ModelExecuter(&Serial);
void setup()
{
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
  TflmTimer::startTimingService(&Serial);
  
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
}

//void loop()
//{
//}

void loop() {
  exec->execute2();
  }
