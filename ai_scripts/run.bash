#!/bin/bash
# Copyright (c) 2019 Xilinx Inc. All Rights Reserved.

source ~/.bash_profile

#Going to halide directory for compilation
cd ..

#Removing previous generated file if exists
if [[ $(ls xhalide_generated.cc) ]]; then
	rm xhalide_generated.cc
fi

#Compiling using XHalide compiler
g++ -D _GLIBCXX_USE_CXX11_ABI=0 -fno-rtti xhalide-convolution.cpp \
	-g -std=c++11 -I ../halide_build/include/ -I ./tools/ \
	-L ../halide_build/lib/ -lHalide -lpthread -ldl -o conv

#Generate xhalide_generated.cc
halidelog=`./conv 48 10 4 2 3 3 4 2 40 8 "nyxz"`

cp xhalide_generated.cc ai_scripts/

cd ai_scripts

project_name="conv";
project="$project_name.prx";
echo "Project name is : $project";


#Compiling the project
compilelog=`xchessmk $project`;

if [[ $(echo $compilelog | grep successfully) ]]; then
    echo "Compilation success! :-)";
else
    echo "Compilation failed! :-( ";
fi

#Running the project
#Make sure that data folder is in the same directory as execute.py
python execute.py;

#Check output
regression_directory="./regression_results/regression_${project_name}_sa_iss_results"
if [[ $(grep PASSED $regression_directory/StandaloneISS/stdout.log) ]]; then
    echo "Functionally correct! :-)";
else
    echo "Functionally incorrect! :-(";
fi

#Get clock cycles
conv_halide_cycles=`grep -E "_Z4conv" $regression_directory/StandaloneISS/profile.prf  | grep % | awk '{print $3}'`
conv_original_cycles=`grep -E "naive" $regression_directory/StandaloneISS/profile.prf  | grep % | awk '{print $3}'`

echo "Original: $conv_original_cycles, Halide: $conv_halide_cycles" >> results.txt;

#Cleaning compilation files -- Preserving to reduce compilation overhead
#rm -rf work

#Cleaning regression results
rm -rf regression_results
