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

#ifndef TENSORFLOW_LITE_MICRO_KERNELS_XTENSA_XTENSA_POOLING_H_
#define TENSORFLOW_LITE_MICRO_KERNELS_XTENSA_XTENSA_POOLING_H_

#include <cstdint>

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/pooling.h"

namespace tflite {

  struct XtensaOpDataPooling {
  OpDataPooling reference_op_data;
#if defined (VISIONP6)
  uint8_t* pContext; // persistent lib context for this instance saved here
  uint32_t contextSize;
#endif // VISIONP6
};

#if defined(VISIONP6)

TfLiteStatus AveragePoolingPrepareVision(TfLiteContext* context, TfLiteNode* node);

TfLiteStatus AveragePoolingEvalQuantizedVision(TfLiteContext* context, TfLiteNode* node);

#endif

}  // namespace tflite

#endif  // TENSORFLOW_LITE_MICRO_KERNELS_XTENSA_XTENSA_POOLING_H_
