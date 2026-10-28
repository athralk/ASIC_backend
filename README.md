# 1. Verilog to gate level
This is a very light weight compilter for understanding how Verilog actually translates to gates and that too NAND and INVERT - the universal gates.

Currently the repo is able to parse verilog (combinational mostly, sequential is under work) and develop optimised canonical forms of the universal gate representation for each root (forest of gate -> technology mapper).

![technology mapper](images\technology_mapper.jpg) <br>
<br>
![corresponding verilog](images\example_verilog.jpg)


## Usage
To run, use it in Linux:
1. make
2. ./output_test /example.v <br>


basically -> ./executable_name /verilog_file_name.v 
