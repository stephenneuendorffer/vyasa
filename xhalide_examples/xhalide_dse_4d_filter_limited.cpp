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

Var x("x"), y("y"), z("z"), n("n"), t("t"), x_i("xi"), x_o("xo"), y_i("y_i"), y_o("y_o");
Var l_o("l_o"), l_i("l_i");
Var x_tile("x_tile"), c_tile("c_tile");

int image_width, image_height, image_channels, nimages;
int filter_width, filter_height, filter_channels, nfilters;
int convolved_output_width, convolved_output_height;

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

int get_variable_range(char letter) {
    if(letter == 'x')
        return convolved_output_width;
    else if(letter == 'y')
        return convolved_output_height;
    else if(letter == 'z')
        return nfilters;
    else if(letter == 'n')
        return nimages;
    else {
        cout<<"Not able to find Halide variable for the letter" << letter << endl;
        assert(false);
    }
}

int main(int argc, char **argv) {

    assert(argc == 15);
    image_width = atoi(argv[1]); //e.g., 48
    image_height = atoi(argv[2]); //e.g., 10;
    image_channels = atoi(argv[3]); //e.g., 4;
    nimages = atoi(argv[4]); //e.g., 2;

            
    filter_width = atoi(argv[5]); //e.g., 3
    filter_height = atoi(argv[6]); //e.g., 3;
    filter_channels = atoi(argv[7]); // e.g., image_channels;
    assert(filter_channels == image_channels);
    nfilters = atoi(argv[8]); //e.g., 2

    convolved_output_width = atoi(argv[9]); //e.g., 40;
    convolved_output_height = atoi(argv[10]); //e.g., 8;

    char *loop_order = argv[11];
    int unroll_jam_size_x = atoi(argv[12]);
    int unroll_jam_size_y = atoi(argv[13]);
    int input_data_tile_channels = atoi(argv[14]);

    assert(image_width % VLEN == 0);

    Buffer<TYPE>  input(input_data_tile_channels, image_width,
                           image_channels/input_data_tile_channels,
                           image_height, nimages);

    int filter_data_tile_channels = input_data_tile_channels;
    Buffer<TYPE>  filter(filter_data_tile_channels, filter_width,
                            filter_channels/ filter_data_tile_channels,
                            filter_height, nfilters);


    Func f_conv_opt("conv");
    RDom r(filter.dim(0).min(), filter.dim(0).extent(), //r.x is inside a tile channels!
           filter.dim(1).min(), filter.dim(1).extent(), // across width
           filter.dim(2).min(), filter.dim(2).extent(),
           filter.dim(3).min(), filter.dim(3).extent());

    f_conv_opt(x, y, z, n) +=
            filter(r.x, r.y, r.z, r.w, z)
            * input(r.x, x + r.y, r.z, y + r.w, n);


    f_conv_opt.bound(get_variable(loop_order[0]), 0, get_variable_range(loop_order[0]));
    f_conv_opt.bound(get_variable(loop_order[1]), 0, get_variable_range(loop_order[1]));
    f_conv_opt.bound(get_variable(loop_order[2]), 0, get_variable_range(loop_order[2]));
    f_conv_opt.bound(get_variable(loop_order[3]), 0, get_variable_range(loop_order[3]));
    f_conv_opt.update()
            .vectorize(x, VLEN)
            .unroll(r.x,filter.dim(0).extent())
            .unroll(r.y, filter.dim(1).extent())
            .unroll(r.z, filter.dim(2).extent())
            .unroll(r.w, filter.dim(3).extent())
            .tile(x, y, x_i, y_i, unroll_jam_size_x, unroll_jam_size_y)
            .unroll(x_i)
            .unroll(y_i)
            .prepare_for_software_pipelining(get_variable(loop_order[0]))
            .reorder(get_variable(loop_order[0]),
                     get_variable(loop_order[1]),
                     get_variable(loop_order[2]),
                     get_variable(loop_order[3]));

    Target target = Target(Target::OS::Linux, Target::Arch::AI, 32,
                           {Target::NoAsserts, Target::NoBoundsQuery});

    f_conv_opt.compile_to_c("xhalide_dse_4d_filter_generated.cc",
                            std::vector<Argument>(), "conv", target);

    printf("Success!\n");
    return 0;
}
