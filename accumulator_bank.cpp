#include"accumulator_bank.hpp"


void accumulator_bank::reset(){
	for (int i=0;i<FEATURES_COL_PER_CHUNK;i++){
		acc[i] = 0;
	}
}


void accumulator_bank::adder(col_coord_type col_coord, product_type product){
	acc[col_coord]+= product;
	//cout<<":"<<product<<endl;
}


product_type accumulator_bank::get_and_clear(col_coord_type col_coord){
	product_type product = acc[col_coord];
	acc[col_coord] = 0;
	return product;
}
