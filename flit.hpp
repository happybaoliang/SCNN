#ifndef FLIT_HPP__
#define FLIT_HPP__

#include"cpu_top.hpp"


struct Flit{
	row_coord_type row;
	col_coord_type col;
	ocoord_type ochannel;
	product_type product;
	Flit(){
		row = 0;
		col = 0;
		product = 0;
		ochannel = 0;
	}
	Flit(ocoord_type co,
		row_coord_type r,
		col_coord_type c,
		product_type prod){
		row = r;
		col = c;
		ochannel = co;
		product = prod;
	}
};


#endif
