// Each work-item reads dj rows of matrix B (or columns of matrix B transposed).
#define dj ((uint)4)
// Each work-item reads di rows of matrix A.
#define di ((uint)1)
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

    float4 ab = (float4) 0.0f;
    for (uint k = 0; k < nv; k += dk)
    {
        for (uint kk = 0; kk < dk; kk += 1, pA += 1, pB += 1)
        {
            float4 a  = pA[nv*0];

            float4 b0 = pB[nv*0];
            float4 b1 = pB[nv*1];
            float4 b2 = pB[nv*2];
            float4 b3 = pB[nv*3];

            ab += (float4)(dot(a, b0), dot(a, b1), dot(a, b2), dot(a, b3));
        }
        // Wait for all work-items to finish the current tile.
        barrier(CLK_GLOBAL_MEM_FENCE);
    }

    global float4 *pC = &C[nv*di*i + j];
    pC[nv*0] = alpha * ab + beta * pC[nv*0];
}
