#ifndef CROSSBAR_HPP__
#define CROSSBAR_HPP__

#include<iostream>
#include<hls_stream.h>

#include"cpu_top.hpp"
#include"accumulator_bank.hpp"
#include"flit.hpp"
#include"arbiter.hpp"


struct crossbar{
	void reset();
	struct arbiter arb[NUM_OF_RESOURCES];
	struct accumulator_bank acc[OUTPUT_CHANNEL_CHUNK_SIZE][FEATURES_ROW_PER_CHUNK];
	bool queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]);
};


#endif
