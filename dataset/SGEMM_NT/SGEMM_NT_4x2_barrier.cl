// Each work-item reads dj rows of matrix B (or columns of matrix B transposed).
#define dj ((uint)4)
// Each work-item reads di rows of matrix A.
#define di ((uint)2)
// Each work-item reaches the barrier after dk accumulations.
#define dk ((uint)32)
// Each row contains (n/4) vectors.
#define nv (n >> 2)

kernel void gemm(
    global float4 const * restrict A,
    global float4 const * restrict B,
    global float4       * restrict C,
    float alpha, float beta, uint n)
{
    const uint j = get_global_id(0);
    const uint i = get_global_id(1);

    global float4 const *pA = &A[nv*di*i];
    global float4 const *pB = &B[nv*dj*j];

    float4 a0b = (float4) 0.0f;
    float4 a1b = (float4) 0.0f;
    for (uint k = 0; k < nv; k += dk)
    {
        for (uint kk = 0; kk < dk; kk += 1, pA += 1, pB += 1)
        {
            float4 a0 = pA[nv*0];
            float4 a1 = pA[nv*1];

            float4 b0 = pB[nv*0];
            float4 b1 = pB[nv*1];
            float4 b2 = pB[nv*2];
            float4 b3 = pB[nv*3];

            a0b += (float4)(dot(a0, b0), dot(a0, b1), dot(a0, b2), dot(a0, b3));
            a1b += (float4)(dot(a1, b0), dot(a1, b1), dot(a1, b2), dot(a1, b3));
        }
        // Wait for all work-items to finish the current tile.
        barrier(CLK_GLOBAL_MEM_FENCE);
    }

    global float4 *pC = &C[nv*di*i + j];
    pC[nv*0] = alpha * a0b + beta * pC[nv*0];
    pC[nv*1] = alpha * a1b + beta * pC[nv*1];
}
