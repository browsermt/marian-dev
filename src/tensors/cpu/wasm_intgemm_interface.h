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

// A structure holding 8-bit quantized values and the associated quantization parameters.
typedef struct QuantizedBuffer {
    int8_t* value;
    float scale;
    int8_t zero_point;
} QuantizedBuffer;


/* Prepare B in a CPU-dependent format from an already quantized, transposed (routine not provided)
 * and CPU-independent format of B. The prepared B will be used as an input to Multiply routine.
 * This function is useful while using the quantized models that are stored in a CPU-independent
 * format on the disk.
 */
void Int8PrepareBQuantizedTransposed(const QuantizedBuffer& B_input, QuantizedBuffer& output, Index inner, Index B_untransposed_cols);


/* Select columns from a prepared B matrix. The number of selected columns must be a multiple of 8.
 */
void Int8SelectColumnsB(const QuantizedBuffer& B_input, QuantizedBuffer& output, Index rows, const Index* cols_begin, const Index* cols_end);


/* Prepare A for the Multiply routine that performs unsigned * signed multiplication. It performs quantization
 * on floating values and adds 127 to each number to make sure that all numbers are positive.
 */
void Int8ShiftPrepareA(const float* A_input, QuantizedBuffer& output, Index rows, Index cols);


/* Prepares bias for the Multiply routine that performs unsigned * signed multiplication.
 */
void Int8ShiftPrepareBias(const QuantizedBuffer& B_input, const float* bias_input, QuantizedBuffer& output, Index width, Index B_cols);


/* A multiply routine to perform unsigned * signed multiplication.
 * It does C = A * B + Bias, presuming A, B and Bias are quantized inputs prepared using the
 * corresponding Prepare* functions.
 */
void Int8ShiftMultiply(const QuantizedBuffer& A_input, const QuantizedBuffer& B_input, const QuantizedBuffer& bias_input, float* output, Index A_rows, Index width, Index B_cols);