#include<iostream>
#include"accumulator.hpp"
using namespace std;


void accumulator::reset(){
	cxb.reset();
}


#ifndef INPUT_HALOS
product_t accumulator::get(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord){
	product_t product = cxb.acc[col_coord][row_coord].get(ochannel);
	//cout<<"accumulator["<<ochannel<<"]["<<row_coord<<"]["<<col_coord<<"]="<<product<<endl;
	return product;
}


void accumulator::clear(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord){
	cxb.acc[col_coord][row_coord].clear(ochannel);
	//cout<<"accumulator["<<ochannel<<"]["<<row_coord<<"]["<<col_coord<<"]="<<product<<endl;
}


bool accumulator::queueing(hls::stream<Flit> (&products)[F][I]){
	return cxb.queueing(products);
}
#else
product_t accumulator::get_and_clear(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord){
	product_t product = cxb.acc[col_coord][row_coord].get_and_clear(ochannel);
	//cout<<"accumulator["<<ochannel<<"]["<<row_coord<<"]["<<col_coord<<"]="<<product<<endl;
	return product;
}


bool accumulator::queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]){
	return cxb.queueing(products,input_halos);
}
#endif
