/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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
#include "Arduino.h"

#include "tensorflow/lite/kernels/internal/reference/floor_mod.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/reference/binary_function.h"
#include "tensorflow/lite/kernels/internal/reference/process_broadcast_shapes.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_utils.h"

// OLD-TODO(b/117523611): We should factor out a binary_op and put binary ops
// there.
namespace tflite {
namespace {

// Input/output tensor index.
constexpr int kInputTensor1 = 0;
constexpr int kInputTensor2 = 1;
constexpr int kOutputTensor = 0;

// OLD-TODO(b/117912880): Support quantization.

TfLiteStatus CalculateOpData(TfLiteContext* context, TfLiteNode* node) {
  //Serial.println("floor_mod.cpp1");
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 2);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  const TfLiteTensor* input1;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kInputTensor1, &input1));
  const TfLiteTensor* input2;
  TF_LITE_ENSURE_OK(context,
                    GetInputSafe(context, node, kInputTensor2, &input2));
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));

  TF_LITE_ENSURE_TYPES_EQ(context, input1->type, input2->type);
  TF_LITE_ENSURE_TYPES_EQ(context, input1->type, output->type);

  return kTfLiteOk;
}

void* Init(TfLiteContext* context, const char* buffer, size_t length) {
  //Serial.println("floor_mod.cpp2");
  return nullptr;
}

TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  //Serial.println("floor_mod.cpp3");
  return CalculateOpData(context, node);
}

template <typename T>
TfLiteStatus EvalFloorMod(TfLiteContext* context, bool requires_broadcast,
                          const TfLiteEvalTensor* input1,
                          const TfLiteEvalTensor* input2,
                          TfLiteEvalTensor* output) {
  //Serial.println("floor_mod.cpp4");
  const T* denominator_data = tflite::micro::GetTensorData<T>(input2);

  if (requires_broadcast) {
    reference_ops::BroadcastBinaryFunction4DSlow<T, T, T>(
        tflite::micro::GetTensorShape(input1),
        tflite::micro::GetTensorData<T>(input1),
        tflite::micro::GetTensorShape(input2), denominator_data,
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<T>(output), reference_ops::FloorMod<T>);
  } else {
    reference_ops::BinaryFunction<T, T, T>(
        tflite::micro::GetTensorShape(input1),
        tflite::micro::GetTensorData<T>(input1),
        tflite::micro::GetTensorShape(input2), denominator_data,
        tflite::micro::GetTensorShape(output),
        tflite::micro::GetTensorData<T>(output), reference_ops::FloorMod<T>);
  }

  return kTfLiteOk;
}

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  //Serial.println("floor_mod.cpp5");
  const TfLiteEvalTensor* input1 =
      tflite::micro::GetEvalInput(context, node, kInputTensor1);
  const TfLiteEvalTensor* input2 =
      tflite::micro::GetEvalInput(context, node, kInputTensor2);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);

  bool requires_broadcast = !tflite::micro::HaveSameShapes(input1, input2);

  switch (input1->type) {
    case kTfLiteFloat32: {
      return EvalFloorMod<float>(context, requires_broadcast, input1, input2,
                                 output);
    }
    default: {
      TF_LITE_KERNEL_LOG(context, "Type '%s' is not supported by FLOOR_MOD.",
                         TfLiteTypeGetName(input1->type));
      return kTfLiteError;
    }
  }
}

}  // namespace

TfLiteRegistration Register_FLOOR_MOD() {
  //Serial.println("floor_mod.cpp6");
  return {/*init=*/Init,
          /*free=*/nullptr,
          /*prepare=*/Prepare,
          /*invoke=*/Eval,
          /*profiling_string=*/nullptr,
          /*builtin_code=*/0,
          /*custom_name=*/nullptr,
          /*version=*/0};
}

}  // namespace tflite
