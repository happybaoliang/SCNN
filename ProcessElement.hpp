#ifndef PROCESS_ELEMENT_HPP__
#define PROCESS_ELEMENT_HPP__

#include"cpu_top.hpp"


#define NUM_OF_PEs		64


#if 1
typedef int xcoord_type;
typedef int ycoord_type;
typedef int ocoord_type;
#else

#endif

struct ProcessElement{
	weight_type weight[F];
	zeros_type weightindex[F];
	int num_of_none_zero_features;
	feature_type featuremap[MAX_NUM_OF_FEATURE_PER_CHUNK];
	zeros_type featureindex[MAX_NUM_OF_FEATURE_PER_CHUNK];
	void AccumulateProduct();
};

#endif
