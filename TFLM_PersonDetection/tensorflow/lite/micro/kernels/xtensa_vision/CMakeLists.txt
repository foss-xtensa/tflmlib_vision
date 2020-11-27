#
# Copyright (c) 2015-2016 Tensilica Inc. ALL RIGHTS RESERVED.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Build script for CNN runtime library

cmake_minimum_required(VERSION 3.0.0)

project(cnnrt)

set(ENABLE_C99 TRUE)
set(NEED_LIBXICNN TRUE)

include(../cmake/setup-args.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IPA_FLAG}")

if(DEFINED XI_ERROR_LEVEL)
  add_definitions(-DXI_ERROR_LEVEL=${XI_ERROR_LEVEL})
endif()
if(DEFINED CNNRT_PERF_LEVEL)
  add_definitions(-DCNNRT_PERF_LEVEL=${CNNRT_PERF_LEVEL})
endif()
if(DEFINED NO_REF_TABLES)
add_definitions(-DNO_REF_TABLES=${NO_REF_TABLES})
endif()

include_directories(${LIBXICNN_DIR}/libxi/include)
include_directories(${LIBXICNN_DIR}/libxi_ref/include)
include_directories(${LIBXICNN_DIR}/../xicnne/libxi/include)
include_directories(${LIBXICNN_DIR}/../xicnne/libxi_ref/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libshared/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if("${XNNE_VERSION}" STREQUAL "2")
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/firmware.S)
    set(asm_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/firmware.S)
  endif()
else()
  set(asm_SOURCES )
endif()

add_library(cnnrt
  src/cnnrt.c
  src/cnnrt_dna100.c
  src/cnnarena.c
  src/cnnlsp.c
  src/cnnrtcompression.c
  src/cnnrtcompression_support.c
  src/cnndebug.c
  src/cnnperf.c
  src/cnncmp.c
  src/cnnrtxmp.c
  ${asm_SOURCES}
  include/cnnrt.h
  include/cnnrt_compression.h
  include/cnnrt_compression_support.h
  include/cnnrt_debug.h
  include/cnnrt_perf.h
  include/cnnrt_xi.h
  include/cnnrt_xi_p6.h
  include/cnnrt_xi_dna100.h
  include/cnnrt_xi_q7.h
  include/cnnrt_xmp.h)

# Compile XI as C++ if building with CSTUBs
if(DEFINED CSTUB_PATH)
  set_source_files_properties(src/cnnrtcompression.c PROPERTIES LANGUAGE CXX)
else()
  # Make sure C compiler is used for .S files
  set_source_files_properties(${asm_SOURCES} PROPERTIES LANGUAGE C)
endif()

install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS cnnrt DESTINATION lib)
