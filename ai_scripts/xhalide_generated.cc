// [AI-CodeGen]:  Code generated by XHalide compiler for AI architecture with vector intrinsics.
// Copyright (c) 2019 Xilinx Inc. All Rights Reserved.
#include<stdint.h>
#define XHALIDE_SRS_SHIFT 10


int conv(int32_t  * restrict _b0_buffer, int _b0_buffer_dim0_min, int _b0_buffer_dim0_extent, int _b0_buffer_dim0_stride, int _b0_buffer_dim1_min, int _b0_buffer_dim1_extent, int _b0_buffer_dim1_stride, int _b0_buffer_dim2_min, int _b0_buffer_dim2_extent, int _b0_buffer_dim2_stride, int _b0_buffer_dim3_min, int _b0_buffer_dim3_extent, int _b0_buffer_dim3_stride,
int32_t  * restrict _b1_buffer, int _b1_buffer_dim0_min, int _b1_buffer_dim0_extent, int _b1_buffer_dim0_stride, int _b1_buffer_dim1_min, int _b1_buffer_dim1_extent, int _b1_buffer_dim1_stride, int _b1_buffer_dim2_min, int _b1_buffer_dim2_extent, int _b1_buffer_dim2_stride, int _b1_buffer_dim3_min, int _b1_buffer_dim3_extent, int _b1_buffer_dim3_stride,
int32_t  * restrict _conv_buffer, int _conv_buffer_dim0_min, int _conv_buffer_dim0_extent, int _conv_buffer_dim0_stride, int _conv_buffer_dim1_min, int _conv_buffer_dim1_extent, int _conv_buffer_dim1_stride, int _conv_buffer_dim2_min, int _conv_buffer_dim2_extent, int _conv_buffer_dim2_stride, int _conv_buffer_dim3_min, int _conv_buffer_dim3_extent, int _conv_buffer_dim3_stride) {
 void *_0 = _b0_buffer;
 void * _b0 = _0;
 int32_t _1 = _b0_buffer_dim0_min;
 int32_t _2 = _b0_buffer_dim0_extent;
 int32_t _3 = _b0_buffer_dim0_stride;
 int32_t _4 = _b0_buffer_dim1_min;
 int32_t _5 = _b0_buffer_dim1_extent;
 int32_t _6 = _b0_buffer_dim1_stride;
 int32_t _7 = _b0_buffer_dim2_min;
 int32_t _8 = _b0_buffer_dim2_extent;
 int32_t _9 = _b0_buffer_dim2_stride;
 int32_t _10 = _b0_buffer_dim3_min;
 int32_t _11 = _b0_buffer_dim3_extent;
 int32_t _12 = _b0_buffer_dim3_stride;
 void *_13 = _b1_buffer;
 void * _b1 = _13;
 int32_t _14 = _b1_buffer_dim0_min;
 int32_t _15 = _b1_buffer_dim0_extent;
 int32_t _16 = _b1_buffer_dim0_stride;
 int32_t _17 = _b1_buffer_dim1_min;
 int32_t _18 = _b1_buffer_dim1_extent;
 int32_t _19 = _b1_buffer_dim1_stride;
 int32_t _20 = _b1_buffer_dim2_min;
 int32_t _21 = _b1_buffer_dim2_extent;
 int32_t _22 = _b1_buffer_dim2_stride;
 int32_t _23 = _b1_buffer_dim3_min;
 int32_t _24 = _b1_buffer_dim3_extent;
 int32_t _25 = _b1_buffer_dim3_stride;
 void *_26 = _conv_buffer;
 void * _conv = _26;
 int32_t _27 = _conv_buffer_dim0_min;
 int32_t _28 = _conv_buffer_dim0_extent;
 int32_t _29 = _conv_buffer_dim0_stride;
 int32_t _30 = _conv_buffer_dim1_min;
 int32_t _31 = _conv_buffer_dim1_extent;
 int32_t _32 = _conv_buffer_dim1_stride;
 int32_t _33 = _conv_buffer_dim2_min;
 int32_t _34 = _conv_buffer_dim2_extent;
 int32_t _35 = _conv_buffer_dim2_stride;
 int32_t _36 = _conv_buffer_dim3_min;
 int32_t _37 = _conv_buffer_dim3_extent;
 int32_t _38 = _conv_buffer_dim3_stride;
 // produce conv
 int32_t _39 = _28 + 7;
 int32_t _40 = _39 >> 3;
 int32_t _41 = _36 * _38;
 int32_t _42 = _27 - _41;
 int32_t _43 = _33 * _35;
 int32_t _44 = _42 - _43;
 int32_t _45 = _30 * _32;
 int32_t _46 = _44 - _45;
 int32_t _47 = _46 - _27;
 for (int _conv_s1_z = _33; _conv_s1_z < _33 + _34; _conv_s1_z++)
 {
  int32_t _48 = _conv_s1_z * 108;
  int32_t _49 = _conv_s1_z * _35;
  int32_t _50 = _49 + _47;
  for (int _conv_s1_n = _36; _conv_s1_n < _36 + _37; _conv_s1_n++)
  {
   int32_t _51 = _conv_s1_n * 576;
   int32_t _52 = _51 + _27;
   int32_t _53 = _conv_s1_n * _38;
   int32_t _54 = _53 + _50;
   for (int _conv_s1_x_x = 0; _conv_s1_x_x < 0 + _40; _conv_s1_x_x++)
   {
    int32_t _55 = _conv_s1_x_x * 8;
    int32_t _56 = _52 + _55;
    int32_t _57 = _54 + _55;
    for (int _conv_s1_y = _30; _conv_s1_y < _30 + _31; _conv_s1_y++)
    chess_prepare_for_pipelining
    {
     int32_t _58 = _48 + 1;
     int32_t _59 = _48 + 2;
     int32_t _60 = _48 + 3;
     int32_t _61 = _48 + 4;
     int32_t _62 = _48 + 5;
     int32_t _63 = _48 + 6;
     int32_t _64 = _48 + 7;
     int32_t _65 = _48 + 8;
     int32_t _66 = _48 + 9;
     int32_t _67 = _48 + 10;
     int32_t _68 = _48 + 11;
     int32_t _69 = _48 + 12;
     int32_t _70 = _48 + 13;
     int32_t _71 = _48 + 14;
     int32_t _72 = _48 + 15;
     int32_t _73 = _48 + 16;
     int32_t _74 = _48 + 17;
     int32_t _75 = _48 + 18;
     int32_t _76 = _48 + 19;
     int32_t _77 = _48 + 20;
     int32_t _78 = _48 + 21;
     int32_t _79 = _48 + 22;
     int32_t _80 = _48 + 23;
     int32_t _81 = _48 + 24;
     int32_t _82 = _48 + 25;
     int32_t _83 = _48 + 26;
     int32_t _84 = _48 + 27;
     int32_t _85 = _48 + 28;
     int32_t _86 = _48 + 29;
     int32_t _87 = _48 + 30;
     int32_t _88 = _48 + 31;
     int32_t _89 = _48 + 32;
     int32_t _90 = _48 + 33;
     int32_t _91 = _48 + 34;
     int32_t _92 = _48 + 35;
     int32_t _93 = _48 + 36;
     int32_t _94 = _48 + 37;
     int32_t _95 = _48 + 38;
     int32_t _96 = _48 + 39;
     int32_t _97 = _48 + 40;
     int32_t _98 = _48 + 41;
     int32_t _99 = _48 + 42;
     int32_t _100 = _48 + 43;
     int32_t _101 = _48 + 44;
     int32_t _102 = _48 + 45;
     int32_t _103 = _48 + 46;
     int32_t _104 = _48 + 47;
     int32_t _105 = _48 + 48;
     int32_t _106 = _48 + 49;
     int32_t _107 = _48 + 50;
     int32_t _108 = _48 + 51;
     int32_t _109 = _48 + 52;
     int32_t _110 = _48 + 53;
     int32_t _111 = _48 + 54;
     int32_t _112 = _48 + 55;
     int32_t _113 = _48 + 56;
     int32_t _114 = _48 + 57;
     int32_t _115 = _48 + 58;
     int32_t _116 = _48 + 59;
     int32_t _117 = _48 + 60;
     int32_t _118 = _48 + 61;
     int32_t _119 = _48 + 62;
     int32_t _120 = _48 + 63;
     int32_t _121 = _48 + 64;
     int32_t _122 = _48 + 65;
     int32_t _123 = _48 + 66;
     int32_t _124 = _48 + 67;
     int32_t _125 = _48 + 68;
     int32_t _126 = _48 + 69;
     int32_t _127 = _48 + 70;
     int32_t _128 = _48 + 71;
     int32_t _129 = _48 + 72;
     int32_t _130 = _48 + 73;
     int32_t _131 = _48 + 74;
     int32_t _132 = _48 + 75;
     int32_t _133 = _48 + 76;
     int32_t _134 = _48 + 77;
     int32_t _135 = _48 + 78;
     int32_t _136 = _48 + 79;
     int32_t _137 = _48 + 80;
     int32_t _138 = _48 + 81;
     int32_t _139 = _48 + 82;
     int32_t _140 = _48 + 83;
     int32_t _141 = _48 + 84;
     int32_t _142 = _48 + 85;
     int32_t _143 = _48 + 86;
     int32_t _144 = _48 + 87;
     int32_t _145 = _48 + 88;
     int32_t _146 = _48 + 89;
     int32_t _147 = _48 + 90;
     int32_t _148 = _48 + 91;
     int32_t _149 = _48 + 92;
     int32_t _150 = _48 + 93;
     int32_t _151 = _48 + 94;
     int32_t _152 = _48 + 95;
     int32_t _153 = _48 + 96;
     int32_t _154 = _48 + 97;
     int32_t _155 = _48 + 98;
     int32_t _156 = _48 + 99;
     int32_t _157 = _48 + 100;
     int32_t _158 = _48 + 101;
     int32_t _159 = _48 + 102;
     int32_t _160 = _48 + 103;
     int32_t _161 = _48 + 104;
     int32_t _162 = _48 + 105;
     int32_t _163 = _48 + 106;
     int32_t _164 = _48 + 107;
     v8acc80 out_0_acc;
     v16int32 loads_temporal_group_0 = undef_v16int32();
     int32_t _165 = _conv_s1_y * 16;
     int32_t _166 = _165 + _56;
     loads_temporal_group_0 = upd_w(loads_temporal_group_0, 0, *(v8int32* ) (_b0_buffer+_166+0));
     v8int32 loads_spatial_group_0 = *(v8int32* ) (_b1_buffer+_48);
     out_0_acc = lmul8(loads_temporal_group_0,0,0x76543210,loads_spatial_group_0,0,0);
     loads_temporal_group_0 = upd_w(loads_temporal_group_0, 1, *(v8int32* ) (_b0_buffer+_166+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_0,1,0x76543210,loads_spatial_group_0,1,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_0,2,0x76543210,loads_spatial_group_0,2,0);
     v16int32 loads_temporal_group_1 = undef_v16int32();
     int32_t _167 = _166 + 16;
     loads_temporal_group_1 = upd_w(loads_temporal_group_1, 0, *(v8int32* ) (_b0_buffer+_167+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_1,0,0x76543210,loads_spatial_group_0,3,0);
     loads_temporal_group_1 = upd_w(loads_temporal_group_1, 1, *(v8int32* ) (_b0_buffer+_167+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_1,1,0x76543210,loads_spatial_group_0,4,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_1,2,0x76543210,loads_spatial_group_0,5,0);
     v16int32 loads_temporal_group_2 = undef_v16int32();
     int32_t _168 = _166 + 32;
     loads_temporal_group_2 = upd_w(loads_temporal_group_2, 0, *(v8int32* ) (_b0_buffer+_168+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_2,0,0x76543210,loads_spatial_group_0,6,0);
     loads_temporal_group_2 = upd_w(loads_temporal_group_2, 1, *(v8int32* ) (_b0_buffer+_168+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_2,1,0x76543210,loads_spatial_group_0,7,0);
     v8int32 loads_spatial_group_1 = *(v8int32* ) (_b1_buffer+_65);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_2,2,0x76543210,loads_spatial_group_1,0,0);
     v16int32 loads_temporal_group_3 = undef_v16int32();
     int32_t _169 = _166 + 48;
     loads_temporal_group_3 = upd_w(loads_temporal_group_3, 0, *(v8int32* ) (_b0_buffer+_169+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_3,0,0x76543210,loads_spatial_group_1,1,0);
     loads_temporal_group_3 = upd_w(loads_temporal_group_3, 1, *(v8int32* ) (_b0_buffer+_169+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_3,1,0x76543210,loads_spatial_group_1,2,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_3,2,0x76543210,loads_spatial_group_1,3,0);
     v16int32 loads_temporal_group_4 = undef_v16int32();
     int32_t _170 = _166 + 64;
     loads_temporal_group_4 = upd_w(loads_temporal_group_4, 0, *(v8int32* ) (_b0_buffer+_170+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_4,0,0x76543210,loads_spatial_group_1,4,0);
     loads_temporal_group_4 = upd_w(loads_temporal_group_4, 1, *(v8int32* ) (_b0_buffer+_170+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_4,1,0x76543210,loads_spatial_group_1,5,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_4,2,0x76543210,loads_spatial_group_1,6,0);
     v16int32 loads_temporal_group_5 = undef_v16int32();
     int32_t _171 = _166 + 80;
     loads_temporal_group_5 = upd_w(loads_temporal_group_5, 0, *(v8int32* ) (_b0_buffer+_171+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_5,0,0x76543210,loads_spatial_group_1,7,0);
     loads_temporal_group_5 = upd_w(loads_temporal_group_5, 1, *(v8int32* ) (_b0_buffer+_171+8));
     v8int32 loads_spatial_group_2 = *(v8int32* ) (_b1_buffer+_73);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_5,1,0x76543210,loads_spatial_group_2,0,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_5,2,0x76543210,loads_spatial_group_2,1,0);
     v16int32 loads_temporal_group_6 = undef_v16int32();
     int32_t _172 = _166 + 96;
     loads_temporal_group_6 = upd_w(loads_temporal_group_6, 0, *(v8int32* ) (_b0_buffer+_172+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_6,0,0x76543210,loads_spatial_group_2,2,0);
     loads_temporal_group_6 = upd_w(loads_temporal_group_6, 1, *(v8int32* ) (_b0_buffer+_172+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_6,1,0x76543210,loads_spatial_group_2,3,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_6,2,0x76543210,loads_spatial_group_2,4,0);
     v16int32 loads_temporal_group_7 = undef_v16int32();
     int32_t _173 = _166 + 112;
     loads_temporal_group_7 = upd_w(loads_temporal_group_7, 0, *(v8int32* ) (_b0_buffer+_173+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_7,0,0x76543210,loads_spatial_group_2,5,0);
     loads_temporal_group_7 = upd_w(loads_temporal_group_7, 1, *(v8int32* ) (_b0_buffer+_173+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_7,1,0x76543210,loads_spatial_group_2,6,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_7,2,0x76543210,loads_spatial_group_2,7,0);
     v16int32 loads_temporal_group_8 = undef_v16int32();
     int32_t _174 = _166 + 128;
     loads_temporal_group_8 = upd_w(loads_temporal_group_8, 0, *(v8int32* ) (_b0_buffer+_174+0));
     v8int32 loads_spatial_group_3 = *(v8int32* ) (_b1_buffer+_81);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_8,0,0x76543210,loads_spatial_group_3,0,0);
     loads_temporal_group_8 = upd_w(loads_temporal_group_8, 1, *(v8int32* ) (_b0_buffer+_174+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_8,1,0x76543210,loads_spatial_group_3,1,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_8,2,0x76543210,loads_spatial_group_3,2,0);
     v16int32 loads_temporal_group_9 = undef_v16int32();
     int32_t _175 = _166 + 144;
     loads_temporal_group_9 = upd_w(loads_temporal_group_9, 0, *(v8int32* ) (_b0_buffer+_175+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_9,0,0x76543210,loads_spatial_group_3,3,0);
     loads_temporal_group_9 = upd_w(loads_temporal_group_9, 1, *(v8int32* ) (_b0_buffer+_175+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_9,1,0x76543210,loads_spatial_group_3,4,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_9,2,0x76543210,loads_spatial_group_3,5,0);
     v16int32 loads_temporal_group_10 = undef_v16int32();
     int32_t _176 = _166 + 160;
     loads_temporal_group_10 = upd_w(loads_temporal_group_10, 0, *(v8int32* ) (_b0_buffer+_176+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_10,0,0x76543210,loads_spatial_group_3,6,0);
     loads_temporal_group_10 = upd_w(loads_temporal_group_10, 1, *(v8int32* ) (_b0_buffer+_176+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_10,1,0x76543210,loads_spatial_group_3,7,0);
     v8int32 loads_spatial_group_4 = *(v8int32* ) (_b1_buffer+_89);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_10,2,0x76543210,loads_spatial_group_4,0,0);
     v16int32 loads_temporal_group_11 = undef_v16int32();
     int32_t _177 = _166 + 176;
     loads_temporal_group_11 = upd_w(loads_temporal_group_11, 0, *(v8int32* ) (_b0_buffer+_177+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_11,0,0x76543210,loads_spatial_group_4,1,0);
     loads_temporal_group_11 = upd_w(loads_temporal_group_11, 1, *(v8int32* ) (_b0_buffer+_177+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_11,1,0x76543210,loads_spatial_group_4,2,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_11,2,0x76543210,loads_spatial_group_4,3,0);
     v16int32 loads_temporal_group_12 = undef_v16int32();
     int32_t _178 = _166 + 192;
     loads_temporal_group_12 = upd_w(loads_temporal_group_12, 0, *(v8int32* ) (_b0_buffer+_178+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_12,0,0x76543210,loads_spatial_group_4,4,0);
     loads_temporal_group_12 = upd_w(loads_temporal_group_12, 1, *(v8int32* ) (_b0_buffer+_178+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_12,1,0x76543210,loads_spatial_group_4,5,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_12,2,0x76543210,loads_spatial_group_4,6,0);
     v16int32 loads_temporal_group_13 = undef_v16int32();
     int32_t _179 = _166 + 208;
     loads_temporal_group_13 = upd_w(loads_temporal_group_13, 0, *(v8int32* ) (_b0_buffer+_179+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_13,0,0x76543210,loads_spatial_group_4,7,0);
     loads_temporal_group_13 = upd_w(loads_temporal_group_13, 1, *(v8int32* ) (_b0_buffer+_179+8));
     v8int32 loads_spatial_group_5 = *(v8int32* ) (_b1_buffer+_97);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_13,1,0x76543210,loads_spatial_group_5,0,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_13,2,0x76543210,loads_spatial_group_5,1,0);
     v16int32 loads_temporal_group_14 = undef_v16int32();
     int32_t _180 = _166 + 224;
     loads_temporal_group_14 = upd_w(loads_temporal_group_14, 0, *(v8int32* ) (_b0_buffer+_180+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_14,0,0x76543210,loads_spatial_group_5,2,0);
     loads_temporal_group_14 = upd_w(loads_temporal_group_14, 1, *(v8int32* ) (_b0_buffer+_180+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_14,1,0x76543210,loads_spatial_group_5,3,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_14,2,0x76543210,loads_spatial_group_5,4,0);
     v16int32 loads_temporal_group_15 = undef_v16int32();
     int32_t _181 = _166 + 240;
     loads_temporal_group_15 = upd_w(loads_temporal_group_15, 0, *(v8int32* ) (_b0_buffer+_181+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_15,0,0x76543210,loads_spatial_group_5,5,0);
     loads_temporal_group_15 = upd_w(loads_temporal_group_15, 1, *(v8int32* ) (_b0_buffer+_181+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_15,1,0x76543210,loads_spatial_group_5,6,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_15,2,0x76543210,loads_spatial_group_5,7,0);
     v16int32 loads_temporal_group_16 = undef_v16int32();
     int32_t _182 = _166 + 256;
     loads_temporal_group_16 = upd_w(loads_temporal_group_16, 0, *(v8int32* ) (_b0_buffer+_182+0));
     v8int32 loads_spatial_group_6 = *(v8int32* ) (_b1_buffer+_105);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_16,0,0x76543210,loads_spatial_group_6,0,0);
     loads_temporal_group_16 = upd_w(loads_temporal_group_16, 1, *(v8int32* ) (_b0_buffer+_182+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_16,1,0x76543210,loads_spatial_group_6,1,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_16,2,0x76543210,loads_spatial_group_6,2,0);
     v16int32 loads_temporal_group_17 = undef_v16int32();
     int32_t _183 = _166 + 272;
     loads_temporal_group_17 = upd_w(loads_temporal_group_17, 0, *(v8int32* ) (_b0_buffer+_183+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_17,0,0x76543210,loads_spatial_group_6,3,0);
     loads_temporal_group_17 = upd_w(loads_temporal_group_17, 1, *(v8int32* ) (_b0_buffer+_183+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_17,1,0x76543210,loads_spatial_group_6,4,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_17,2,0x76543210,loads_spatial_group_6,5,0);
     v16int32 loads_temporal_group_18 = undef_v16int32();
     int32_t _184 = _166 + 288;
     loads_temporal_group_18 = upd_w(loads_temporal_group_18, 0, *(v8int32* ) (_b0_buffer+_184+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_18,0,0x76543210,loads_spatial_group_6,6,0);
     loads_temporal_group_18 = upd_w(loads_temporal_group_18, 1, *(v8int32* ) (_b0_buffer+_184+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_18,1,0x76543210,loads_spatial_group_6,7,0);
     v8int32 loads_spatial_group_7 = *(v8int32* ) (_b1_buffer+_113);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_18,2,0x76543210,loads_spatial_group_7,0,0);
     v16int32 loads_temporal_group_19 = undef_v16int32();
     int32_t _185 = _166 + 304;
     loads_temporal_group_19 = upd_w(loads_temporal_group_19, 0, *(v8int32* ) (_b0_buffer+_185+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_19,0,0x76543210,loads_spatial_group_7,1,0);
     loads_temporal_group_19 = upd_w(loads_temporal_group_19, 1, *(v8int32* ) (_b0_buffer+_185+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_19,1,0x76543210,loads_spatial_group_7,2,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_19,2,0x76543210,loads_spatial_group_7,3,0);
     v16int32 loads_temporal_group_20 = undef_v16int32();
     int32_t _186 = _166 + 320;
     loads_temporal_group_20 = upd_w(loads_temporal_group_20, 0, *(v8int32* ) (_b0_buffer+_186+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_20,0,0x76543210,loads_spatial_group_7,4,0);
     loads_temporal_group_20 = upd_w(loads_temporal_group_20, 1, *(v8int32* ) (_b0_buffer+_186+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_20,1,0x76543210,loads_spatial_group_7,5,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_20,2,0x76543210,loads_spatial_group_7,6,0);
     v16int32 loads_temporal_group_21 = undef_v16int32();
     int32_t _187 = _166 + 336;
     loads_temporal_group_21 = upd_w(loads_temporal_group_21, 0, *(v8int32* ) (_b0_buffer+_187+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_21,0,0x76543210,loads_spatial_group_7,7,0);
     loads_temporal_group_21 = upd_w(loads_temporal_group_21, 1, *(v8int32* ) (_b0_buffer+_187+8));
     v8int32 loads_spatial_group_8 = *(v8int32* ) (_b1_buffer+_121);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_21,1,0x76543210,loads_spatial_group_8,0,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_21,2,0x76543210,loads_spatial_group_8,1,0);
     v16int32 loads_temporal_group_22 = undef_v16int32();
     int32_t _188 = _166 + 352;
     loads_temporal_group_22 = upd_w(loads_temporal_group_22, 0, *(v8int32* ) (_b0_buffer+_188+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_22,0,0x76543210,loads_spatial_group_8,2,0);
     loads_temporal_group_22 = upd_w(loads_temporal_group_22, 1, *(v8int32* ) (_b0_buffer+_188+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_22,1,0x76543210,loads_spatial_group_8,3,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_22,2,0x76543210,loads_spatial_group_8,4,0);
     v16int32 loads_temporal_group_23 = undef_v16int32();
     int32_t _189 = _166 + 368;
     loads_temporal_group_23 = upd_w(loads_temporal_group_23, 0, *(v8int32* ) (_b0_buffer+_189+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_23,0,0x76543210,loads_spatial_group_8,5,0);
     loads_temporal_group_23 = upd_w(loads_temporal_group_23, 1, *(v8int32* ) (_b0_buffer+_189+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_23,1,0x76543210,loads_spatial_group_8,6,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_23,2,0x76543210,loads_spatial_group_8,7,0);
     v16int32 loads_temporal_group_24 = undef_v16int32();
     int32_t _190 = _166 + 384;
     loads_temporal_group_24 = upd_w(loads_temporal_group_24, 0, *(v8int32* ) (_b0_buffer+_190+0));
     v8int32 loads_spatial_group_9 = *(v8int32* ) (_b1_buffer+_129);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_24,0,0x76543210,loads_spatial_group_9,0,0);
     loads_temporal_group_24 = upd_w(loads_temporal_group_24, 1, *(v8int32* ) (_b0_buffer+_190+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_24,1,0x76543210,loads_spatial_group_9,1,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_24,2,0x76543210,loads_spatial_group_9,2,0);
     v16int32 loads_temporal_group_25 = undef_v16int32();
     int32_t _191 = _166 + 400;
     loads_temporal_group_25 = upd_w(loads_temporal_group_25, 0, *(v8int32* ) (_b0_buffer+_191+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_25,0,0x76543210,loads_spatial_group_9,3,0);
     loads_temporal_group_25 = upd_w(loads_temporal_group_25, 1, *(v8int32* ) (_b0_buffer+_191+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_25,1,0x76543210,loads_spatial_group_9,4,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_25,2,0x76543210,loads_spatial_group_9,5,0);
     v16int32 loads_temporal_group_26 = undef_v16int32();
     int32_t _192 = _166 + 416;
     loads_temporal_group_26 = upd_w(loads_temporal_group_26, 0, *(v8int32* ) (_b0_buffer+_192+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_26,0,0x76543210,loads_spatial_group_9,6,0);
     loads_temporal_group_26 = upd_w(loads_temporal_group_26, 1, *(v8int32* ) (_b0_buffer+_192+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_26,1,0x76543210,loads_spatial_group_9,7,0);
     v8int32 loads_spatial_group_10 = *(v8int32* ) (_b1_buffer+_137);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_26,2,0x76543210,loads_spatial_group_10,0,0);
     v16int32 loads_temporal_group_27 = undef_v16int32();
     int32_t _193 = _166 + 432;
     loads_temporal_group_27 = upd_w(loads_temporal_group_27, 0, *(v8int32* ) (_b0_buffer+_193+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_27,0,0x76543210,loads_spatial_group_10,1,0);
     loads_temporal_group_27 = upd_w(loads_temporal_group_27, 1, *(v8int32* ) (_b0_buffer+_193+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_27,1,0x76543210,loads_spatial_group_10,2,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_27,2,0x76543210,loads_spatial_group_10,3,0);
     v16int32 loads_temporal_group_28 = undef_v16int32();
     int32_t _194 = _166 + 448;
     loads_temporal_group_28 = upd_w(loads_temporal_group_28, 0, *(v8int32* ) (_b0_buffer+_194+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_28,0,0x76543210,loads_spatial_group_10,4,0);
     loads_temporal_group_28 = upd_w(loads_temporal_group_28, 1, *(v8int32* ) (_b0_buffer+_194+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_28,1,0x76543210,loads_spatial_group_10,5,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_28,2,0x76543210,loads_spatial_group_10,6,0);
     v16int32 loads_temporal_group_29 = undef_v16int32();
     int32_t _195 = _166 + 464;
     loads_temporal_group_29 = upd_w(loads_temporal_group_29, 0, *(v8int32* ) (_b0_buffer+_195+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_29,0,0x76543210,loads_spatial_group_10,7,0);
     loads_temporal_group_29 = upd_w(loads_temporal_group_29, 1, *(v8int32* ) (_b0_buffer+_195+8));
     v8int32 loads_spatial_group_11 = *(v8int32* ) (_b1_buffer+_145);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_29,1,0x76543210,loads_spatial_group_11,0,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_29,2,0x76543210,loads_spatial_group_11,1,0);
     v16int32 loads_temporal_group_30 = undef_v16int32();
     int32_t _196 = _166 + 480;
     loads_temporal_group_30 = upd_w(loads_temporal_group_30, 0, *(v8int32* ) (_b0_buffer+_196+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_30,0,0x76543210,loads_spatial_group_11,2,0);
     loads_temporal_group_30 = upd_w(loads_temporal_group_30, 1, *(v8int32* ) (_b0_buffer+_196+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_30,1,0x76543210,loads_spatial_group_11,3,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_30,2,0x76543210,loads_spatial_group_11,4,0);
     v16int32 loads_temporal_group_31 = undef_v16int32();
     int32_t _197 = _166 + 496;
     loads_temporal_group_31 = upd_w(loads_temporal_group_31, 0, *(v8int32* ) (_b0_buffer+_197+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_31,0,0x76543210,loads_spatial_group_11,5,0);
     loads_temporal_group_31 = upd_w(loads_temporal_group_31, 1, *(v8int32* ) (_b0_buffer+_197+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_31,1,0x76543210,loads_spatial_group_11,6,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_31,2,0x76543210,loads_spatial_group_11,7,0);
     v16int32 loads_temporal_group_32 = undef_v16int32();
     int32_t _198 = _166 + 512;
     loads_temporal_group_32 = upd_w(loads_temporal_group_32, 0, *(v8int32* ) (_b0_buffer+_198+0));
     v8int32 loads_spatial_group_12 = *(v8int32* ) (_b1_buffer+_153);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_32,0,0x76543210,loads_spatial_group_12,0,0);
     loads_temporal_group_32 = upd_w(loads_temporal_group_32, 1, *(v8int32* ) (_b0_buffer+_198+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_32,1,0x76543210,loads_spatial_group_12,1,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_32,2,0x76543210,loads_spatial_group_12,2,0);
     v16int32 loads_temporal_group_33 = undef_v16int32();
     int32_t _199 = _166 + 528;
     loads_temporal_group_33 = upd_w(loads_temporal_group_33, 0, *(v8int32* ) (_b0_buffer+_199+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_33,0,0x76543210,loads_spatial_group_12,3,0);
     loads_temporal_group_33 = upd_w(loads_temporal_group_33, 1, *(v8int32* ) (_b0_buffer+_199+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_33,1,0x76543210,loads_spatial_group_12,4,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_33,2,0x76543210,loads_spatial_group_12,5,0);
     v16int32 loads_temporal_group_34 = undef_v16int32();
     int32_t _200 = _166 + 544;
     loads_temporal_group_34 = upd_w(loads_temporal_group_34, 0, *(v8int32* ) (_b0_buffer+_200+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_34,0,0x76543210,loads_spatial_group_12,6,0);
     loads_temporal_group_34 = upd_w(loads_temporal_group_34, 1, *(v8int32* ) (_b0_buffer+_200+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_34,1,0x76543210,loads_spatial_group_12,7,0);
     v8int32 loads_spatial_group_13 = *(v8int32* ) (_b1_buffer+_161);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_34,2,0x76543210,loads_spatial_group_13,0,0);
     v16int32 loads_temporal_group_35 = undef_v16int32();
     int32_t _201 = _166 + 560;
     loads_temporal_group_35 = upd_w(loads_temporal_group_35, 0, *(v8int32* ) (_b0_buffer+_201+0));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_35,0,0x76543210,loads_spatial_group_13,1,0);
     loads_temporal_group_35 = upd_w(loads_temporal_group_35, 1, *(v8int32* ) (_b0_buffer+_201+8));
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_35,1,0x76543210,loads_spatial_group_13,2,0);
     out_0_acc = lmac8(out_0_acc,loads_temporal_group_35,2,0x76543210,loads_spatial_group_13,3,0);
     int32_t _202 = _conv_s1_y * _32;
     int32_t _203 = _202 + _57;
     *(v8int32*)(_conv_buffer+_203) = srs(out_0_acc, XHALIDE_SRS_SHIFT);
    } // for _conv_s1_y
   } // for _conv_s1_x_x
  } // for _conv_s1_n
 } // for _conv_s1_z
 return 0;
}
