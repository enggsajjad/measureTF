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
#include "Arduino.h"
#include <cmath>

#include "tensorflow/lite/c/c_api_internal.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"

namespace tflite {
namespace ops {
namespace micro {
namespace elementwise {
namespace {

bool IsNumericSupportedType(const TfLiteType type) {
  Serial.println("elementwise.cpp1");
  return type == kTfLiteFloat32;
}

bool IsLogicalSupportedType(const TfLiteType type) {
  Serial.println("elementwise.cpp2");
  return type == kTfLiteBool;
}

typedef bool (*IsSupportedType)(TfLiteType);
template <IsSupportedType>
TfLiteStatus GenericPrepare(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp3");
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 1);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);
  const TfLiteTensor* input = GetInput(context, node, 0);
  TfLiteTensor* output = GetOutput(context, node, 0);
  TF_LITE_ENSURE_EQ(context, input->type, output->type);
  if (!IsSupportedType(input->type)) {
    context->ReportError(context, "Input data type %s (%d) is not supported.",
                         TfLiteTypeGetName(input->type), input->type);
    return kTfLiteError;
  }
  return kTfLiteOk;
}

template <typename T>
inline TfLiteStatus EvalImpl(TfLiteContext* context, TfLiteNode* node,
                             T func(T), TfLiteType expected_type) {
  Serial.println("elementwise.cpp4");
  const TfLiteTensor* input = GetInput(context, node, 0);
  TfLiteTensor* output = GetOutput(context, node, 0);
  TF_LITE_ENSURE_EQ(context, input->type, expected_type);
  const int64_t num_elements = NumElements(input);
  const T* in_data = GetTensorData<T>(input);
  T* out_data = GetTensorData<T>(output);
  for (int64_t i = 0; i < num_elements; ++i) {
    out_data[i] = func(in_data[i]);
  }
  return kTfLiteOk;
}

inline TfLiteStatus EvalNumeric(TfLiteContext* context, TfLiteNode* node,
                                float float_func(float)) {
  Serial.println("elementwise.cpp5");
  return EvalImpl<float>(context, node, float_func, kTfLiteFloat32);
}

inline TfLiteStatus EvalLogical(TfLiteContext* context, TfLiteNode* node,
                                bool bool_func(bool)) {
  Serial.println("elementwise.cpp6");
  return EvalImpl<bool>(context, node, bool_func, kTfLiteBool);
}

TfLiteStatus AbsEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp7");
  return EvalNumeric(context, node, std::abs);
}

TfLiteStatus SinEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp8");
  return EvalNumeric(context, node, std::sin);
}

TfLiteStatus CosEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp9");
  return EvalNumeric(context, node, std::cos);
}

TfLiteStatus LogEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp10");
  return EvalNumeric(context, node, std::log);
}

TfLiteStatus SqrtEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp11");
  return EvalNumeric(context, node, std::sqrt);
}

TfLiteStatus RsqrtEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp12");
  return EvalNumeric(context, node, [](float f) { return 1.f / std::sqrt(f); });
}

TfLiteStatus SquareEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp13");
  return EvalNumeric(context, node, [](float f) { return f * f; });
}

TfLiteStatus LogicalNotEval(TfLiteContext* context, TfLiteNode* node) {
  Serial.println("elementwise.cpp14");
  return EvalLogical(context, node, [](bool v) { return !v; });
}

}  // namespace
}  // namespace elementwise

TfLiteRegistration* Register_ABS() {
  Serial.println("elementwise.cpp15");
  static TfLiteRegistration r = {
      /* init */ nullptr, /* free */ nullptr,
      elementwise::GenericPrepare<elementwise::IsNumericSupportedType>,
      elementwise::AbsEval};
  return &r;
}

TfLiteRegistration* Register_SIN() {
  Serial.println("elementwise.cpp16");
  static TfLiteRegistration r = {
      /* init */ nullptr, /* free */ nullptr,
      elementwise::GenericPrepare<elementwise::IsNumericSupportedType>,
      elementwise::SinEval};
  return &r;
}

TfLiteRegistration* Register_COS() {
  Serial.println("elementwise.cpp17");
  static TfLiteRegistration r = {
      /* init */ nullptr, /* free */ nullptr,
      elementwise::GenericPrepare<elementwise::IsNumericSupportedType>,
      elementwise::CosEval};
  return &r;
}

TfLiteRegistration* Register_LOG() {
  Serial.println("elementwise.cpp18");
  static TfLiteRegistration r = {
      /* init */ nullptr, /* free */ nullptr,
      elementwise::GenericPrepare<elementwise::IsNumericSupportedType>,
      elementwise::LogEval};
  return &r;
}

TfLiteRegistration* Register_SQRT() {
  Serial.println("elementwise.cpp19");
  static TfLiteRegistration r = {
      /* init */ nullptr, /* free */ nullptr,
      elementwise::GenericPrepare<elementwise::IsNumericSupportedType>,
      elementwise::SqrtEval};
  return &r;
}

TfLiteRegistration* Register_RSQRT() {
  Serial.println("elementwise.cpp20");
  static TfLiteRegistration r = {
      /* init */ nullptr, /* free */ nullptr,
      elementwise::GenericPrepare<elementwise::IsNumericSupportedType>,
      elementwise::RsqrtEval};
  return &r;
}

TfLiteRegistration* Register_SQUARE() {
  Serial.println("elementwise.cpp21");
  static TfLiteRegistration r = {
      /* init */ nullptr, /* free */ nullptr,
      elementwise::GenericPrepare<elementwise::IsNumericSupportedType>,
      elementwise::SquareEval};
  return &r;
}

TfLiteRegistration* Register_LOGICAL_NOT() {
  Serial.println("elementwise.cpp21");
  static TfLiteRegistration r = {
      /*init=*/nullptr, /*free=*/nullptr,
      elementwise::GenericPrepare<elementwise::IsLogicalSupportedType>,
      elementwise::LogicalNotEval};
  return &r;
}

}  // namespace micro
}  // namespace ops
}  // namespace tflite
