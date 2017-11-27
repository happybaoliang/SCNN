#ifndef CROSSBAR_HPP__
#define CROSSBAR_HPP__

#include<hls_stream.h>
#include"accumulator_bank.hpp"
#include"flit.hpp"
#include"arbiter.hpp"


struct crossbar{
	void reset();
	Flit flit[NUM_OF_REQUESTS];
	valid_type valid[NUM_OF_REQUESTS];
	struct arbiter arb[NUM_OF_RESOURCES];
	struct accumulator_bank acc[MAX_OUTPUT_CHANNEL_GROUP_SIZE][MAX_FEATURES_ROW_PER_CHUNK];
	bool queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]);
};


#endif
