// Copyright (c) 2019 Xilinx Inc. All Rights Reserved.

#ifndef _FILTER_2D_H_
#define _FILTER_2D_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define PARALLEL_FACTOR_32b 8   // Parallelization factor for 32b operations (8x mults)
#define SRS_SHIFT 10            // SRS shift used can be increased if input data likewise adjusted)

int conv7D_naive(int32_t  * restrict _b0_buffer, int _b0_buffer_dim0_min, int _b0_buffer_dim0_extent, int _b0_buffer_dim0_stride, int _b0_buffer_dim1_min, int _b0_buffer_dim1_extent, int _b0_buffer_dim1_stride, int _b0_buffer_dim2_min, int _b0_buffer_dim2_extent, int _b0_buffer_dim2_stride, int _b0_buffer_dim3_min, int _b0_buffer_dim3_extent, int _b0_buffer_dim3_stride,
int32_t  * restrict _b1_buffer, int _b1_buffer_dim0_min, int _b1_buffer_dim0_extent, int _b1_buffer_dim0_stride, int _b1_buffer_dim1_min, int _b1_buffer_dim1_extent, int _b1_buffer_dim1_stride, int _b1_buffer_dim2_min, int _b1_buffer_dim2_extent, int _b1_buffer_dim2_stride, int _b1_buffer_dim3_min, int _b1_buffer_dim3_extent, int _b1_buffer_dim3_stride,
int32_t  * restrict _conv_buffer, int _conv_buffer_dim0_min, int _conv_buffer_dim0_extent, int _conv_buffer_dim0_stride, int _conv_buffer_dim1_min, int _conv_buffer_dim1_extent, int _conv_buffer_dim1_stride, int _conv_buffer_dim2_min, int _conv_buffer_dim2_extent, int _conv_buffer_dim2_stride, int _conv_buffer_dim3_min, int _conv_buffer_dim3_extent, int _conv_buffer_dim3_stride);


int conv(int32_t  * restrict _b0_buffer, int _b0_buffer_dim0_min, int _b0_buffer_dim0_extent, int _b0_buffer_dim0_stride, int _b0_buffer_dim1_min, int _b0_buffer_dim1_extent, int _b0_buffer_dim1_stride, int _b0_buffer_dim2_min, int _b0_buffer_dim2_extent, int _b0_buffer_dim2_stride, int _b0_buffer_dim3_min, int _b0_buffer_dim3_extent, int _b0_buffer_dim3_stride,
int32_t  * restrict _b1_buffer, int _b1_buffer_dim0_min, int _b1_buffer_dim0_extent, int _b1_buffer_dim0_stride, int _b1_buffer_dim1_min, int _b1_buffer_dim1_extent, int _b1_buffer_dim1_stride, int _b1_buffer_dim2_min, int _b1_buffer_dim2_extent, int _b1_buffer_dim2_stride, int _b1_buffer_dim3_min, int _b1_buffer_dim3_extent, int _b1_buffer_dim3_stride,
int32_t  * restrict _conv_buffer, int _conv_buffer_dim0_min, int _conv_buffer_dim0_extent, int _conv_buffer_dim0_stride, int _conv_buffer_dim1_min, int _conv_buffer_dim1_extent, int _conv_buffer_dim1_stride, int _conv_buffer_dim2_min, int _conv_buffer_dim2_extent, int _conv_buffer_dim2_stride, int _conv_buffer_dim3_min, int _conv_buffer_dim3_extent, int _conv_buffer_dim3_stride);
#endif
