############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
############################################################
open_project SCNN
add_files SCNN/accumulator.cpp
add_files SCNN/accumulator.hpp
add_files SCNN/accumulator_bank.cpp
add_files SCNN/accumulator_bank.hpp
add_files SCNN/arbiter.cpp
add_files SCNN/arbiter.hpp
add_files SCNN/crossbar.cpp
add_files SCNN/crossbar.hpp
add_files SCNN/flit.hpp
add_files SCNN/fpga_top.cpp
add_files SCNN/fpga_top.hpp
add_files SCNN/process_element.cpp
add_files SCNN/process_element.hpp
add_files -tb SCNN/cpu_top.cpp
add_files -tb SCNN/cpu_top.hpp
add_files -tb SCNN/feature.bin
add_files -tb SCNN/weights.bin
open_solution "solution1"
set_part {xqku115-rlf1924-2-i} -tool vivado
create_clock -period 10 -name default
#source "./SCNN/solution1/directives.tcl"
csim_design -clean -compiler gcc
csynth_design
cosim_design
export_design -format ip_catalog
