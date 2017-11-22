#ifndef ACCUMULATOR_BANK_HPP__
#define ACCUMULATOR_BANK_HPP__

#include<iostream>
#include"cpu_top.hpp"

using namespace std;


struct accumulator_bank{
	void reset();
	product_type acc[FEATURES_COL_PER_CHUNK];
	product_type get_and_clear(col_coord_type col_coord);
	void adder(col_coord_type col_coord, product_type product);
};


#endif
