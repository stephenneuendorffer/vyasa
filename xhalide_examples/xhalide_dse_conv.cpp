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

/*#define TYPE int32_t
#define VLEN 8*/

Var x("x"), y("y"), z("z"), n("n"), t("t"), x_i("xi"), x_o("xo"), y_i("y_i"), y_o("y_o");
Var l_o("l_o"), l_i("l_i");
Var x_tile("x_tile"), c_tile("c_tile");
Var var_output_activations_dim_0;
Var var_output_activations_dim_1;
Var var_output_activations_dim_2;
Var var_output_activations_dim_3;
Var var_output_activations_dim_4;
Var var_output_activations_dim_5;
Var var_output_activations_dim_6;
Var var_output_activations_dim_7;

Var var_output_activations_inter_tile_dim_0;
Var var_output_activations_inter_tile_dim_1;
Var var_output_activations_inter_tile_dim_2;
Var var_output_activations_inter_tile_dim_3;
Var var_output_activations_inter_tile_dim_4;
Var var_output_activations_inter_tile_dim_5;
Var var_output_activations_inter_tile_dim_6;
Var var_output_activations_inter_tile_dim_7;

Var var_output_activations_intra_tile_dim_0;
Var var_output_activations_intra_tile_dim_1;
Var var_output_activations_intra_tile_dim_2;
Var var_output_activations_intra_tile_dim_3;
Var var_output_activations_intra_tile_dim_4;
Var var_output_activations_intra_tile_dim_5;
Var var_output_activations_intra_tile_dim_6;
Var var_output_activations_intra_tile_dim_7;

int output_activations_dim_0_size; //
int output_activations_dim_1_size; //
int output_activations_dim_2_size; //
int output_activations_dim_3_size; //
int output_activations_dim_4_size; //
int output_activations_dim_5_size; //
int output_activations_dim_6_size; //
int output_activations_dim_7_size; //

int output_activations_dim_tile_0_size; //
int output_activations_dim_tile_1_size; //
int output_activations_dim_tile_2_size; //
int output_activations_dim_tile_3_size; //
int output_activations_dim_tile_4_size; //
int output_activations_dim_tile_5_size; //
int output_activations_dim_tile_6_size; //
int output_activations_dim_tile_7_size; //

Var GetVariable(char dim) {
  if(dim == '0')
    return var_output_activations_dim_0;
  else if(dim == '1')
    return var_output_activations_dim_1;
  else if(dim == '2')
    return var_output_activations_dim_2;
  else if(dim == '3')
    return var_output_activations_dim_3;
  else if(dim == '4')
    return var_output_activations_dim_4;
  else if(dim == '5')
    return var_output_activations_dim_5;
  else if(dim == '6')
    return var_output_activations_dim_6;
  else if(dim == '7')
    return var_output_activations_dim_7;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

Var GetInterTileVariable(int dim) {
  if(dim == 0)
    return var_output_activations_inter_tile_dim_0;
  else if(dim == 1)
    return var_output_activations_inter_tile_dim_1;
  else if(dim == 2)
    return var_output_activations_inter_tile_dim_2;
  else if(dim == 3)
    return var_output_activations_inter_tile_dim_3;
  else if(dim == 4)
    return var_output_activations_inter_tile_dim_4;
  else if(dim == 5)
    return var_output_activations_inter_tile_dim_5;
  else if(dim == 6)
    return var_output_activations_inter_tile_dim_6;
  else if(dim == 7)
    return var_output_activations_inter_tile_dim_7;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

Var GetIntraTileVariable(int dim) {
  if(dim == 0)
    return var_output_activations_intra_tile_dim_0;
  else if(dim == 1)
    return var_output_activations_intra_tile_dim_1;
  else if(dim == 2)
    return var_output_activations_intra_tile_dim_2;
  else if(dim == 3)
    return var_output_activations_intra_tile_dim_3;
  else if(dim == 4)
    return var_output_activations_intra_tile_dim_4;
  else if(dim == 5)
    return var_output_activations_intra_tile_dim_5;
  else if(dim == 6)
    return var_output_activations_intra_tile_dim_6;
  else if(dim == 7)
    return var_output_activations_intra_tile_dim_7;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

Var GetInterTileVariable(char dim) {
  if(dim == '0')
    return var_output_activations_inter_tile_dim_0;
  else if(dim == '1')
    return var_output_activations_inter_tile_dim_1;
  else if(dim == '2')
    return var_output_activations_inter_tile_dim_2;
  else if(dim == '3')
    return var_output_activations_inter_tile_dim_3;
  else if(dim == '4')
    return var_output_activations_inter_tile_dim_4;
  else if(dim == '5')
    return var_output_activations_inter_tile_dim_5;
  else if(dim == '6')
    return var_output_activations_inter_tile_dim_6;
  else if(dim == '7')
    return var_output_activations_inter_tile_dim_7;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

Var GetIntraTileVariable(char dim) {
  if(dim == '0')
    return var_output_activations_intra_tile_dim_0;
  else if(dim == '1')
    return var_output_activations_intra_tile_dim_1;
  else if(dim == '2')
    return var_output_activations_intra_tile_dim_2;
  else if(dim == '3')
    return var_output_activations_intra_tile_dim_3;
  else if(dim == '4')
    return var_output_activations_intra_tile_dim_4;
  else if(dim == '5')
    return var_output_activations_intra_tile_dim_5;
  else if(dim == '6')
    return var_output_activations_intra_tile_dim_6;
  else if(dim == '7')
    return var_output_activations_intra_tile_dim_7;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

Var GetVariable(int dim) {
  if(dim == 0)
    return var_output_activations_dim_0;
  else if(dim == 1)
    return var_output_activations_dim_1;
  else if(dim == 2)
    return var_output_activations_dim_2;
  else if(dim == 3)
    return var_output_activations_dim_3;
  else if(dim == 4)
    return var_output_activations_dim_4;
  else if(dim == 5)
    return var_output_activations_dim_5;
  else if(dim == 6)
    return var_output_activations_dim_6;
  else if(dim == 7)
    return var_output_activations_dim_7;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

int GetVariableSize(int dim) {
  if(dim == 0)
    return output_activations_dim_0_size;
  else if(dim == 1)
    return output_activations_dim_1_size;
  else if(dim == 2)
    return output_activations_dim_2_size;
  else if(dim == 3)
    return output_activations_dim_3_size;
  else if(dim == 4)
    return output_activations_dim_4_size;
  else if(dim == 5)
    return output_activations_dim_5_size;
  else if(dim == 6)
    return output_activations_dim_6_size;
  else if(dim == 7)
    return output_activations_dim_7_size;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

int GetVariableTileSize(int dim) {
  if(dim == 0)
    return output_activations_dim_tile_0_size;
  else if(dim == 1)
    return output_activations_dim_tile_1_size;
  else if(dim == 2)
    return output_activations_dim_tile_2_size;
  else if(dim == 3)
    return output_activations_dim_tile_3_size;
  else if(dim == 4)
    return output_activations_dim_tile_4_size;
  else if(dim == 5)
    return output_activations_dim_tile_5_size;
  else if(dim == 6)
    return output_activations_dim_tile_6_size;
  else if(dim == 7)
    return output_activations_dim_tile_7_size;
  else {
    cout<<"Not able to find Halide variable for the letter" << dim << endl;
    assert(false);
  }
}

int ctoi(char dim) {
  return dim - '0';
}


int main(int argc, char **argv) {

  assert(argc == 37);

  int input_activations_dim_0_size = atoi(argv[1]);
  int input_activations_dim_1_size = atoi(argv[2]);
  int input_activations_dim_2_size = atoi(argv[3]);
  int input_activations_dim_3_size = atoi(argv[4]);
  int input_activations_dim_4_size = atoi(argv[5]);
  int input_activations_dim_5_size = atoi(argv[6]);
  int input_activations_dim_6_size = atoi(argv[7]);
  int input_activations_dim_7_size = atoi(argv[8]);

  int filter_dim_0_size = atoi(argv[9]);
  int filter_dim_1_size = atoi(argv[10]);
  int filter_dim_2_size = atoi(argv[11]);
  int filter_dim_3_size = atoi(argv[12]);
  int filter_dim_4_size = atoi(argv[13]);
  int filter_dim_5_size = atoi(argv[14]);
  int filter_dim_6_size = atoi(argv[15]);
  int filter_dim_7_size = atoi(argv[16]);

  output_activations_dim_0_size = atoi(argv[17]); //
  output_activations_dim_1_size = atoi(argv[18]); //
  output_activations_dim_2_size = atoi(argv[19]); //
  output_activations_dim_3_size = atoi(argv[20]); //
  output_activations_dim_4_size = atoi(argv[21]); //
  output_activations_dim_5_size = atoi(argv[22]); //
  output_activations_dim_6_size = atoi(argv[23]); //
  output_activations_dim_7_size = atoi(argv[24]); //

  char *input_to_filter_mapping = argv[25];
  char *filter_unroll_dims = argv[26];
  char *inter_tile_loop_order = argv[27];
  char *intra_tile_loop_order = argv[28];

  output_activations_dim_tile_0_size = atoi(argv[29]);
  output_activations_dim_tile_1_size = atoi(argv[30]);
  output_activations_dim_tile_2_size = atoi(argv[31]);
  output_activations_dim_tile_3_size = atoi(argv[32]);
  output_activations_dim_tile_4_size = atoi(argv[33]);
  output_activations_dim_tile_5_size = atoi(argv[34]);
  output_activations_dim_tile_6_size = atoi(argv[35]);
  output_activations_dim_tile_7_size = atoi(argv[36]);
          
  Buffer<TYPE>  input(input_activations_dim_0_size,
                         input_activations_dim_1_size,
                         input_activations_dim_2_size,
                         input_activations_dim_3_size,
                         input_activations_dim_4_size,
                         input_activations_dim_5_size,
                         input_activations_dim_6_size,
                         input_activations_dim_7_size);
  
  Buffer<TYPE>  filter(filter_dim_0_size,
                          filter_dim_1_size,
                          filter_dim_2_size,
                          filter_dim_3_size,
                          filter_dim_4_size,
                          filter_dim_5_size,
                          filter_dim_6_size,
                          filter_dim_7_size);

  RDom r(filter.dim(0).min(), filter.dim(0).extent(),
                 filter.dim(1).min(), filter.dim(1).extent(),
                 filter.dim(2).min(), filter.dim(2).extent(),
                 filter.dim(3).min(), filter.dim(3).extent(),
                 filter.dim(4).min(), filter.dim(4).extent(),
                 filter.dim(5).min(), filter.dim(5).extent(),
                 filter.dim(6).min(), filter.dim(6).extent(),
                 filter.dim(7).min(), filter.dim(7).extent());

  cout << ctoi(input_to_filter_mapping[0]) << "\n";

  Func f_conv_naive("conv_naive");
  f_conv_naive(var_output_activations_dim_0,
               var_output_activations_dim_1,
               var_output_activations_dim_2,
               var_output_activations_dim_3,
               var_output_activations_dim_4,
               var_output_activations_dim_5,
               var_output_activations_dim_6,
               var_output_activations_dim_7)
          +=  filter(r.operator[](0),
                     r.operator[](1),
                     r.operator[](2),
                     r.operator[](3),
                     r.operator[](4),
                     r.operator[](5),
                     r.operator[](6),
                     r.operator[](7))
          * input(var_output_activations_dim_0 + r.operator[](ctoi(input_to_filter_mapping[0])),
                  var_output_activations_dim_1 + r.operator[](ctoi(input_to_filter_mapping[1])),
                  var_output_activations_dim_2 + r.operator[](ctoi(input_to_filter_mapping[2])),
                  var_output_activations_dim_3 + r.operator[](ctoi(input_to_filter_mapping[3])),
                  var_output_activations_dim_4 + r.operator[](ctoi(input_to_filter_mapping[4])),
                  var_output_activations_dim_5 + r.operator[](ctoi(input_to_filter_mapping[5])),
                  var_output_activations_dim_6 + r.operator[](ctoi(input_to_filter_mapping[6])),
                  var_output_activations_dim_7 + r.operator[](ctoi(input_to_filter_mapping[7])));

  Target target = Target(Target::OS::Linux, Target::Arch::AI, 32,
          {Target::NoAsserts, Target::NoBoundsQuery});
  f_conv_naive.compile_to_c("xhalide_generated_naive.cc",
                            std::vector<Argument>(), "conv_naive", target);


  Func f_conv_opt("conv_opt");
  f_conv_opt(var_output_activations_dim_0,
             var_output_activations_dim_1,
             var_output_activations_dim_2,
             var_output_activations_dim_3,
             var_output_activations_dim_4,
             var_output_activations_dim_5,
             var_output_activations_dim_6,
             var_output_activations_dim_7)
          +=  filter(r.operator[](0),
                     r.operator[](1),
                     r.operator[](2),
                     r.operator[](3),
                     r.operator[](4),
                     r.operator[](5),
                     r.operator[](6),
                     r.operator[](7))
          * input(var_output_activations_dim_0 + r.operator[](ctoi(input_to_filter_mapping[0])),
                  var_output_activations_dim_1 + r.operator[](ctoi(input_to_filter_mapping[1])),
                  var_output_activations_dim_2 + r.operator[](ctoi(input_to_filter_mapping[2])),
                  var_output_activations_dim_3 + r.operator[](ctoi(input_to_filter_mapping[3])),
                  var_output_activations_dim_4 + r.operator[](ctoi(input_to_filter_mapping[4])),
                  var_output_activations_dim_5 + r.operator[](ctoi(input_to_filter_mapping[5])),
                  var_output_activations_dim_6 + r.operator[](ctoi(input_to_filter_mapping[6])),
                  var_output_activations_dim_7 + r.operator[](ctoi(input_to_filter_mapping[7])));

  for(int i = 0; i < 8; i++) {
    f_conv_opt.bound(GetVariable(i), 0, GetVariableSize(i));
  }


  f_conv_opt.update().vectorize(var_output_activations_dim_0, VLEN);


  for(int i = 0; i < strlen(filter_unroll_dims); i++) {
    int dim = ctoi(filter_unroll_dims[i]);
    f_conv_opt.update().unroll(r.operator[](dim),
                               filter.dim(dim).extent());
  }

  for(int i = 0; i < 8; i++) {
    f_conv_opt.update().split(GetVariable(i), 
                              GetInterTileVariable(i),
                              GetIntraTileVariable(i),
                              GetVariableTileSize(i));
  }


  f_conv_opt.update()
          .reorder(GetIntraTileVariable(intra_tile_loop_order[0]),
                   GetIntraTileVariable(intra_tile_loop_order[1]),
                   GetIntraTileVariable(intra_tile_loop_order[2]),
                   GetIntraTileVariable(intra_tile_loop_order[3]),
                   GetIntraTileVariable(intra_tile_loop_order[4]),
                   GetIntraTileVariable(intra_tile_loop_order[5]),
                   GetIntraTileVariable(intra_tile_loop_order[6]),
                   GetIntraTileVariable(intra_tile_loop_order[7]),
                   GetInterTileVariable(inter_tile_loop_order[0]),
                   GetInterTileVariable(inter_tile_loop_order[1]),
                   GetInterTileVariable(inter_tile_loop_order[2]),
                   GetInterTileVariable(inter_tile_loop_order[3]),
                   GetInterTileVariable(inter_tile_loop_order[4]),
                   GetInterTileVariable(inter_tile_loop_order[5]),
                   GetInterTileVariable(inter_tile_loop_order[6]),
                   GetInterTileVariable(inter_tile_loop_order[7]));

  for(int i = 0; i < 8; i++) {
    f_conv_opt.update().unroll(GetIntraTileVariable(intra_tile_loop_order[i]));
  }

  f_conv_opt.update().prepare_for_software_pipelining(
          GetInterTileVariable(inter_tile_loop_order[0]));

  target = Target(Target::OS::Linux, Target::Arch::AI, 32,
                         {Target::NoAsserts, Target::NoBoundsQuery});
  f_conv_opt.compile_to_c("xhalide_generated_opt.cc",
                            std::vector<Argument>(), "conv_opt", target);

/*
  //f_conv_opt(y, x, z, n) += filter(r.y, r.x, r.z, z) * input(y + r.y, x + r.x, r.z, n);
  //f_conv_opt(x, y, z, n) += filter(r.z, r.x, r.y, z) * input((x + r.x)%2, (x+r.x)/2, y + r.y, r.z, n);
  f_conv_opt(x, y, z, n) +=
          filter(r.x, r.y, r.z, z)
          * input(x + r.x,
                  y+r.y,
                  r.z, n);

  f_conv_opt.bound(get_variable(inter_tile_loop_order[0]), 0, get_variable_range(inter_tile_loop_order[0]));
  f_conv_opt.bound(get_variable(inter_tile_loop_order[1]), 0, get_variable_range(inter_tile_loop_order[1]));
  f_conv_opt.bound(get_variable(inter_tile_loop_order[2]), 0, get_variable_range(inter_tile_loop_order[2]));
  f_conv_opt.bound(get_variable(inter_tile_loop_order[3]), 0, get_variable_range(inter_tile_loop_order[3]));
  f_conv_opt.update()
          .vectorize(x, vlen)
          .unroll(r.x,filter.dim(0).extent())
          .unroll(r.y, filter.dim(1).extent())
          .unroll(r.z, filter.dim(2).extent())
          .tile(x, y, x_i, y_i, unroll_jam_size_x, unroll_jam_size_y)
          .unroll(x_i)
          .unroll(y_i)
          .prepare_for_software_pipelining(get_variable(inter_tile_loop_order[0]))
          .reorder(get_variable(inter_tile_loop_order[0]),
                   get_variable(inter_tile_loop_order[1]),
                   get_variable(inter_tile_loop_order[2]),
                   get_variable(inter_tile_loop_order[3]));


  f_conv_opt.compile_to_c("xhalide_generated_opt.cc", std::vector<Argument>(), "conv_opt", target);
  //f_conv.compile_to_c("convolution.c", {input, filter}, "conv", target);
*/

  printf("Success!\n");
  return 0;
}
