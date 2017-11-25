#ifndef ACCUMULATOR_BANK_HPP__
#define ACCUMULATOR_BANK_HPP__

#include<iostream>
#include"cpu_top.hpp"

using namespace std;


struct accumulator_bank{
	void reset();
	product_t acc[MAX_FEATURES_COL_PER_CHUNK];
	product_t get_and_clear(dimension_t col_coord);
	void adder(dimension_t col_coord, product_t product);
};


#endif
