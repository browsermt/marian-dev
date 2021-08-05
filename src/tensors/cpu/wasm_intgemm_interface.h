#pragma once

/* Main interface for integer matrix multiplication (C = A * B + Bias) for wasm.
 *
 * A is typically activations whose rows should be a multiple of 1 (i.e. no restriction) and
 * columns should be a multiple of 64.
 * 
 * B is typically fixed model parameters whose rows should be a multiple of 64 and columns
 * should be a multiple of 8.
 * 
 * All matrices A, B and C are in row-major format.
 */

#include <cstdint>

using Index = unsigned int;


/**
 * Prepare B for the Matrix Multiply routine.
 *
 * B is prepared in a CPU-dependent format from an already quantized, transposed and a CPU-independent
 * format of B. This function is useful while using the quantized models that are stored in a
 * CPU-independent format on the disk.
 *
 * @param[in]   B_input               An array representing the input 2-D matrix.
 *                                    Size of the array = `B_untransposed_cols` * `B_untransposed_rows`
 * @param[in]   B_untransposed_rows   No. of rows of prepared B matrix. It should be a multiple of 64.
 * @param[in]   B_untransposed_cols   No. of columns of prepared B matrix. It should be a multiple of 8.
 * @param[out]  output                An array representing the prepared B matrix in row-major format.
 *                                    Size of the array = `B_untransposed_rows` * `B_untransposed_cols`
 */
void Int8PrepareBQuantizedTransposed(const int8_t* B_input,
                                    Index B_untransposed_rows,
                                    Index B_untransposed_cols,
                                    int8_t* output);


/**
 * Select a subset of columns from a prepared B matrix.
 *
 * Indices of the columns to be selected are specified by an array.
 *
 * @param[in]   B_input        An array representing the input 2-D matrix in row-major format.
 *                             Size of the array = `B_rows` * `B_cols`
 * @param[in]   B_rows         No. of rows of input matrix. It should be a multiple of 64.
 * @param[in]   B_cols         No. of columns of input matrix. It should be a multiple of 8.
 * @param[in]   cols           An array of column indices to be selected from input matrix.
 *                             All indices of the array should be valid. i.e.
 *                             i.e. 0 <= cols[N] < B_cols   where N = 0, 1, 2 .... (`num_cols`-1)
 * @param[in]   num_cols       Size of the `cols` array. It should be a multiple of 8.
 * @param[out]  output         An array representing the selected columns of input matrix.
 *                             Size of the array = `B_rows` * `num_cols`s
 */
void Int8SelectColumnsB(const int8_t* B_input,
                        Index B_rows,
                        Index B_cols,
                        const Index* cols,
                        const Index num_cols,
                        int8_t* output);


/**
 * Prepare A for the Matrix Multiply routine.
 *
 * It performs quantization on floating values.
 *
 * @param[in]   A_input     An array representing the input 2-D matrix in row-major format.
 *                          Size of the array = `A_rows` * `A_cols`
 * @param[in]   scale       The scaling factor (for quantization)
 * @param[in]   zero_point  The zero point (for quantization)
 * @param[in]   A_rows      No. of rows of input matrix. No restriction on its size.
 * @param[in]   A_cols      No. of columns of input matrix. It should be a multiple of 64.
 * @param[out]  output      An array representing the prepared A matrix in row-major format having
 *                          the same size as of the input matrix i.e. `A_rows` * `A_cols`
 */
void Int8ShiftPrepareA(const float* A_input,
                        float scale,
                        int8_t zero_point,
                        Index A_rows,
                        Index A_cols,
                        int8_t* output);


/**
 * Prepares bias for the Matrix Multiply routine.
 *
 * @param[in]   B_input      An array representing the input 2-D matrix in row-major format from
 *                           which bias will be prepared. Size of the array = `B_rows` * `B_cols`
 * @param[in]   scale        The scaling factor (for quantization)
 * @param[in]   zero_point   The zero point (for quantization)
 * @param[in]   B_rows       No. of rows of input matrix. It should be a multiple of 64.
 * @param[in]   B_cols       No. of columns of input matrix. It should be a multiple of 8.
 * @param[in]   bias_input   An array representing the input bias having size = 1 * `B_cols`
 * @param[out]  output       An array representing the prepared bias having size = 1 * `B_cols`
 */
void Int8ShiftPrepareBias(const int8_t* B_input,
                        float scale,
                        int8_t zero_point,
                        Index B_rows,
                        Index B_cols,
                        const float* bias_input,
                        float* output);


/**
 * A multiply routine to perform multiplication of 2 matrices.
 *
 * It does C = A * B + Bias, presuming A, B and Bias are inputs prepared using the
 * corresponding Prepare* functions.
 *
 * @param[in]   A_input       An array representing the input 2-D matrix A in row-major format.
 *                            Size of the array = `A_rows` * `width`
 * @param[in]   A_scale       The scaling factor (for quantization) of A
 * @param[in]   A_zero_point  The zero point (for quantization) of A
 * @param[in]   B_input       An array representing the input 2-D matrix B in row-major format.
 *                            Size of the array = `width` * `B_cols`
 * @param[in]   B_scale       The scaling factor (for quantization) of B
 * @param[in]   B_zero_point  The zero point (for quantization) of B
 * @param[in]   A_rows        No. of rows of input matrix A. No restriction on its size.
 * @param[in]   width         No. of columns of input matrix A (= no. of rows of input matrix B).
 *                            It should be a multiple of 64.
 * @param[in]   B_cols        No. of columns of input matrix B. It should be a multiple of 8.
 * @param[out]  output        An array representing the multiplication result of input matrix A and B
 *                            in row-major format. Size of the array = `A_rows` * `B_cols`
 */
void Int8ShiftMultiply(const int8_t* A_input,
                        float scale_A,
                        int8_t zero_point_A,
                        const int8_t* B_input,
                        float scale_B,
                        int8_t zero_point_B,
                        const float* bias_input,
                        Index A_rows,
                        Index width,
                        Index B_cols,
                        float* output);
