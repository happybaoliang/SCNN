#ifndef ACCUMULATOR_BANK_HPP__
#define ACCUMULATOR_BANK_HPP__

#include"cpu_top.hpp"


struct accumulator_bank{
	void adder(col_coord_type xcoord, row_coord_type ycoord, product_type product);
	product_type get_and_clear(col_coord_type xcoord, row_coord_type ycoord);
	product_type acc[FEATURES_ROW_PER_CHUNK][FEATURES_COL_PER_CHUNK];
};


#endif
