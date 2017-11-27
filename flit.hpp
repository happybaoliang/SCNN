#ifndef FLIT_HPP__
#define FLIT_HPP__

#include"common.hpp"


struct Flit{
	dimension_t row;
	dimension_t col;
	output_channel_t ochannel;
	product_t product;
	Flit(){
		row = 0;
		col = 0;
		product = 0;
		ochannel = 0;
	}
	Flit(output_channel_t co,
		dimension_t r,
		dimension_t c,
		product_t prod){
		row = r;
		col = c;
		ochannel = co;
		product = prod;
	}
};


#endif
