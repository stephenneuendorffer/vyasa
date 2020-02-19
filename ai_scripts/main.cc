// Copyright (c) 2019 Xilinx Inc. All Rights Reserved.

#include "global.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_IMAGE_SIZE 32768    // Test image are small (enough for 128x128 or 256x16)
#define MAX_KERNEL_SIZE 400     // Assume max kernel size is somethign like 20*20
#define VEC PARALLEL_FACTOR_32b // Vectorization factor

int32_t in_data[MAX_IMAGE_SIZE];
int32_t out_data[MAX_IMAGE_SIZE];
int32_t kernel_data[MAX_KERNEL_SIZE];
int32_t out_data_ref[MAX_IMAGE_SIZE];

int main(){
    int i=0, j=0, k=0;

    // Load kernel coefficients from input file stream
    int kernel_width = get_ss(0);
    int kernel_height = get_ss(0);
    int kernel_channels = get_ss(0);
    int nkernels = get_ss(0);


    int input_image_width       = get_ss(0);
    int output_image_width = get_ss(0);
    int input_image_height      = get_ss(0);
    int output_image_height = get_ss(0);
    int input_image_channels    = get_ss(0);
    int output_image_channels = nkernels;
    int input_nimages          = get_ss(0);
    int output_nimages = input_nimages;

    int index = 0;
    for(int n = 0; n < nkernels; n++) {
        for (int z = 0; z < kernel_channels; z++) {
            for (int y = 0; y < kernel_height; y++) {
                for (int x = 0; x < kernel_width; x++) {
                    kernel_data[index++] = get_ss(0);
                }
            }
        }
    }

    kernel_data[index] = 0;

    // Print kernel coefficent values
    printf("kernel info: %d, %d, %d, %d\n", nkernels, kernel_channels, kernel_height, kernel_width);

    for(i=0; i < nkernels * kernel_channels * kernel_height * kernel_width; i++){
        printf("%d ",kernel_data[i]);
    }
    printf("\n");





    // Print image size
    printf("width: %d, height: %d, channels: %d, nimages: %d\n",
            input_image_width, input_image_height, input_image_channels, input_nimages);

    // Read in image to local memory
    index = 0;
    for(int n = 0; n < input_nimages; n++) {
        for (int z = 0; z < input_image_channels; z++) {
            for (int y = 0; y < input_image_height; y++) {
                for (int x = 0; x < output_image_width+kernel_width-1; x++) {
                    in_data[index++] = get_ss(0);
                }

                for(j=output_image_width+kernel_width-1; j < input_image_width; j++) {
                    in_data[index++] = 0;
                }
            }
        }
    }


    // Compute 7D Convolution
    conv7D_naive(in_data, 0, input_image_width, 1,
                   0, input_image_height, input_image_width,
                   0, input_image_channels, input_image_width * input_image_height,
                   0, input_nimages, input_image_width * input_image_height * input_image_channels,
          kernel_data, 0, kernel_width, 1,
                       0, kernel_height, kernel_width,
                       0, kernel_channels, kernel_width * kernel_height,
                       0, nkernels, kernel_width * kernel_height * kernel_channels,
         out_data_ref, 0, output_image_width, 1,
                   0, output_image_height, output_image_width,
                   0, output_image_channels, output_image_width * output_image_height,
                   0, output_nimages, output_image_width * output_image_height * output_image_channels);


    conv(in_data, 0, input_image_width, 1,
                   0, input_image_height, input_image_width,
                   0, input_image_channels, input_image_width * input_image_height,
                   0, input_nimages, input_image_width * input_image_height * input_image_channels,
          kernel_data, 0, kernel_width, 1,
                       0, kernel_height, kernel_width,
                       0, kernel_channels, kernel_width * kernel_height,
                       0, nkernels, kernel_width * kernel_height * kernel_channels,
         out_data, 0, output_image_width, 1,
                   0, output_image_height, output_image_width,
                   0, output_image_channels, output_image_width * output_image_height,
                   0, output_nimages, output_image_width * output_image_height * output_image_channels);

    // Compare results with reference image
    // NOTE: There will be some rounding errors in results so we accept absolute value differences < 5

/*    int refImageWidth       = get_ss(1);
    int refImageHeight      = get_ss(1);
    int refImageChannels    = get_ss(1);
    int refImages          = get_ss(1); */
    int errors = 0;
    int max_error = 0;
    int absErrorDiff = 0;

/*    for(int i = 0; i < refImageWidth * refImageHeight * refImageChannels * refImages; i++) {
        int32_t ref_output = get_ss(1);
        if(out_data[i] != ref_output){
            printf("Delta found: Index %d is %d and should be %d\n", i, out_data[i], ref_output);
            absErrorDiff = abs(out_data[i] - ref_output);
            if(absErrorDiff > max_error) printf("max found in index: %d\n",i);
            max_error = max_error < absErrorDiff ? absErrorDiff : max_error;
            errors++;
        }
    } */

    for(int i = 0; i < output_image_width * output_image_height * output_image_channels * output_nimages; i++) {
        int32_t ref_output = out_data_ref[i];
        if((out_data[i] >> 0) != ref_output){
            printf("Delta found: Index %d is %d and should be %d\n", i, out_data[i], ref_output);
            absErrorDiff = abs(out_data[i] - ref_output);
            if(absErrorDiff > max_error) printf("max found in index: %d\n",i);
            max_error = max_error < absErrorDiff ? absErrorDiff : max_error;
            errors++;
        }
    }

    if(errors==0 || max_error < 5){
        printf("PASSED, Max delta: %d, pixel intensity\n\n", max_error);
    }else{
        printf("FAIL. Number of deltas = %d, Max delta: %d, pixel intensity\n\n", errors, max_error);
    }

    return 0;
}
