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

#include "tensorflow/lite/micro/examples/person_detection/main_functions.h"
#include "tensorflow/lite/micro/kernels/xtensa_vision/utils.h"

#if defined(__XTENSA__)
#include <xtensa/xt_profiling.h>
#endif

// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
int main(int argc, char* argv[]) {
  USER_DEFINED_HOOKS_STOP();
  int indx;
  int framesToProcess = 2;
  setup();

#if (ENABLE_OUTPUT_DUMP_FILES)
  framesToProcess = 1;
#endif

  for(indx=0; indx < framesToProcess; indx++){
    loop();
  }

  return 0;
}
