/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef NN_H
#define NN_H

#include <stdint.h>

#include "wasi_nn.h"

#define MAX_MODEL_SIZE    85000000
#define INPUT_TENSOR_DIMS 4

// WASI-NN wrappers

error wasm_load(char *model_name, graph *g, execution_target target);
error wasm_init_execution_context(graph g, graph_execution_context *ctx);
error wasm_set_input(graph_execution_context ctx, uint8_t *input_tensor,
                     uint32_t *dim);
error wasm_compute(graph_execution_context ctx);
error wasm_get_output(graph_execution_context ctx, uint32_t index,
                      float *out_tensor, uint32_t *out_size);

#endif
