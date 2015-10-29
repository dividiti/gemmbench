kernel void gemm(
    global float const * restrict A,
    global float const * restrict B,
    global float       * restrict C,
    float alpha, float beta, uint n)
{
    const uint j = get_global_id(0);
    const uint i = get_global_id(1);

    float ABij = 0.0f;
    for (uint k = 0; k < n; k += 1)
    {
        ABij += A[i*n + k] * B[k*n + j];
    }
    C[i*n + j] = alpha * ABij + beta * C[i*n + j];
}
