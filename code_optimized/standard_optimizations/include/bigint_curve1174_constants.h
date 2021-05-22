#ifndef BIGINT_CURVE1174_CONSTANTS_H_    /* Include guard */
#define BIGINT_CURVE1174_CONSTANTS_H_

#include "bigint.h"

#define Q_CHUNKS 8
#define Q_MSB_CHUNK 0x07ffffff
#define Q_LSB_CHUNK 0xfffffff7
#define Q_INTERMEDIATE_CHUNK 0xffffffff

/**
 * Hard coded prime q = 2^251 - 9 of Curve1174
 */
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q, 0, 0, Q_CHUNKS,
    (Q_LSB_CHUNK, 0xffffffff, 0xffffffff, 0xffffffff,
     0xffffffff, 0xffffffff, 0xffffffff, Q_MSB_CHUNK));

// q - 2
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_m2, 0, 0, Q_CHUNKS,
    (Q_LSB_CHUNK - 2, 0xffffffff, 0xffffffff, 0xffffffff,
     0xffffffff, 0xffffffff, 0xffffffff, Q_MSB_CHUNK));

/**
 * The following are q * a for a \in [2, 33]. We need those for a fast mod operation.
 */
#define Q_1_CHUNK_0 0xfffffff7
#define Q_1_CHUNK_7 0x7ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_1, 0, 0, Q_CHUNKS,
     (Q_1_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_1_CHUNK_7));

#define Q_2_CHUNK_0 0xffffffee
#define Q_2_CHUNK_7 0xfffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_2, 0, 0, Q_CHUNKS,
     (Q_2_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_2_CHUNK_7));

#define Q_3_CHUNK_0 0xffffffe5
#define Q_3_CHUNK_7 0x17ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_3, 0, 0, Q_CHUNKS,
     (Q_3_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_3_CHUNK_7));

#define Q_4_CHUNK_0 0xffffffdc
#define Q_4_CHUNK_7 0x1fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_4, 0, 0, Q_CHUNKS,
     (Q_4_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_4_CHUNK_7));

#define Q_5_CHUNK_0 0xffffffd3
#define Q_5_CHUNK_7 0x27ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_5, 0, 0, Q_CHUNKS,
     (Q_5_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_5_CHUNK_7));

#define Q_6_CHUNK_0 0xffffffca
#define Q_6_CHUNK_7 0x2fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_6, 0, 0, Q_CHUNKS,
     (Q_6_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_6_CHUNK_7));

#define Q_7_CHUNK_0 0xffffffc1
#define Q_7_CHUNK_7 0x37ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_7, 0, 0, Q_CHUNKS,
     (Q_7_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_7_CHUNK_7));

#define Q_8_CHUNK_0 0xffffffb8
#define Q_8_CHUNK_7 0x3fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_8, 0, 0, Q_CHUNKS,
     (Q_8_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_8_CHUNK_7));

#define Q_9_CHUNK_0 0xffffffaf
#define Q_9_CHUNK_7 0x47ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_9, 0, 0, Q_CHUNKS,
     (Q_9_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_9_CHUNK_7));

#define Q_10_CHUNK_0 0xffffffa6
#define Q_10_CHUNK_7 0x4fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_10, 0, 0, Q_CHUNKS,
     (Q_10_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_10_CHUNK_7));

#define Q_11_CHUNK_0 0xffffff9d
#define Q_11_CHUNK_7 0x57ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_11, 0, 0, Q_CHUNKS,
     (Q_11_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_11_CHUNK_7));

#define Q_12_CHUNK_0 0xffffff94
#define Q_12_CHUNK_7 0x5fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_12, 0, 0, Q_CHUNKS,
     (Q_12_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_12_CHUNK_7));

#define Q_13_CHUNK_0 0xffffff8b
#define Q_13_CHUNK_7 0x67ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_13, 0, 0, Q_CHUNKS,
     (Q_13_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_13_CHUNK_7));

#define Q_14_CHUNK_0 0xffffff82
#define Q_14_CHUNK_7 0x6fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_14, 0, 0, Q_CHUNKS,
     (Q_14_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_14_CHUNK_7));

#define Q_15_CHUNK_0 0xffffff79
#define Q_15_CHUNK_7 0x77ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_15, 0, 0, Q_CHUNKS,
     (Q_15_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_15_CHUNK_7));

#define Q_16_CHUNK_0 0xffffff70
#define Q_16_CHUNK_7 0x7fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_16, 0, 0, Q_CHUNKS,
     (Q_16_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_16_CHUNK_7));

#define Q_17_CHUNK_0 0xffffff67
#define Q_17_CHUNK_7 0x87ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_17, 0, 0, Q_CHUNKS,
     (Q_17_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_17_CHUNK_7));

#define Q_18_CHUNK_0 0xffffff5e
#define Q_18_CHUNK_7 0x8fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_18, 0, 0, Q_CHUNKS,
     (Q_18_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_18_CHUNK_7));

#define Q_19_CHUNK_0 0xffffff55
#define Q_19_CHUNK_7 0x97ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_19, 0, 0, Q_CHUNKS,
     (Q_19_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_19_CHUNK_7));

#define Q_20_CHUNK_0 0xffffff4c
#define Q_20_CHUNK_7 0x9fffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_20, 0, 0, Q_CHUNKS,
     (Q_20_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_20_CHUNK_7));

#define Q_21_CHUNK_0 0xffffff43
#define Q_21_CHUNK_7 0xa7ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_21, 0, 0, Q_CHUNKS,
     (Q_21_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_21_CHUNK_7));

#define Q_22_CHUNK_0 0xffffff3a
#define Q_22_CHUNK_7 0xafffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_22, 0, 0, Q_CHUNKS,
     (Q_22_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_22_CHUNK_7));

#define Q_23_CHUNK_0 0xffffff31
#define Q_23_CHUNK_7 0xb7ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_23, 0, 0, Q_CHUNKS,
     (Q_23_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_23_CHUNK_7));

#define Q_24_CHUNK_0 0xffffff28
#define Q_24_CHUNK_7 0xbfffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_24, 0, 0, Q_CHUNKS,
     (Q_24_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_24_CHUNK_7));

#define Q_25_CHUNK_0 0xffffff1f
#define Q_25_CHUNK_7 0xc7ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_25, 0, 0, Q_CHUNKS,
     (Q_25_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_25_CHUNK_7));

#define Q_26_CHUNK_0 0xffffff16
#define Q_26_CHUNK_7 0xcfffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_26, 0, 0, Q_CHUNKS,
     (Q_26_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_26_CHUNK_7));

#define Q_27_CHUNK_0 0xffffff0d
#define Q_27_CHUNK_7 0xd7ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_27, 0, 0, Q_CHUNKS,
     (Q_27_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_27_CHUNK_7));

#define Q_28_CHUNK_0 0xffffff04
#define Q_28_CHUNK_7 0xdfffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_28, 0, 0, Q_CHUNKS,
     (Q_28_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_28_CHUNK_7));

#define Q_29_CHUNK_0 0xfffffefb
#define Q_29_CHUNK_7 0xe7ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_29, 0, 0, Q_CHUNKS,
     (Q_29_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_29_CHUNK_7));

#define Q_30_CHUNK_0 0xfffffef2
#define Q_30_CHUNK_7 0xefffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_30, 0, 0, Q_CHUNKS,
     (Q_30_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_30_CHUNK_7));

#define Q_31_CHUNK_0 0xfffffee9
#define Q_31_CHUNK_7 0xf7ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_31, 0, 0, Q_CHUNKS,
     (Q_31_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_31_CHUNK_7));

#define Q_32_CHUNK_0 0xfffffee0
#define Q_32_CHUNK_7 0xffffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_32, 0, 0, Q_CHUNKS,
     (Q_32_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
      0xffffffff, 0xffffffff, 0xffffffff, Q_32_CHUNK_7));

#define Q_33_CHUNK_0 0xfffffed7
#define Q_33_CHUNK_7 0x07ffffff
BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_33, 0, 0, Q_CHUNKS + 1,
    (Q_33_CHUNK_0, 0xffffffff, 0xffffffff, 0xffffffff,
     0xffffffff, 0xffffffff, 0xffffffff, Q_33_CHUNK_7, 0x1));

/**
 * The constant 288 is useful because 288 = 2^256 mod q
 */
__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_288, 0, 0, 1, (288));

#endif // BIGINT_CURVE1174_CONSTANTS_H_
