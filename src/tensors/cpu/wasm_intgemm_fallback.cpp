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
  LOG(info, "Calling fallback implementation of \"int8PrepareA\"");
  intgemm::Int8Shift::PrepareA(input_A,
                               output,
                               scale, /*Quant Mult*/
                               rows_A,
                               width);
}

extern "C" void int8PrepareBFallback(const float* input_B,
                                     float scale,
                                     float zero_point,
                                     Index width,
                                     Index cols_B,
                                     int8_t* output) {
  LOG(info, "Calling fallback implementation of \"int8PrepareB\"");
  intgemm::Int8::PrepareB(input_B,
                          output,
                          scale, /*Quant Mult*/
                          width,
                          cols_B);
}

extern "C" void int8PrepareBFromTransposedFallback(const float* input_B_transposed,
                                                   float scale,
                                                   float zero_point,
                                                   Index width,
                                                   Index cols_B,
                                                   int8_t* output) {
  LOG(info, "Calling fallback implementation of \"int8PrepareBFromTransposed\"");
  ABORT("Unimplemented int8PrepareBFromTransposedFallback");
}

extern "C" void int8PrepareBFromQuantizedTransposedFallback(const int8_t* input_B_quant_transposed,
                                                            Index width,
                                                            Index cols_B,
                                                            int8_t* output) {
  LOG(info, "Calling fallback implementation of \"int8PrepareBFromQuantizedTransposed\"");
  intgemm::Int8::PrepareBQuantizedTransposed(input_B_quant_transposed, output, width, cols_B);
}

extern "C" void int8PrepareBiasFallback(const int8_t* input_B_prepared,
                                        float scale,
                                        float zero_point,
                                        Index width,
                                        Index cols_B,
                                        const float* input_bias,
                                        float* output) {
  LOG(info, "Calling fallback implementation of \"int8PrepareBias\"");
  intgemm::Int8Shift::PrepareBias(
      input_B_prepared,
      width,
      cols_B,
      intgemm::callbacks::UnquantizeAndAddBiasAndWrite(scale, input_bias, output));
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
  LOG(info, "Calling fallback implementation of \"int8MultiplyAndAddBias\"");
  intgemm::Int8Shift::Multiply(
      input_A_prepared,
      input_B_prepared,
      rows_A,
      width,
      cols_B,
      intgemm::callbacks::UnquantizeAndAddBiasAndWrite(scale_A, input_bias_prepared, output));
}

extern "C" void int8SelectColumnsOfBFallback(const int8_t* input_B_prepared,
                                             Index width,
                                             Index cols_B,
                                             const Index* cols,
                                             const Index num_cols,
                                             int8_t* output) {
  LOG(info, "Calling fallback implementation of \"int8SelectColumnsOfB\"");
  intgemm::Int8::SelectColumnsB(input_B_prepared, output, width, cols, cols + num_cols);
}

#endif  // WASM
