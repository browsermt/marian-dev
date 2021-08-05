#pragma once

/* Main interface for integer matrix multiplication (C = A * B) for wasm.
 *
 * A is typically activations whose rows should be a multiple of 1 (i.e. no restriction) and
 * columns should be a multiple of 64.
 * 
 * B is typically fixed model parameters whose rows should be a multiple of 64 and columns
 * should be a multiple of 8.
 * 
 * C is row major.
 */

#include <cstdint>

using Index = unsigned int;


/* Prepare B in a CPU-dependent format from an already quantized, transposed (routine not provided)
 * and CPU-independent format of B. The prepared B will be used as an input to Multiply routine.
 * This function is useful while using the quantized models that are stored in a CPU-independent
 * format on the disk.
 */
void Int8PrepareBQuantizedTransposed(const int8_t* B_input,
                                    Index B_untransposed_rows,
                                    Index B_untransposed_cols,
                                    int8_t* output);


/* Select columns from a prepared B matrix. The number of selected columns must be a multiple of 8.
 */
void Int8SelectColumnsB(const int8_t* B_input,
                        Index B_rows,
                        Index B_cols,             // We need this for bound checking for B buffer?
                        const Index* cols_begin,  // Shouldn't cols_begin be just Index and not Index*?
                        const Index* cols_end,    // Shouldn't cols_end be just Index and not Index*?
                        int8_t* output);


/* Prepare A for the Multiply routine that performs unsigned * signed multiplication. It performs
 * quantization on floating values and adds 127 to each number to make sure that all numbers are
 * positive.
 */
void Int8ShiftPrepareA(const float* A_input,
                        float scale,
                        int8_t zero_point,
                        Index A_rows,
                        Index A_cols,
                        int8_t* output);


/* Prepares bias for the Multiply routine that performs unsigned * signed multiplication.
 */
void Int8ShiftPrepareBias(const int8_t* B_input,
                        float scale,
                        int8_t zero_point,
                        const float* bias_input,
                        Index B_rows,
                        Index B_cols,
                        int8_t* output);


/* A multiply routine to perform unsigned * signed multiplication.
 * It does C = A * B + Bias, presuming A, B and Bias are quantized inputs prepared using the
 * corresponding Prepare* functions.
 */
void Int8ShiftMultiply(const int8_t* A_input,
                        float scale_A,
                        int8_t zero_point_A,
                        const int8_t* B_input,
                        float scale_B,
                        int8_t zero_point_B,
                        const int8_t* bias_input,
                        Index A_rows,
                        Index width,
                        Index B_cols,
                        float* output);
