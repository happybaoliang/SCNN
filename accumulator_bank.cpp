#include<iostream>
#include"accumulator_bank.hpp"

using namespace std;


void accumulator_bank::reset(){
	for (int i=0;i<MAX_FEATURES_COL_PER_CHUNK;i++){
		acc[i] = 0;
	}
}


void accumulator_bank::adder(dimension_t col_coord, product_t product){
	//cout<<" "<<acc[col_coord];
	acc[col_coord] += product;
	//cout<<"+="<<product<<endl;
}


product_t accumulator_bank::get_and_clear(dimension_t col_coord){
	product_t product = acc[col_coord];
	acc[col_coord] = 0;
	return product;
}


#ifndef INPUT_HALOS
product_t accumulator_bank::get(dimension_t col_coord){
	product_t product = acc[col_coord];
	return product;
}


void accumulator_bank::clear(dimension_t col_coord){
	acc[col_coord] = 0;
}
#endif
