#include"accumulator_bank.hpp"


void accumulator_bank::adder(col_coord_type xcoord, row_coord_type ycoord, product_type product){
	acc[xcoord][ycoord]+= product;
}


product_type accumulator_bank::get_and_clear(col_coord_type xcoord, row_coord_type ycoord){
	product_type product = acc[xcoord][ycoord];
	acc[xcoord][ycoord] = 0;
	return product;
}
