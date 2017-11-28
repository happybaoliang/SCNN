#include<iostream>
#include"accumulator.hpp"
using namespace std;


void accumulator::reset(){
	cxb.reset();
}


product_t accumulator::get_and_clear(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord){
	product_t product = cxb.acc[ochannel][row_coord].get_and_clear(col_coord);
	//cout<<"accumulator["<<ochannel<<"]["<<row_coord<<"]["<<col_coord<<"]="<<product<<endl;
	return product;
}


#ifndef INPUT_HALOS
product_t accumulator::get(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord){
	product_t product = cxb.acc[ochannel][row_coord].get(col_coord);
	//cout<<"accumulator["<<ochannel<<"]["<<row_coord<<"]["<<col_coord<<"]="<<product<<endl;
	return product;
}


void accumulator::clear(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord){
	cxb.acc[ochannel][row_coord].clear(col_coord);
	//cout<<"accumulator["<<ochannel<<"]["<<row_coord<<"]["<<col_coord<<"]="<<product<<endl;
}


bool accumulator::queueing(hls::stream<Flit> (&products)[F][I]){
	return cxb.queueing(products);
}
#else
bool accumulator::queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]){
	return cxb.queueing(products,input_halos);
}
#endif
