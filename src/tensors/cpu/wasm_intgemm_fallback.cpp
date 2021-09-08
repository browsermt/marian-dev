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
  ABORT("Unimplemented fallback for interface \"int8PrepareA\"");
}

#endif  // WASM
