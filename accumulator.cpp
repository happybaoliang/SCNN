#include"accumulator.hpp"


void accumulator::reset(){
	cxb.reset();
}


bool accumulator::queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]){
	return cxb.queueing(products,input_halos);
}


product_type accumulator::get_and_clear(ocoord_type ochannel, row_coord_type row_coord, col_coord_type col_coord){
	product_type product = cxb.acc[ochannel][row_coord].get_and_clear(col_coord);
	//cout<<"accumulator["<<ochannel<<"]["<<row_coord<<"]["<<col_coord<<"]="<<product<<endl;
	return product;
}
