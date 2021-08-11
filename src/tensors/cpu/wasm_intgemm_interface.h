#pragma once

/** Main interface for integer matrix multiplication followed by addition of bias for wasm.
 *
 * C = A * B + Bias
 *
 * A is typically activations whose rows should be a multiple of 1 (i.e. no restriction) and
 * columns should be a multiple of 64.
 *
 * B is typically fixed model parameters whose rows should be a multiple of 64 and columns
 * should be a multiple of 8.
 *
 * All matrices A, B and C are in row-major format.
 *
 * Please note that most of the functions in this interface might have architecture specific
 * implementations.
 */

#include <cstdint>

using Index = unsigned int;

/**
 * Prepare B for the Matrix Multiply routine.
 *
 * B is prepared in a CPU-dependent format by performing quantization on floating values.
 * Please note that this interface might have architecture specific implementations.
 *
 * @param[in]   input_B                An array representing the input 2-D matrix.
 *                                     Size of the array = `output_rows` * `output_cols`.
 *
 *                                     If the input matrix is in transposed form then:
 *                                     Shape of the matrix: (`output_cols`, `output_rows`)
 *
 *                                     If the input matrix is NOT in transposed form then:
 *                                     Shape of the matrix: (`output_rows`, `output_cols`)
 * @param[in]   scale                  The scaling factor (for quantization)
 * @param[in]   zero_point             The zero point (for quantization)
 * @param[in]   is_input_transposed    Whether the input matrix is in transposed form or not.
 * @param[in]   output_rows            No. of rows of output (prepared B) matrix.
 *                                     It should be a multiple of 64.
 * @param[in]   output_cols            No. of columns of output (prepared B) matrix.
 *                                     It should be a multiple of 8.
 * @param[out]  output                 An array representing the prepared B matrix in row-major
 *                                     format. Size of the array = `output_rows` * `output_cols`.
 *                                     Shape of the matrix: (`output_rows`, `output_cols`)
 */
void int8PrepareB(const float* input_B,
                  float scale,
                  int8_t zero_point,
                  bool is_input_transposed,
                  Index output_rows,
                  Index output_cols,
                  int8_t* output);

/**
 * Prepare B for the Matrix Multiply routine from an already quantized, transposed and a
 * CPU-independent format of B.
 *
 * B is prepared in a CPU-dependent format. This function is useful while using the quantized models
 * that are stored in a CPU-independent format on the disk.
 *
 * @param[in]   input_B         An array representing the input 2-D matrix.
 *                              Size of the array = `output_rows` * `output_cols`.
 *                              Shape of the matrix: (`output_cols`, `output_rows`)
 * @param[in]   output_rows     No. of rows of output (prepared B) matrix.
 *                              It should be a multiple of 64.
 * @param[in]   output_cols     No. of columns of output (prepared B) matrix.
 *                              It should be a multiple of 8.
 * @param[out]  output          An array representing the prepared B matrix in row-major format.
 *                              Size of the array = `output_rows` * `output_cols`.
 *                              Shape of the matrix: (`output_rows`, `output_cols`)
 */
void int8PrepareBQuantizedTransposed(const int8_t* input_B,
                                     Index output_rows,
                                     Index output_cols,
                                     int8_t* output);

/**
 * Prepare A for the Matrix Multiply routine.
 *
 * It performs quantization on floating values.
 * Please note that this interface might have architecture specific implementations.
 *
 * @param[in]   input_A        An array representing the input 2-D matrix in row-major format.
 *                             Size of the array = `output_rows` * `output_cols`.
 *                             Shape of the matrix: (`output_rows`, `output_cols`)
 * @param[in]   scale          The scaling factor (for quantization)
 * @param[in]   zero_point     The zero point (for quantization)
 * @param[in]   output_rows    No. of rows of output (prepared A) matrix.
 *                             No restriction on its size.
 * @param[in]   output_cols    No. of columns of output (prepared A) matrix.
 *                             It should be a multiple of 64.
 * @param[out]  output         An array representing the prepared A matrix in row-major format.
 *                             Size of the array = `output_rows` * `output_cols`.
 *                             Shape of the matrix: (`output_rows`, `output_cols`)
 */
void int8PrepareA(const float* input_A,
                  float scale,
                  int8_t zero_point,
                  Index output_rows,
                  Index output_cols,
                  int8_t* output);

/**
 * Prepares bias for the Matrix Multiply routine.
 *
 * It uses the prepared B and a bias input to prepare the final bias.
 *
 * @param[in]   input_B      An array representing the prepared B (input) 2-D matrix in row-major
 *                           format. Size of the array = `rows_B` * `cols_B`.
 *                           Shape of the matrix: (`rows_B`, `cols_B`)
 * @param[in]   scale        The scaling factor (for quantization)
 * @param[in]   zero_point   The zero point (for quantization)
 * @param[in]   rows_B       No. of rows of the prepared B matrix. It should be a multiple of 64.
 * @param[in]   cols_B       No. of columns of prepared B matrix. It should be a multiple of 8.
 * @param[in]   bias_input   An array representing the input bias. Size of the array = 1 * `cols_B`
 * @param[out]  output       An array representing the final prepared bias.
 *                           Size of the array = 1 * `cols_B`
 */
void int8PrepareBias(const int8_t* input_B,
                     float scale,
                     int8_t zero_point,
                     Index rows_B,
                     Index cols_B,
                     const float* bias_input,
                     float* output);

/**
 * Perform multiplication of 2 matrices followed by adding a bias.
 *
 * i.e Output = A * B + Bias
 *
 * Please note that:
 * 1. This interface might have architecture specific implementation.
 * 2. Inputs A, B and Bias must be prepared using the corresponding implementations
 *    of int8Prepare* functions for that architecture.
 *
 * @param[in]   input_A       An array representing prepared A (input) 2-D matrix in row-major
 *                            format. Size of the array = `rows_A` * `width`.
 *                            Shape of the matrix: (`rows_A`, `width`)
 * @param[in]   scale_A       The scaling factor (for quantization) of A
 * @param[in]   zero_point_A  The zero point (for quantization) of A
 * @param[in]   input_B       An array representing prepared B (input) 2-D matrix in row-major
 *                            format. Size of the array = `width` * `cols_B`.
 *                            Shape of the matrix: (`width`, `cols_B`)
 * @param[in]   scale_B       The scaling factor (for quantization) of B
 * @param[in]   zero_point_B  The zero point (for quantization) of B
 * @param[in]   bias_input    An array representing the prepared bias.
 *                            Size of the array = 1 * `cols_B`
 * @param[in]   rows_A        No. of rows of prepared A matrix. No restriction on its size.
 * @param[in]   width         No. of columns of prepared A matrix (= no. of rows of prepared B
 *                            matrix). It should be a multiple of 64.
 * @param[in]   cols_B        No. of columns of prepared B matrix. It should be a multiple of 8.
 * @param[out]  output        An array representing the multiplication result in row-major format.
 *                            Size of the array = `rows_A` * `cols_B`
 */
void int8MultiplyAndAddBias(const int8_t* input_A,
                            float scale_A,
                            int8_t zero_point_A,
                            const int8_t* input_B,
                            float scale_B,
                            int8_t zero_point_B,
                            const float* bias_input,
                            Index rows_A,
                            Index width,
                            Index cols_B,
                            float* output);

/**
 * Select a subset of columns from a prepared B matrix.
 *
 * Indices of the columns to be selected are specified by an array.
 *
 * @param[in]   input_B        An array representing the prepared B (input) 2-D matrix in row-major
 *                             format. Size of the array = `rows_B` * `cols_B`.
 *                             Shape of the matrix: (`rows_B`, `cols_B`)
 * @param[in]   rows_B         No. of rows of input matrix. It should be a multiple of 64.
 * @param[in]   cols_B         No. of columns of input matrix. It should be a multiple of 8.
 * @param[in]   cols           An array of column indices to be selected from input matrix.
 *                             All indices of the array should be valid. i.e.
 *                             i.e. 0 <= cols[N] < cols_B   where N = 0, 1, 2 .... (`num_cols`-1)
 * @param[in]   num_cols       Size of the `cols` array. It should be a multiple of 8.
 * @param[out]  output         An array representing the selected columns of input matrix.
 *                             Size of the array = `rows_B` * `num_cols`.
 *                             Shape of the matrix: (`rows_B`, `num_cols`)
 */
void int8SelectColumnsOfB(const int8_t* input_B,
                          Index rows_B,
                          Index cols_B,
                          const Index* cols,
                          const Index num_cols,
                          int8_t* output);
