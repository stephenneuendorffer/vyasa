// (c) Copyright 2019 Xilinx Inc. All Rights Reserved.
// On linux, you can compile and run it like so:
// g++ xhalide-blur.cpp -g -I ../include -L ../bin -lHalide -lpthread -ldl -o lesson_01 -std=c++11
// LD_LIBRARY_PATH=../bin ./lesson_01

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"
#include "halide_image_io.h"
#include <fstream>
using namespace Halide::Tools;

#include <iostream>
using namespace std;

// This time we'll just import the entire Halide namespace
using namespace Halide;

// We'll also include stdio for printf.
#include <stdio.h>

static inline int shift_float(float x) {
    const int shift = 256;
    return (int) (x * 256);
}

int main(int argc, char **argv) {

    int image_width = 272; //Multiples of 4 elements -- 128 bits //Also, multiples of 8 for full vectorization
    int image_height = 16;
    int image_channels = 1;
    int filter_width = 3;
    int filter_height = 3;
    int filter_channels = 1;


    Buffer<int32_t>  input(image_width, image_height);
    Buffer<int32_t>  filter(filter_width, filter_height);

    string read_data;
    ifstream input_stream ("input.txt");
    for (int y = 0; y < input.height(); y++) {
        for (int x = 0; x < input.width(); x++) {
            //input(x, y) = rand() & 0xfff;
            if(getline(input_stream, read_data)) {
                input(x, y) = (int32_t) stoi(read_data);
            } else {
                printf("Reading image data failed:\n");
                exit(0);
            }
        }
    }

    for (int y = 0; y < filter.height(); y++) {
        for (int x = 0; x < filter.width(); x++) {
            //input(x, y) = rand() & 0xfff;
            if(getline(input_stream, read_data)) {
                filter(x, y) = (int32_t) stoi(read_data);
            } else {
                printf("Reading filter data failed:\n");
                exit(0);
            }
        }
    }

    input_stream.close();



    Var x("x"), y("y"), z("z"), n("n");
    Var xo("xo"), xi("xi");

    Func f_conv("conv");
    RDom r(filter.dim(0).min(), filter.dim(0).extent(),
           filter.dim(1).min(), filter.dim(1).extent());

    //f_conv(y,x) += filter(r.y, r.x) * input(y + r.y, x + r.x);

    f_conv(x, y) += filter(r.x, r.y) * input(x + r.x, y + r.y);

    int vlen = 8;
    f_conv.update().vectorize(x,vlen).unroll(r.x,filter_width).unroll(r.y, filter_height);  //.reorder(y,x);//.unroll(x,2);//.reorder(y,x);

    Target target = Target(Target::OS::Linux, Target::Arch::AI, 32, {Target::NoAsserts, Target::NoBoundsQuery});
    f_conv.compile_to_c("convolution.c", std::vector<Argument>(), "conv", target);

    // JIT compilation to get outputs
/*    Halide::Buffer<int32_t> output = f_conv.realize(
            image_width-filter_width+1, image_height-filter_height+1);

    std::ofstream globals_stream("output.txt");
    for (int j = 0; j < output.height(); j++) {
        for (int i = 0; i < output.width(); i++) {
            globals_stream << output(i, j) << " \n";
        }
    } */

    printf("Success!\n");
    return 0;
}
