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

#include "tensorflow/lite/micro/kernels/softmax.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/softmax.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "C:\Users\Sajjad\Documents\Arduino\myExample\nano33sense\hello_world_tobias1\tflmTimer.h"

namespace tflite
{
  namespace
  {

    void SoftmaxQuantized(const TfLiteEvalTensor *input, TfLiteEvalTensor *output,
                          const SoftmaxParams &op_data)
    {
      Serial.println("softmax.cpp1");
      if (input->type == kTfLiteInt8)
      {
        if (output->type == kTfLiteInt16)
        {
          tflite::reference_ops::Softmax(
              op_data, tflite::micro::GetTensorShape(input),
              tflite::micro::GetTensorData<int8_t>(input),
              tflite::micro::GetTensorShape(output),
              tflite::micro::GetTensorData<int16_t>(output));
        }
        else
        {
          tflite::reference_ops::Softmax(
              op_data, tflite::micro::GetTensorShape(input),
              tflite::micro::GetTensorData<int8_t>(input),
              tflite::micro::GetTensorShape(output),
              tflite::micro::GetTensorData<int8_t>(output));
        }
      }
      else
      {
        tflite::reference_ops::SoftmaxInt16(
            op_data, tflite::micro::GetTensorShape(input),
            tflite::micro::GetTensorData<int16_t>(input),
            tflite::micro::GetTensorShape(output),
            tflite::micro::GetTensorData<int16_t>(output));
      }
    }

    TfLiteStatus SoftmaxEval(TfLiteContext *context, TfLiteNode *node)
    {
      Serial.println("softmax.cpp2");
      TflmTimer::measureTime(CMSIS_SOFTMAX);
      const TfLiteEvalTensor *input = tflite::micro::GetEvalInput(context, node, 0);
      TfLiteEvalTensor *output = tflite::micro::GetEvalOutput(context, node, 0);

      TFLITE_DCHECK(node->user_data != nullptr);
      SoftmaxParams op_data = *static_cast<SoftmaxParams *>(node->user_data);

      switch (input->type)
      {
      case kTfLiteFloat32:
      {
        tflite::reference_ops::Softmax(
            op_data, tflite::micro::GetTensorShape(input),
            tflite::micro::GetTensorData<float>(input),
            tflite::micro::GetTensorShape(output),
            tflite::micro::GetTensorData<float>(output));
        TflmTimer::measureTime(CMSIS_SOFTMAX);
        return kTfLiteOk;
      }
      case kTfLiteInt8:
      case kTfLiteInt16:
      {
        SoftmaxQuantized(input, output, op_data);
        TflmTimer::measureTime(CMSIS_SOFTMAX);
        return kTfLiteOk;
      }
      default:
        TF_LITE_KERNEL_LOG(context, "Type %s (%d) not supported.",
                           TfLiteTypeGetName(input->type), input->type);
        TflmTimer::measureTime(CMSIS_SOFTMAX);
        return kTfLiteError;
      }
    }
  } // namespace

  TfLiteRegistration Register_SOFTMAX()
  {
    Serial.println("softmax.cpp3");
    return {/*init=*/SoftmaxInit,
            /*free=*/nullptr,
            /*prepare=*/SoftmaxPrepare,
            /*invoke=*/SoftmaxEval,
            /*profiling_string=*/nullptr,
            /*builtin_code=*/0,
            /*custom_name=*/nullptr,
            /*version=*/0};
  }

} // namespace tflite