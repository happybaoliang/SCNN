#ifndef ACCUMULATOR_BANK_HPP__
#define ACCUMULATOR_BANK_HPP__

#include"common.hpp"


struct accumulator_bank{
	void reset();
#ifndef INPUT_HALOS
	void clear(output_channel_t ocoord);
	product_t get(output_channel_t ocoord);
#endif
	product_t acc[MAX_OUTPUT_CHANNEL_GROUP_SIZE];
	product_t get_and_clear(output_channel_t ocoord);
	void adder(output_channel_t ocoord, product_t product);
};


#endif
