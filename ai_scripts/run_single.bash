#!/bin/bash
# Copyright (c) 2019 Xilinx Inc. All Rights Reserved.

source ~/.bash_profile

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
if [[ $(grep SUCCESS $regression_directory/StandaloneISS/stdout.log) ]]; then
    echo "Functionally correct! :-)";
else
    echo "Functionally incorrect! :-(";
fi

#Get clock cycles
conv_halide_cycles=`grep -E "_Z4conv" $regression_directory/StandaloneISS/profile.prf  | grep % | awk '{print $3}'`
conv_original_cycles=`grep -E "naive" $regression_directory/StandaloneISS/profile.prf  | grep % | awk '{print $3}'`

echo "Original: $conv_original_cycles, Halide: $conv_halide_cycles";

#Cleaning compilation files
#rm -rf work

#Cleaning regression results
#rm -rf regression_results
