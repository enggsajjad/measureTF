/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "output_handler.h"



void HandleOutput(tflite::ErrorReporter* error_reporter, float x_value,
                  float y_value) {
    
  // Log the current X and Y values
  TF_LITE_REPORT_ERROR(error_reporter, "x_value: %f, y_value: %f\n",
                       static_cast<double>(x_value),
                       static_cast<double>(y_value));

}
/*
x_value: 1.0587068*2^1, y_value: 1.5957363*2^-1

x_value: 1.0593302*2^1, y_value: 1.5946169*2^-1

x_value: 1.0598989*2^1, y_value: 1.5935956*2^-1

x_value: 1.0604643*2^1, y_value: 1.5925812*2^-1
*/
/*
x_value: 1.734265*2^1, y_value: 1.0*2^-127

x_value: 1.7348275*2^1, y_value: 1.0*2^-127

x_value: 1.735459*2^1, y_value: 1.0*2^-127

x_value: 1.7360072*2^1, y_value: 1.0*2^-127

x_value: 1.7366435*2^1, y_value: 1.0*2^-127
*/