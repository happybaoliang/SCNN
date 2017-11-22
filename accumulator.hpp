#ifndef ACCUMULATOR_HPP__
#define ACCUMULATOR_HPP__


#include<hls_stream.h>

#include"crossbar.hpp"


struct accumulator{
	void reset();
	struct crossbar cxb;
	bool queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]);
	product_type get_and_clear(ocoord_type ochannel, row_coord_type row_coord, col_coord_type col_coord);
};


#endif
