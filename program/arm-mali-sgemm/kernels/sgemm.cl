/*
 * Copyright (c) 2016 ARM Limited.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma OPENCL EXTENSION cl_khr_fp16 : enable

__kernel void mm_fp32( __global float *const mtx_a,
                       __global float *const mtx_b,
                       __global float       *mtx_out
                     )
{
    /* src_addr.s0 = address of matrix A */
    /* src_addr.s1 = address of matrix B */

    /* Compute address for matrix A and B */
    uint2 src_addr = (uint2)(get_global_id (1), get_global_id (0)) * (uint2)(MATRIX_A_INTERLEAVED_STRIDE,
                                                                             MATRIX_B_TRANSPOSED_STRIDE);

    /* Compute end row address for matrix B */
    uint end_row_mtx_b = src_addr.s1 + MATRIX_B_TRANSPOSED_STRIDE;

    /* Reset accumulators */
    float4 c00 = 0.0f;
    float4 c10 = 0.0f;
    float4 c20 = 0.0f;
    float4 c30 = 0.0f;

    for(; src_addr.s1 < end_row_mtx_b; src_addr += (uint2)(8, 8))
    {
        /* Load values from matrix A (interleaved) and matrix B (transposed) */
        float4 a0 = vload4(0, mtx_a + src_addr.s0);
        float4 b0 = vload4(0, mtx_b + src_addr.s1);

        c00 += (float4)a0.s0 * b0;
        c10 += (float4)a0.s1 * b0;
        c20 += (float4)a0.s2 * b0;
        c30 += (float4)a0.s3 * b0;

        /* Load values from matrix A (interleaved) and matrix B (transposed) */
        a0 = vload4(0, mtx_a + src_addr.s0 + 4);
        b0 = vload4(0, mtx_b + src_addr.s1 + 4);

        c00 += (float4)a0.s0 * b0;
        c10 += (float4)a0.s1 * b0;
        c20 += (float4)a0.s2 * b0;
        c30 += (float4)a0.s3 * b0;
    }

    /* Wait for all work-items to finish */
    barrier(CLK_GLOBAL_MEM_FENCE);

    /* Compute destination address */
    uint dst_addr = (get_global_id (0) + get_global_id (1) * COL_MTX_C) * 4;

    /* Store 4x4 block */
    vstore4(c00, 0, mtx_out + dst_addr + 0 * COL_MTX_C);
    vstore4(c10, 0, mtx_out + dst_addr + 1 * COL_MTX_C);
    vstore4(c20, 0, mtx_out + dst_addr + 2 * COL_MTX_C);
    vstore4(c30, 0, mtx_out + dst_addr + 3 * COL_MTX_C);
}

__kernel void mm_fp16( __global half *const mtx_a,
                       __global half *const mtx_b,
                       __global half       *mtx_out
                     )
{
    /* src_addr.s0 = address of matrix A */
    /* src_addr.s1 = address of matrix B */

    /* Compute address for matrix A and B */
    uint2 src_addr = (uint2)(get_global_id (1), get_global_id (0)) * (uint2)(MATRIX_A_INTERLEAVED_STRIDE,
                                                                             MATRIX_B_TRANSPOSED_STRIDE);

    /* Compute end row address for matrix B */
    uint end_row_mtx_b = src_addr.s1 + MATRIX_B_TRANSPOSED_STRIDE;

    /* Reset accumulators */
    half8 c00 = 0.0f;
    half8 c10 = 0.0f;
    half8 c20 = 0.0f;
    half8 c30 = 0.0f;

    for(; src_addr.s1 < end_row_mtx_b; src_addr += (uint2)(8, 16))
    {
        /* Load values from matrix A (interleaved) and matrix B (transposed) */
        half4 a0 = vload4(0, mtx_a + src_addr.s0);
        half8 b0 = vload8(0, mtx_b + src_addr.s1);

        c00 += (half8)a0.s0 * b0;
        c10 += (half8)a0.s1 * b0;
        c20 += (half8)a0.s2 * b0;
        c30 += (half8)a0.s3 * b0;

        /* Load values from matrix A (interleaved) and matrix B (transposed) */
        a0 = vload4(0, mtx_a + src_addr.s0 + 4);
        b0 = vload8(0, mtx_b + src_addr.s1 + 8);

        c00 += (half8)a0.s0 * b0;
        c10 += (half8)a0.s1 * b0;
        c20 += (half8)a0.s2 * b0;
        c30 += (half8)a0.s3 * b0;
    }

    /* Wait for all work-items to finish */
    barrier(CLK_GLOBAL_MEM_FENCE);

    /* Compute destination address */
    uint dst_addr = (get_global_id (0) * 2 + get_global_id (1) * COL_MTX_C) * 4;

    /* Store 8x4 block */
    vstore8(c00, 0, mtx_out + dst_addr + 0 * COL_MTX_C);
    vstore8(c10, 0, mtx_out + dst_addr + 1 * COL_MTX_C);
    vstore8(c20, 0, mtx_out + dst_addr + 2 * COL_MTX_C);
    vstore8(c30, 0, mtx_out + dst_addr + 3 * COL_MTX_C);
}

__kernel void mm_hybrid( __global half *const mtx_a,
                         __global half *const mtx_b,
                         __global half       *mtx_out
                       )
{
    /* src_addr.s0 = address of matrix A */
    /* src_addr.s1 = address of matrix B */

    /* Compute address for matrix A and B */
    uint2 src_addr = (uint2)(get_global_id (1), get_global_id (0)) * (uint2)(MATRIX_A_INTERLEAVED_STRIDE,
                                                                             MATRIX_B_TRANSPOSED_STRIDE);

    /* Compute end row address for matrix B */
    uint end_row_mtx_b = src_addr.s1 + MATRIX_B_TRANSPOSED_STRIDE;

    /* Reset accumulators */
    float4 c00 = 0.0f;
    float4 c10 = 0.0f;
    float4 c20 = 0.0f;
    float4 c30 = 0.0f;

    for(; src_addr.s1 < end_row_mtx_b; src_addr += (uint2)(8, 8))
    {
        /* Load values from matrix A (interleaved) and matrix B (transposed) */
        half8 a0_a1 = vload8(0, mtx_a + src_addr.s0);
        half8 b0_b1 = vload8(0, mtx_b + src_addr.s1);

        c00 += convert_float4((half4)a0_a1.s0 * b0_b1.s0123);
        c00 += convert_float4((half4)a0_a1.s4 * b0_b1.s4567);
        c10 += convert_float4((half4)a0_a1.s1 * b0_b1.s0123);
        c10 += convert_float4((half4)a0_a1.s5 * b0_b1.s4567);

        c20 += convert_float4((half4)a0_a1.s2 * b0_b1.s0123);
        c20 += convert_float4((half4)a0_a1.s6 * b0_b1.s4567);
        c30 += convert_float4((half4)a0_a1.s3 * b0_b1.s0123);
        c30 += convert_float4((half4)a0_a1.s7 * b0_b1.s4567);
    }

    /* Wait for all work-items to finish */
    barrier(CLK_GLOBAL_MEM_FENCE);

    /* Compute destination address */
    uint dst_addr = (get_global_id (0) + get_global_id (1) * COL_MTX_C) * 4;

    /* Store 4x4 block */
    vstore_half4(c00, 0, mtx_out + dst_addr + 0 * COL_MTX_C);
    vstore_half4(c10, 0, mtx_out + dst_addr + 1 * COL_MTX_C);
    vstore_half4(c20, 0, mtx_out + dst_addr + 2 * COL_MTX_C);
    vstore_half4(c30, 0, mtx_out + dst_addr + 3 * COL_MTX_C);

}

__kernel void transpose_fp32( __global float const *mtx_b,
                              __global float       *mtx_out
                            )
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    /* Compute address for Matrix B - source */
    uint src_addr = x * 4 + y * COL_MTX_B;

    /* Compute address for Matrix B transposed - destination */
    uint dst_addr = y * 4 + x * MATRIX_B_TRANSPOSED_STRIDE;

    float4 b0 = vload4(0, mtx_b + src_addr);

    vstore4(b0, 0, mtx_out + dst_addr);
}

__kernel void transpose_fp16( __global half const *mtx_b,
                              __global half       *mtx_out
                            )
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    /* Compute address for Matrix B - source */
    uint src_addr = x * 8 + y * COL_MTX_B;

    /* Compute address for Matrix B transposed - destination */
    uint dst_addr = y * 8 + x * MATRIX_B_TRANSPOSED_STRIDE;

    half8 b0 = vload8(0, mtx_b + src_addr);

    vstore8(b0, 0, mtx_out + dst_addr);
}

__kernel void transpose_hybrid( __global half const *mtx_b,
                                __global half       *mtx_out
                              )
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    /* Compute address for Matrix B - source */
    uint src_addr = x * 4 + y * COL_MTX_B;

    /* Compute address for Matrix B transposed - destination */
    uint dst_addr = y * 4 + x * MATRIX_B_TRANSPOSED_STRIDE;

    half4 b0 = vload4(0, mtx_b + src_addr);

    vstore4(b0, 0, mtx_out + dst_addr);
}

__kernel void interleave_fp32( __global float const *mtx_a,
                               __global float       *mtx_out
                             )
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    /* Compute address for Matrix A - source */
    uint src_addr = x * 4 + y * 4 * COL_MTX_A;

    /* Compute address for Matrix A interleaved - destination */
    uint dst_addr = x * 16 + y * MATRIX_A_INTERLEAVED_STRIDE;

    /* Load values from Matrix A */
    float4 a0 = vload4(0, mtx_a + src_addr + 0 * COL_MTX_A);
    float4 a1 = vload4(0, mtx_a + src_addr + 1 * COL_MTX_A);
    float4 a2 = vload4(0, mtx_a + src_addr + 2 * COL_MTX_A);
    float4 a3 = vload4(0, mtx_a + src_addr + 3 * COL_MTX_A);

    float4 val0 = (float4)(a0.s0, a1.s0, a2.s0, a3.s0);
    vstore4(val0, 0, mtx_out + dst_addr);

    val0 = (float4)(a0.s1, a1.s1, a2.s1, a3.s1);
    vstore4(val0, 0, mtx_out + dst_addr + 4);

    val0 = (float4)(a0.s2, a1.s2, a2.s2, a3.s2);
    vstore4(val0, 0, mtx_out + dst_addr + 8);

    val0 = (float4)(a0.s3, a1.s3, a2.s3, a3.s3);
    vstore4(val0, 0, mtx_out + dst_addr + 12);
}

__kernel void interleave_fp16( __global half const *mtx_a,
                               __global half       *mtx_out
                             )
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    /* Compute address for Matrix A - source */
    uint src_addr = x * 4 + y * 4 * COL_MTX_A;

    /* Compute address for Matrix A interleaved - destination */
    uint dst_addr = x * 16 + y * MATRIX_A_INTERLEAVED_STRIDE;

    half4 a0 = vload4(0, mtx_a + src_addr + 0 * COL_MTX_A);
    half4 a1 = vload4(0, mtx_a + src_addr + 1 * COL_MTX_A);
    half4 a2 = vload4(0, mtx_a + src_addr + 2 * COL_MTX_A);
    half4 a3 = vload4(0, mtx_a + src_addr + 3 * COL_MTX_A);

    half4 val0 = (half4)(a0.s0, a1.s0, a2.s0, a3.s0);
    vstore4(val0, 0, mtx_out + dst_addr);

    val0 = (half4)(a0.s1, a1.s1, a2.s1, a3.s1);
    vstore4(val0, 0, mtx_out + dst_addr + 4);

    val0 = (half4)(a0.s2, a1.s2, a2.s2, a3.s2);
    vstore4(val0, 0, mtx_out + dst_addr + 8);

    val0 = (half4)(a0.s3, a1.s3, a2.s3, a3.s3);
    vstore4(val0, 0, mtx_out + dst_addr + 12);
}

__kernel void finalize_fp32( __global float const *mtx_c,
                             __global float       *mtx_axb,
                             float                alpha,
                             float                beta
                           )
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    /* Compute address */
    uint addr = x * 4 + y * COL_MTX_C;

    /* Load values from A x B */
    float4 axb = vload4(0, mtx_axb + addr);

    /* Load values from Matrix C */
    float4 c = vload4(0, mtx_c + addr);

    /* Computes alpha * axb + beta * c */
    float4 out = (float4)alpha * axb + (float4)beta * c;

    /* Store final result in axb matrix */
    vstore4(out, 0, mtx_axb + addr);
}

__kernel void finalize_fp16( __global half const *mtx_c,
                             __global half        *mtx_axb,
                             float               alpha,
                             float               beta
                           )
{
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    /* Compute address */
    uint addr = x * 8 + y * COL_MTX_C;

    half8 axb = vload8(0, mtx_axb + addr);
    half8 c = vload8(0, mtx_c + addr);

    /* Computes alpha * acc + beta * c. The sum is in fp32 */
    float8 out = convert_float8((half8)alpha * axb) + convert_float8((half8)beta * c);

    vstore_half8(out, 0, mtx_axb + addr);
}
