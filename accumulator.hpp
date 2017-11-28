#ifndef ACCUMULATOR_HPP__
#define ACCUMULATOR_HPP__

#include<hls_stream.h>
#include"crossbar.hpp"


struct accumulator{
	void reset();
	struct crossbar cxb;
#ifdef INPUT_HALOS
	bool queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]);
#else
	bool queueing(hls::stream<Flit> (&products)[F][I]);
	void clear(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord);
	product_t get(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord);
#endif
	product_t get_and_clear(output_channel_t ochannel, dimension_t row_coord, dimension_t col_coord);
};


#endif
