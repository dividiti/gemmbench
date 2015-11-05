// Each work-item reads dj rows of matrix B (or columns of matrix B transposed).
#define dj ((uint)2)
// Each work-item reads di rows of matrix A.
#define di ((uint)1)
// Each row contains (n/2) vectors.
#define nv (n >> 1)

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

kernel void gemm(
    global double2 const * restrict A,
    global double2 const * restrict B,
    global double2       * restrict C,
    double alpha, double beta, uint n)
{
    const uint j = get_global_id(0);
    const uint i = get_global_id(1);

    global double2 const *pA = &A[nv*di*i];
    global double2 const *pB = &B[j];

    double2 ab = (double2) 0.0;
    for (uint k = 0; k < nv; k += 1, pA += 1, pB += n)
    {
        double2 a  = pA[nv*0];

        double2 b0 = pB[nv*0];
        double2 b1 = pB[nv*1];

        ab += a.s0*b0 + a.s1*b1;
    }

    global double2 *pC = &C[nv*di*i + j];
    pC[nv*0] = alpha * ab + beta * pC[nv*0];
}

#pragma OPENCL EXTENSION cl_khr_fp64 : disable
