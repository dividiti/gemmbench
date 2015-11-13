// Each work-item reads dj rows of matrix B (or columns of matrix B transposed).
#define dj ((uint)4)
// Each work-item reads di rows of matrix A.
#define di ((uint)1)
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
    for (uint k = 0; k < nv; k += 2, pA += 2, pB += 2)
    {
        float4  a_0 = pA[nv*0+0]; float4  a_1 = pA[nv*0+1];

        float4 b0_0 = pB[nv*0+0]; float4 b0_1 = pB[nv*0+1];
        float4 b1_0 = pB[nv*1+0]; float4 b1_1 = pB[nv*1+1];
        float4 b2_0 = pB[nv*2+0]; float4 b2_1 = pB[nv*2+1];
        float4 b3_0 = pB[nv*3+0]; float4 b3_1 = pB[nv*3+1];

        ab += (float4)(dot(a_0, b0_0), dot(a_0, b1_0), dot(a_0, b2_0), dot(a_0, b3_0));
        ab += (float4)(dot(a_1, b0_1), dot(a_1, b1_1), dot(a_1, b2_1), dot(a_1, b3_1));
    }

    global float4 *pC = &C[nv*di*i + j];
    pC[nv*0] = alpha * ab + beta * pC[nv*0];
}
