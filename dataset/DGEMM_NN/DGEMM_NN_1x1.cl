#pragma OPENCL EXTENSION cl_khr_fp64 : enable

kernel void gemm(
    global double const * restrict A,
    global double const * restrict B,
    global double       * restrict C,
    double alpha, double beta, uint n)
{
    const uint j = get_global_id(0);
    const uint i = get_global_id(1);

    double ABij = 0.0;
    for (uint k = 0; k < n; k += 1)
    {
        ABij += A[i*n + k] * B[k*n + j];
    }
    C[i*n + j] = alpha * ABij + beta * C[i*n + j];
}

#pragma OPENCL EXTENSION cl_khr_fp64 : disable
