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
#include "Arduino.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_utils.h"

namespace tflite {
namespace {

constexpr int kInputTensor = 0;
constexpr int kAxisTensor = 1;
constexpr int kOutputTensor = 0;

TfLiteStatus GetAxisValueFromTensor(TfLiteContext* context,
                                    const TfLiteTensor* axis,
                                    int32_t* axis_value) {
  Serial.println("expand_dims.cpp1");
  const int axis_dims = (tflite::GetTensorShape(axis)).DimensionsCount();
  if (axis_dims > 1) {
    TF_LITE_KERNEL_LOG(context, "Axis has only one element for Expand_Dims.",
                       axis_dims);
    return kTfLiteError;
  }

  if (kTfLiteInt32 == (axis->type)) {
    const int32_t* axis_ptr = tflite::GetTensorData<int32_t>(axis);
    *axis_value = axis_ptr[0];
    return kTfLiteOk;
  } else {
    TF_LITE_KERNEL_LOG(context,
                       "Axis type %s (%d) not supported by Expand_Dims.",
                       TfLiteTypeGetName(axis->type), axis->type);
    return kTfLiteError;
  }
}

// Verifies that the output tensor's dimension shape is equivalent to inserting
// a dimension of length 1 at the dimension index axis of input's shape as
// defined in https://www.tensorflow.org/api_docs/python/tf/expand_dims.
TfLiteStatus VerifyTensorDim(TfLiteContext* context, const TfLiteTensor* input,
                             const TfLiteTensor* axis_tensor,
                             const TfLiteTensor* output) {
  Serial.println("expand_dims.cpp2");
  int32_t axis_value = 0;
  TF_LITE_ENSURE_OK(context,
                    GetAxisValueFromTensor(context, axis_tensor, &axis_value));

  tflite::RuntimeShape input_shape = tflite::GetTensorShape(input);
  if (axis_value < 0) {
    axis_value = input_shape.DimensionsCount() + 1 + axis_value;
  }
  TF_LITE_ENSURE(context, axis_value <= input_shape.DimensionsCount());

  // TFLM only supports fixed dimension tensor and assumes that the output shape
  // is fully specified in the model. As such, TFLM directly use the pointer to
  // the dimension array in the model buffer.
  tflite::RuntimeShape output_shape = tflite::GetTensorShape(output);

  TF_LITE_ENSURE(context, output_shape.DimensionsCount() ==
                              input_shape.DimensionsCount() + 1);
  for (int i = 0; i < output_shape.DimensionsCount(); ++i) {
    if (i < axis_value) {
      TF_LITE_ENSURE(context, output_shape.Dims(i) == input_shape.Dims(i));
    } else if (i == axis_value) {
      TF_LITE_ENSURE(context, output_shape.Dims(i) == 1);
    } else {
      TF_LITE_ENSURE(context, output_shape.Dims(i) == input_shape.Dims(i - 1));
    }
  }
  return kTfLiteOk;
}

TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("expand_dims.cpp3");
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 2);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);
  const TfLiteTensor* input;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kInputTensor, &input));
  const TfLiteTensor* axis;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, kAxisTensor, &axis));
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context,
                    GetOutputSafe(context, node, kOutputTensor, &output));
  output->type = input->type;
  if (IsDynamicTensor(axis)) {
    TF_LITE_KERNEL_LOG(context,
                       "DynamicTensor is not yet supported by Expand_Dims.");
    return kTfLiteError;
  }
  return VerifyTensorDim(context, input, axis, output);
}

template <typename T>
void memCopyN(T* out, const T* in, const int num_elements) {
  Serial.println("expand_dims.cpp4");
  for (int i = 0; i < num_elements; ++i) {
    out[i] = in[i];
  }
}

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("expand_dims.cpp5");
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);
  const int flat_size = ElementCount(*input->dims);

  switch (input->type) {
    case kTfLiteFloat32: {
      memCopyN(tflite::micro::GetTensorData<float>(output),
               tflite::micro::GetTensorData<float>(input), flat_size);
    } break;
    case kTfLiteInt8: {
      memCopyN(tflite::micro::GetTensorData<int8_t>(output),
               tflite::micro::GetTensorData<int8_t>(input), flat_size);
    } break;
    default:
      TF_LITE_KERNEL_LOG(
          context,
          "Expand_Dims only currently supports int8 and float32, got %d.",
          input->type);
      return kTfLiteError;
  }
  return kTfLiteOk;
}
}  // namespace

TfLiteRegistration Register_EXPAND_DIMS() {
  Serial.println("expand_dims.cpp6");
  return {/*init=*/nullptr,
          /*free=*/nullptr,
          /*prepare=*/Prepare,
          /*invoke=*/Eval,
          /*profiling_string=*/nullptr,
          /*builtin_code=*/0,
          /*custom_name=*/nullptr,
          /*version=*/0};
}

}  // namespace tflite