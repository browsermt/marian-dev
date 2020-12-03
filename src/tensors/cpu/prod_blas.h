#if MKL_FOUND
#include <mkl.h>
#elif BLAS_FOUND
  #if WASM_COMPATIBLE_BLAS
    #include "3rd_party/onnxjs/src/wasm-ops/gemm.h"
  #else
    #include <cblas.h>
  #endif // WASM_COMPATIBLE_BLAS
#endif

inline void sgemm(bool transA,
                  bool transB,
                  int rows_a,
                  int rows_b,
                  int width,
                  float alpha,
                  float* a,
                  int lda,
                  float* b,
                  int ldb,
                  float beta,
                  float* c,
                  int ldc) {
#if BLAS_FOUND
    #if WASM_COMPATIBLE_BLAS
        gemm_f32_imp(transA, transB, rows_a, rows_b, width, alpha, a, b, beta, c);
    #else
        cblas_sgemm(CblasRowMajor,
                    transA ? CblasTrans : CblasNoTrans,
                    transB ? CblasTrans : CblasNoTrans,
                    rows_a,
                    rows_b,
                    width,
                    alpha,
                    a,
                    lda,
                    b,
                    ldb,
                    beta,
                    c,
                    ldc);
    #endif
#else
    transA; transB; rows_a; rows_b; width; alpha; a; lda; b; ldb; beta; c; ldc; // make compiler happy
    ABORT("Marian must be compiled with a BLAS library");
#endif
}
