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
#include "tensorflow/lite/kernels/internal/reference/pooling.h"
#include "Arduino.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/pooling.h"
#include "C:\Users\Sajjad\Documents\Arduino\myExample\nano33sense\hello_world_tobias1\tflmTimer.h"

namespace tflite
{

  namespace
  {

    TfLiteStatus AverageEval(TfLiteContext *context, TfLiteNode *node)
    {
      Serial.println("pooling.cpp1");
      TflmTimer::measureTime(CMSIS_AVG_POOLING);
      TFLITE_DCHECK(node->builtin_data != nullptr);
      auto *params = reinterpret_cast<TfLitePoolParams *>(node->builtin_data);

      TFLITE_DCHECK(node->user_data != nullptr);
      const OpDataPooling *data =
          static_cast<const OpDataPooling *>(node->user_data);

      const TfLiteEvalTensor *input =
          micro::GetEvalInput(context, node, kPoolingInputTensor);
      TfLiteEvalTensor *output =
          micro::GetEvalOutput(context, node, kPoolingOutputTensor);

      // Inputs and outputs share the same type, guaranteed by the converter.
      switch (input->type)
      {
      case kTfLiteFloat32:
        AveragePoolingEvalFloat(context, node, params, data, input, output);
        break;
      case kTfLiteInt8:
        AveragePoolingEvalQuantized(context, node, params, data, input, output);
        break;
      default:
        TF_LITE_KERNEL_LOG(context, "Input type %s is not currently supported",
                           TfLiteTypeGetName(input->type));
        TflmTimer::measureTime(CMSIS_AVG_POOLING);
        return kTfLiteError;
      }
      TflmTimer::measureTime(CMSIS_AVG_POOLING);
      return kTfLiteOk;
    }

    TfLiteStatus MaxEval(TfLiteContext *context, TfLiteNode *node)
    {
      Serial.println("pooling.cpp2");
      TflmTimer::measureTime(CMSIS_MAX_POOLING);
      TFLITE_DCHECK(node->builtin_data != nullptr);
      auto *params = reinterpret_cast<TfLitePoolParams *>(node->builtin_data);

      TFLITE_DCHECK(node->user_data != nullptr);
      const OpDataPooling *data =
          static_cast<const OpDataPooling *>(node->user_data);

      const TfLiteEvalTensor *input =
          micro::GetEvalInput(context, node, kPoolingInputTensor);
      TfLiteEvalTensor *output =
          micro::GetEvalOutput(context, node, kPoolingOutputTensor);

      switch (input->type)
      {
      case kTfLiteFloat32:
        MaxPoolingEvalFloat(context, node, params, data, input, output);
        break;
      case kTfLiteInt8:
        MaxPoolingEvalQuantized(context, node, params, data, input, output);
        break;
      default:
        TF_LITE_KERNEL_LOG(context, "Type %s not currently supported.",
                           TfLiteTypeGetName(input->type));
        TflmTimer::measureTime(CMSIS_MAX_POOLING);
        return kTfLiteError;
      }
      TflmTimer::measureTime(CMSIS_MAX_POOLING);
      return kTfLiteOk;
    }

    void *Init(TfLiteContext *context, const char *buffer, size_t length)
    {
      Serial.println("pooling.cpp2");
      TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
      return context->AllocatePersistentBuffer(context, sizeof(OpDataPooling));
    }

  } // namespace

  TfLiteRegistration Register_AVERAGE_POOL_2D()
  {
    Serial.println("pooling.cpp3");
    return {/*init=*/Init,
            /*free=*/nullptr,
            /*prepare=*/PoolingPrepare,
            /*invoke=*/AverageEval,
            /*profiling_string=*/nullptr,
            /*builtin_code=*/0,
            /*custom_name=*/nullptr,
            /*version=*/0};
  }

  TfLiteRegistration Register_MAX_POOL_2D()
  {
    Serial.println("pooling.cpp4");
    return {/*init=*/Init,
            /*free=*/nullptr,
            /*prepare=*/PoolingPrepare,
            /*invoke=*/MaxEval,
            /*profiling_string=*/nullptr,
            /*builtin_code=*/0,
            /*custom_name=*/nullptr,
            /*version=*/0};
  }

} // namespace tflite
