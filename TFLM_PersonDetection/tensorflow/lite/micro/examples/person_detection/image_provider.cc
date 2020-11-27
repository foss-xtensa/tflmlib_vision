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
#include "tensorflow/lite/micro/examples/person_detection/person_image_data.h"
#include "tensorflow/lite/micro/examples/person_detection/no_person_image_data.h"
#include "tensorflow/lite/micro/examples/person_detection/image_provider.h"
#include "tensorflow/lite/micro/examples/person_detection/model_settings.h"

static int toggleImage = 0;
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, uint8_t* image_data) {
  const uint8_t *person_data;
  if(toggleImage++ % 2)
	  person_data = g_person_data;
  else
	  person_data = g_no_person_data;

  for (int i = 0; i < image_width * image_height * channels ; ++i) {
		image_data[i] = person_data[i];
  }
  return kTfLiteOk;
}
