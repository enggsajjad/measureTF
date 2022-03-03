/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

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
#include "tensorflow/lite/micro/kernels/logical.h"
#include "Arduino.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/reference/binary_function.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"

namespace tflite {
namespace {

TfLiteStatus LogicalOrEval(TfLiteContext* context, TfLiteNode* node) {
  //Serial.println("logical.cpp1");
  return LogicalImpl(context, node, LogicalOr);
}

TfLiteStatus LogicalAndEval(TfLiteContext* context, TfLiteNode* node) {
  //Serial.println("logical.cpp2");
  return LogicalImpl(context, node, LogicalAnd);
}

}  // namespace

TfLiteRegistration Register_LOGICAL_OR() {
  //Serial.println("logical.cpp3");
  // Init, Free, Prepare, Eval are satisfying the Interface required by
  // TfLiteRegistration.
  return {/*init=*/nullptr,
          /*free=*/nullptr,
          /*prepare=*/nullptr,
          /*invoke=*/LogicalOrEval,
          /*profiling_string=*/nullptr,
          /*builtin_code=*/0,
          /*custom_name=*/nullptr,
          /*version=*/0};
}

TfLiteRegistration Register_LOGICAL_AND() {
  //Serial.println("logical.cpp4");
  // Init, Free, Prepare, Eval are satisfying the Interface required by
  // TfLiteRegistration.
  return {/*init=*/nullptr,
          /*free=*/nullptr,
          /*prepare=*/nullptr,
          /*invoke=*/LogicalAndEval,
          /*profiling_string=*/nullptr,
          /*builtin_code=*/0,
          /*custom_name=*/nullptr,
          /*version=*/0};
}

}  // namespace tflite
