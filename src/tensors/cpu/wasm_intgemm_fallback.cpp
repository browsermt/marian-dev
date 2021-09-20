/** A fallback (non-optimized) implementation of "wasm_gemm_interface.h" interface for integer
 * matrix multiplication for wasm target.
 *
 * This implementation is built and exported from the main module and can serve as a polyfill
 * (fallback) for browsers that don't support an optimized implementation of
 * "wasm_gemm_interface.h".
 */

#if defined(WASM)

#include "3rd_party/intgemm/intgemm/intgemm.h"
#include "common/logging.h"
#include "wasm_intgemm_interface.h"

extern "C" void int8PrepareAFallback(const float* input_A,
                                     float scale,
                                     float zero_point,
                                     Index rows_A,
                                     Index width,
                                     int8_t* output) {
  LOG(info, "Calling fallback implementation of interface \"int8PrepareA\"");
  // ToDo: Add fallback implementation
}

extern "C" void int8PrepareBFallback(const float* input_B,
                  float scale,
                  float zero_point,
                  Index width,
                  Index cols_B,
                  int8_t* output) {
  LOG(info, "Calling fallback implementation of interface \"int8PrepareB\"");
  // ToDo: Add fallback implementation
}

extern "C" void int8PrepareBFromTransposedFallback(const float* input_B_transposed,
                                float scale,
                                float zero_point,
                                Index width,
                                Index cols_B,
                                int8_t* output) {
  LOG(info, "Calling fallback implementation of interface \"int8PrepareBFromTransposed\"");
  // ToDo: Add fallback implementation
}

extern "C" void int8PrepareBFromQuantizedTransposedFallback(const int8_t* input_B_quant_transposed,
                                         Index width,
                                         Index cols_B,
                                         int8_t* output) {
  LOG(info, "Calling fallback implementation of interface \"int8PrepareBFromQuantizedTransposed\"");
  // ToDo: Add fallback implementation
}

extern "C" void int8PrepareBiasFallback(const int8_t* input_B_prepared,
                     float scale,
                     float zero_point,
                     Index width,
                     Index cols_B,
                     const float* input_bias,
                     float* output) {
  LOG(info, "Calling fallback implementation of interface \"int8PrepareBias\"");
  // ToDo: Add fallback implementation
}

extern "C" void int8MultiplyAndAddBiasFallback(const int8_t* input_A_prepared,
                            float scale_A,
                            float zero_point_A,
                            const int8_t* input_B_prepared,
                            float scale_B,
                            float zero_point_B,
                            const float* input_bias_prepared,
                            Index rows_A,
                            Index width,
                            Index cols_B,
                            float* output) {
  LOG(info, "Calling fallback implementation of interface \"int8MultiplyAndAddBias\"");
  // ToDo: Add fallback implementation
}

extern "C" void int8SelectColumnsOfBFallback(const int8_t* input_B_prepared,
                          Index width,
                          Index cols_B,
                          const Index* cols,
                          const Index num_cols,
                          int8_t* output) {
  LOG(info, "Calling fallback implementation of interface \"int8SelectColumnsOfB\"");
  // ToDo: Add fallback implementation
}

#endif  // WASM
