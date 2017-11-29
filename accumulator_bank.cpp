#include<iostream>
#include"accumulator_bank.hpp"

using namespace std;


void accumulator_bank::reset(){
	for (output_channel_t i=0;i<MAX_OUTPUT_CHANNEL_GROUP_SIZE;i++){
		acc[i] = 0;
	}
}


//任意时刻只有一个输出通道是活跃的，因此可以只用一个加法器
void accumulator_bank::adder(output_channel_t ocoord, product_t product){
	//cout<<" "<<"acc["<<ocoord<<"]:"<<acc[ocoord];
	acc[ocoord] += product;
	//cout<<"+="<<product<<endl;
}


#ifndef INPUT_HALOS
product_t accumulator_bank::get(output_channel_t ocoord){
	product_t product = acc[ocoord];
	return product;
}


void accumulator_bank::clear(output_channel_t ocoord){
	acc[ocoord] = 0;
}
#else
product_t accumulator_bank::get_and_clear(output_channel_t ocoord){
	product_t product = acc[ocoord];
	acc[ocoord] = 0;
	return product;
}
#endif
