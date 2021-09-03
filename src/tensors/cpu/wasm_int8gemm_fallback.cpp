#if defined(WASM)

#include "wasm_intgemm_interface.h"
#include "3rd_party/intgemm/intgemm/intgemm.h"
#include "common/logging.h"

extern "C"
void int8PrepareAFallback(const float* input_A,
                  float scale,
                  float zero_point,
                  Index rows_A,
                  Index width,
                  int8_t* output)
{
  LOG(info, "Calling fallback implementation of int8PrepareA");
  ABORT("Unimplemented fallback for int8PrepareA");
}

/*
void __attribute__((export_name("int8_prepare_a_fallback")))
int8PrepareAFallback(const float *A, int8_t *output_addr, float quant_mult, int rows, int cols)
{
  intgemm::Int8::PrepareA(A, output_addr, quant_mult, rows, cols);
}

void __attribute__((export_name("mmul_int8_shifted_ext")))
Int8ShiftedMultiplyExport(
  const int8_t *A, const int8_t *B, int A_rows, int width, int B_cols,
  float unquant_mult, const float* bias_addr, float* output_addr
) {
  intgemm::Int8Shift::Multiply(A, B, A_rows, width, B_cols,
                               intgemm::callbacks::UnquantizeAndAddBiasAndWrite(unquant_mult, bias_addr, output_addr));

}

void __attribute__((export_name("prepare_b_int8_ext")))
Int8PrepareBExport(const float *A, int8_t *output_addr, float quant_mult, int rows, int cols)
{
  intgemm::Int8::PrepareB(A, output_addr, quant_mult, rows, cols);
}

void __attribute__((export_name("prepare_bias_int8_shifted_ext")))
Int8ShiftedPrepareBiasExport(
  const int8_t *A, int rows, int cols,
  float unquant_mult, const float* bias_addr, float* output_addr
) {
  intgemm::Int8Shift::PrepareBias(A, rows, cols, 
                                  intgemm::callbacks::UnquantizeAndAddBiasAndWrite(unquant_mult, bias_addr, output_addr));
}

void __attribute__((export_name("mmul_int8_ext")))
Int8MultiplyExport(
  const int8_t *A, const int8_t *B, int A_rows, int width, int B_cols,
  float unquant_mult, float* output_addr
) {
  intgemm::Int8::Multiply(A, B, A_rows, width, B_cols, 
                          intgemm::callbacks::UnquantizeAndWrite(unquant_mult, output_addr));  
}

void __attribute__((export_name("prepare_qt_int8_ext")))
Int8PrepareBQuantizedTransposedExport(
  const int8_t *A, int8_t *output_addr, int rows, int cols
) {
    intgemm::Int8::PrepareBQuantizedTransposed(A, output_addr, rows, cols);
}
*/
#endif //WASM