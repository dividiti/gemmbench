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
  float v__93_4;
  float v__93_5;
  float v__93_6;
  float v__93_7;
  float v__93_8;
  float v__93_9;
  float v__93_10;
  float v__93_11;
  float v__93_12;
  float v__93_13;
  float v__93_14;
  float v__93_15;
  float v__100_0;
  float v__100_1;
  float v__100_2;
  float v__100_3;
  float v__100_4;
  float v__100_5;
  float v__100_6;
  float v__100_7;
  float v__100_8;
  float v__100_9;
  float v__100_10;
  float v__100_11;
  float v__100_12;
  float v__100_13;
  float v__100_14;
  float v__100_15;
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
          v__93_2 = idfloat(v__92);
          v__93_3 = idfloat(v__92);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__93_4 = idfloat(v__92);
          v__93_5 = idfloat(v__92);
          v__93_6 = idfloat(v__92);
          v__93_7 = idfloat(v__92);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__93_8 = idfloat(v__92);
          v__93_9 = idfloat(v__92);
          v__93_10 = idfloat(v__92);
          v__93_11 = idfloat(v__92);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__93_12 = idfloat(v__92);
          v__93_13 = idfloat(v__92);
          v__93_14 = idfloat(v__92);
          v__93_15 = idfloat(v__92);
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
                v__100_0 = dot(vload4(((v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_gl_id_87 * v_K_2) + v_i_84),v__90));
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
                v__100_1 = dot(vload4(((v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_1 = add(v__93_1, v__100_1);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_2 = dot(vload4(((v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 2) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
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
                v__100_3 = dot(vload4(((v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4((((3 * v_K_2) / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
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
                /* map_seq */
                /* unroll */
                /* map_seq */
                /* unroll */
                v__100_4 = dot(vload4(((v_K_2 / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_4 = add(v__93_4, v__100_4);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_5 = dot(vload4(((v_K_2 / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_5 = add(v__93_5, v__100_5);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_6 = dot(vload4(((v_K_2 / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 2) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_6 = add(v__93_6, v__100_6);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_7 = dot(vload4(((v_K_2 / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4((((3 * v_K_2) / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_7 = add(v__93_7, v__100_7);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* end unroll */
                /* end map_seq */
                /* map_seq */
                /* unroll */
                /* map_seq */
                /* unroll */
                v__100_8 = dot(vload4(((v_K_2 / 2) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_8 = add(v__93_8, v__100_8);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_9 = dot(vload4(((v_K_2 / 2) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_9 = add(v__93_9, v__100_9);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_10 = dot(vload4(((v_K_2 / 2) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 2) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_10 = add(v__93_10, v__100_10);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_11 = dot(vload4(((v_K_2 / 2) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4((((3 * v_K_2) / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_11 = add(v__93_11, v__100_11);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* end unroll */
                /* end map_seq */
                /* map_seq */
                /* unroll */
                /* map_seq */
                /* unroll */
                v__100_12 = dot(vload4((((3 * v_K_2) / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_12 = add(v__93_12, v__100_12);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_13 = dot(vload4((((3 * v_K_2) / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_13 = add(v__93_13, v__100_13);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_14 = dot(vload4((((3 * v_K_2) / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4(((v_K_2 / 2) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_14 = add(v__93_14, v__100_14);
                  /* end unroll */
                  /* end reduce_seq */
                }
                /* map_seq */
                /* unroll */
                v__100_15 = dot(vload4((((3 * v_K_2) / 4) + (v_gl_id_88 * v_K_2) + v_i_84),v__89), vload4((((3 * v_K_2) / 4) + (v_gl_id_87 * v_K_2) + v_i_84),v__90));
                /* end unroll */
                /* end map_seq */
                {
                  /* reduce_seq */
                  /* unroll */
                  v__93_15 = add(v__93_15, v__100_15);
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
          v__104[((4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87))] = id(v__93_0);
          v__104[(1 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87))] = id(v__93_1);
          v__104[(2 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87))] = id(v__93_2);
          v__104[(3 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87))] = id(v__93_3);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__104[((4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + v_N_0)] = id(v__93_4);
          v__104[(1 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + v_N_0)] = id(v__93_5);
          v__104[(2 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + v_N_0)] = id(v__93_6);
          v__104[(3 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + v_N_0)] = id(v__93_7);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__104[((4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (2 * v_N_0))] = id(v__93_8);
          v__104[(1 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (2 * v_N_0))] = id(v__93_9);
          v__104[(2 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (2 * v_N_0))] = id(v__93_10);
          v__104[(3 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (2 * v_N_0))] = id(v__93_11);
          /* end unroll */
          /* end map_seq */
          /* map_seq */
          /* unroll */
          v__104[((4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (3 * v_N_0))] = id(v__93_12);
          v__104[(1 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (3 * v_N_0))] = id(v__93_13);
          v__104[(2 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (3 * v_N_0))] = id(v__93_14);
          v__104[(3 + (4 * v_gl_id_88 * v_N_0) + (4 * v_gl_id_87) + (3 * v_N_0))] = id(v__93_15);
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
