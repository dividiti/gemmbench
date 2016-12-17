#ifndef Tuple_float4_float4_DEFINED
#define Tuple_float4_float4_DEFINED
typedef struct {
  float4 _0;
  float4 _1;
} Tuple_float4_float4;
#endif
float idfloat(float x){
  { return x; }}
float add(float x, float y){
  { return x+y; }}
float id(float x){
  { return x; }}
kernel void KERNEL(const global float* restrict v__89, const global float* restrict v__90, global float* v__104, int v_K_2, int v_M_1, int v_N_0){ 
#ifndef WORKGROUP_GUARD
#define WORKGROUP_GUARD
#endif
WORKGROUP_GUARD
{
  /* Static local memory */
  /* Typed Value memory */
  float v__92;
  /* Private Memory */
  float v__93_0;
  float v__93_1;
  float v__93_2;
  float v__93_3;
  float v__100_0;
  float v__100_1;
  float v__100_2;
  float v__100_3;
  {
    int v_gl_id_88 = get_global_id(0);
    {
      {
        int v_gl_id_87 = get_global_id(1);
        {
          float v__158 = 0.0f;
          v__92 = v__158;
          /* map_seq */
          /* unroll */
          /* map_seq */
          /* unroll */
          v__93_0 = idfloat(v__92);
          v__93_1 = idfloat(v__92);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__93_2 = idfloat(v__92);
          v__93_3 = idfloat(v__92);
          /* end unroll */
          /* end map_seq */
          /* end unroll */
          /* end map_seq */
          {
            /* reduce_seq */
            for (int v_i_84 = 0; v_i_84 < (v_K_2 / (4)); v_i_84 += 1) {
              {
                /* map_seq */
                /* unroll */
                /* map_seq */
                /* unroll */
                /* map_seq */
                /* unroll */
                v__100_0 = dot(vload4((((v_gl_id_88 * v_K_2) / 2) + v_i_84),v__89), vload4((((v_gl_id_87 * v_K_2) / 2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_0 = add(v__93_0, v__100_0);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_1 = dot(vload4((((v_gl_id_88 * v_K_2) / 2) + v_i_84),v__89), vload4(((((2 * v_gl_id_87 * v_K_2) + v_K_2) / 4) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_1 = add(v__93_1, v__100_1);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* end unroll */
                /* end map_seq */
                /* map_seq */
                /* unroll */
                /* map_seq */
                /* unroll */
                v__100_2 = dot(vload4(((((2 * v_gl_id_88 * v_K_2) + v_K_2) / 4) + v_i_84),v__89), vload4((((v_gl_id_87 * v_K_2) / 2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_2 = add(v__93_2, v__100_2);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_3 = dot(vload4(((((2 * v_gl_id_88 * v_K_2) + v_K_2) / 4) + v_i_84),v__89), vload4(((((2 * v_gl_id_87 * v_K_2) + v_K_2) / 4) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_3 = add(v__93_3, v__100_3);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* end unroll */
                /* end map_seq */
                /* end unroll */
                /* end map_seq */
              }
            }
            /* end reduce_seq */
          }
          /* map_seq */
          /* unroll */
          /* map_seq */
          /* unroll */
          /* map_seq */
          /* unroll */
          v__104[((2 * v_gl_id_88 * v_N_0) + (2 * v_gl_id_87))] = id(v__93_0);
          v__104[(1 + (2 * v_gl_id_88 * v_N_0) + (2 * v_gl_id_87))] = id(v__93_1);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__104[((2 * v_gl_id_88 * v_N_0) + (2 * v_gl_id_87) + v_N_0)] = id(v__93_2);
          v__104[(1 + (2 * v_gl_id_88 * v_N_0) + (2 * v_gl_id_87) + v_N_0)] = id(v__93_3);
          /* end unroll */
          /* end map_seq */
          /* end unroll */
          /* end map_seq */
          /* end unroll */
          /* end map_seq */
        }
      }
    }
  }
}
}
