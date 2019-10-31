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

Var x("x"), y("y"), z("z"), n("n");

Var get_variable(char letter) {
    if(letter == 'x')
        return x;
    else if(letter == 'y')
        return y;
    else if(letter == 'z')
        return z;
    else if(letter == 'n')
        return n;
    else {
        cout<<"Not able to find Halide variable for the letter" << letter << endl;
        assert(false);
    }
}
int main(int argc, char **argv) {

    assert(argc == 12);
    int image_width = atoi(argv[1]); //e.g., 48
    int image_height = atoi(argv[2]); //e.g., 10;
    int image_channels = atoi(argv[3]); //e.g., 4;
    int nimages = atoi(argv[4]); //e.g., 2;

            
    int filter_width = atoi(argv[5]); //e.g., 3
    int filter_height = atoi(argv[6]); //e.g., 3;
    int filter_channels = atoi(argv[7]); // e.g., image_channels;
    assert(filter_channels == image_channels);
    int nfilters = atoi(argv[8]); //e.g., 2

    int convolved_output_width = atoi(argv[9]); //e.g., 40;
    int convolved_output_height = atoi(argv[10]); //e.g., 8;

    char *loop_order = argv[11];

    Buffer<int32_t>  input(image_width, image_height, image_channels, nimages);
    Buffer<int32_t>  filter(filter_width, filter_height, filter_channels, nfilters);
    //ImageParam input(type_of<int32_t>(), 4);
    //ImageParam filter(type_of<int32_t>(), 4);


    Func f_conv("conv");
    RDom r(filter.dim(0).min(), filter.dim(0).extent(),
           filter.dim(1).min(), filter.dim(1).extent(),
           filter.dim(2).min(), filter.dim(2).extent());

    f_conv(x, y, z, n) += filter(r.x, r.y, r.z, z) * input(x + r.x, y + r.y, r.z, n);

    int vlen = 8;
    f_conv.update()
            .vectorize(x, vlen)
            .unroll(r.x,filter_width)
            .unroll(r.y, filter_height)
            .unroll(r.z, filter_channels)
            .prepare_for_software_pipelining(get_variable(loop_order[0]))
            .reorder(get_variable(loop_order[0]),
                     get_variable(loop_order[1]),
                     get_variable(loop_order[2]),
                     get_variable(loop_order[3]));


    Target target = Target(Target::OS::Linux, Target::Arch::AI, 32, {Target::NoAsserts, Target::NoBoundsQuery});
    f_conv.compile_to_c("xhalide_generated.cc", std::vector<Argument>(), "conv", target);
    //f_conv.compile_to_c("convolution.c", {input, filter}, "conv", target);

    printf("Success!\n");
    return 0;
}
