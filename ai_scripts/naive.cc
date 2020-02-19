// Copyright (c) 2019 Xilinx Inc. All Rights Reserved.

#include "global.h"

int conv7D_naive(int32_t  * restrict _b0_buffer, int _b0_buffer_dim0_min, int _b0_buffer_dim0_extent, int _b0_buffer_dim0_stride, int _b0_buffer_dim1_min, int _b0_buffer_dim1_extent, int _b0_buffer_dim1_stride, int _b0_buffer_dim2_min, int _b0_buffer_dim2_extent, int _b0_buffer_dim2_stride, int _b0_buffer_dim3_min, int _b0_buffer_dim3_extent, int _b0_buffer_dim3_stride,
int32_t  * restrict _b1_buffer, int _b1_buffer_dim0_min, int _b1_buffer_dim0_extent, int _b1_buffer_dim0_stride, int _b1_buffer_dim1_min, int _b1_buffer_dim1_extent, int _b1_buffer_dim1_stride, int _b1_buffer_dim2_min, int _b1_buffer_dim2_extent, int _b1_buffer_dim2_stride, int _b1_buffer_dim3_min, int _b1_buffer_dim3_extent, int _b1_buffer_dim3_stride,
int32_t  * restrict _conv_buffer, int _conv_buffer_dim0_min, int _conv_buffer_dim0_extent, int _conv_buffer_dim0_stride, int _conv_buffer_dim1_min, int _conv_buffer_dim1_extent, int _conv_buffer_dim1_stride, int _conv_buffer_dim2_min, int _conv_buffer_dim2_extent, int _conv_buffer_dim2_stride, int _conv_buffer_dim3_min, int _conv_buffer_dim3_extent, int _conv_buffer_dim3_stride) {

    for (int n = 0; n < _conv_buffer_dim3_extent; n++) {
        for (int c = 0; c < _conv_buffer_dim2_extent; c++) {
            for (int j = 0; j < _conv_buffer_dim1_extent; j++) {
                for (int i = 0; i < _conv_buffer_dim0_extent; i++) {
                    int acc = 0;
                    for (int z = 0; z < _b1_buffer_dim2_extent; z++) {
                        for (int y = 0; y < _b1_buffer_dim1_extent; y++) {
                            for (int x = 0; x < _b1_buffer_dim0_extent; x++) {
                                int input_index = (i+x) + (j+y) * (_b0_buffer_dim1_stride)
                                                        + z * (_b0_buffer_dim2_stride)
                                                        + n * (_b0_buffer_dim3_stride);

                                int filter_index = (x) + (y) * (_b1_buffer_dim1_stride)
                                                        + z * (_b1_buffer_dim2_stride)
                                                        + c * (_b1_buffer_dim3_stride);
                                acc += _b0_buffer[input_index] * _b1_buffer[filter_index];
                            }
                        }
                    }

                    int index = i + j * _conv_buffer_dim1_stride
                                  + c * (_conv_buffer_dim2_stride)
                                  + n * (_conv_buffer_dim3_stride);
                    _conv_buffer[index] = acc >> SRS_SHIFT ;
                }
            }
        }
    }

}
