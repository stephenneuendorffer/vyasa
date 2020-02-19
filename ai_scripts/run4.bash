#!/bin/bash
# Copyright (c) 2019 Xilinx Inc. All Rights Reserved.

source ~/.bash_profile

buffer_size=1024 #4KB after double buffering -- 1024 int32 elements
#Filters: 3x3
filter_width=3
filter_height=3

create_data_file() {

		#Removing previous data file if exists
	if [[ $(ls ./data/kernelAndInImage_256x16_k3_gaussblur.txt) ]]; then
		rm ./data/kernelAndInImage_256x16_k3_gaussblur.txt
	fi

	cp ./data/data.txt ./data/kernelAndInImage_256x16_k3_gaussblur.txt
	params="$5\n$6\n$7\n$8\n$1\n$9\n$2\n${10}\n$3\n$4";

	for (( filters=1; filters<=$8; filters++ ))
	do
		for (( channels=1; channels<=$7; channels++ ))
		do
			for (( height=1; height<=$6; height++ ))
			do
				for (( width=1; width<=$5; width++ ))
				do
					params+="\n";
					params+=`echo "$width*64" | bc`;
				done
			done
		done
	done

	sed -i "1i$params" ./data/kernelAndInImage_256x16_k3_gaussblur.txt
}

declare -a looporders=("xyzn" "xynz" "xzyn" "xzny" "xnzy" "xnyz" "yxzn"
					   "yxnz" "yzxn" "yznx" "ynzx" "ynxz" "zyxn" "zynx"
					   "zxyn" "zxny" "znxy" "znyx" "nyzx" "nyxz" "nzyx"
					   "nzxy" "nxzy" "nxyz")

run_halide() {
for i in "${looporders[@]}"
	do
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
		halidelog=`./conv $1 $2 $3 $4 $5 $6 $7 $8 $9 $10 $i`

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

		echo "Loop order: $i Original: $conv_original_cycles Halide: $conv_halide_cycles" >> results.txt;

		#Cleaning compilation files -- Reducing compilation time
		#rm -rf work

		#Cleaning regression results
		#rm -rf regression_results
	done
}




for (( output_images=1; output_images<=100; output_images++ ))
do
	for (( output_channels=1; output_channels<=100; output_channels++ ))
	do
		for (( output_height=1; output_height<=100; output_height++ ))
		do
			for (( output_width=8; output_width<=400; output_width+=8 ))
			do
				output_volume=`echo "$output_images * $output_channels * $output_height * $output_width" | bc`;
				for (( filter_channels=4; filter_channels<=100; filter_channels+=4 ))
				do
					filter_volume=`echo "$filter_width * $filter_height * $filter_channels *$output_channels" | bc`;

					#TODO: May not be same if filter_width > 8
					input_width=`echo "$output_width + 8" | bc`;
					input_height=`echo "$output_height + $filter_height - 1" | bc`;
					input_volume=`echo "$input_width * $input_height * $filter_channels * $output_images" | bc`;

					total_volume=`echo "$output_volume + $filter_volume + $input_volume" | bc`;
					if (( $total_volume < $buffer_size )); then
						echo "$input_width $input_height $filter_channels $output_images $filter_width $filter_height $filter_channels $output_channels $output_width $output_height $total_volume";
						create_data_file $input_width $input_height $filter_channels $output_images $filter_width $filter_height $filter_channels $output_channels $output_width $output_height;
					    run_halide $input_width $input_height $filter_channels $output_images $filter_width $filter_height $filter_channels $output_channels $output_width $output_height;
					    exit
				  	fi
				done
			done
		done
	done
done
