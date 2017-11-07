#ifndef PROCESS_ELEMENT_HPP__
#define PROCESS_ELEMENT_HPP__

#include"cpu_top.hpp"



#if 1
typedef int xcoord_type;
typedef int ycoord_type;
typedef int ocoord_type;
#else

#endif

struct ProcessElement{
	int kernel_size;
	int total_weights;
	int total_features;
	weight_type weight[F];
	int total_input_channel;
	xcoord_type GetXCoord();
	ycoord_type GetYCoord();
	ocoord_type GetOCoord();
	void AccumulateProduct();
	zeros_type weightindex[F];
	unsigned char GetMatrixId();
	int num_of_none_zero_features;
	feature_type featuremap[MAX_NUM_OF_FEATURE_PER_CHUNK];
	zeros_type featureindex[MAX_NUM_OF_FEATURE_PER_CHUNK];
	feature_type output_feature[OUTPUT_CHANNEL_NUM][FEATURES_ROW_PER_CHUNK][FEATURES_COL_PER_CHUNK];
	ProcessElement():total_weights(0),total_features(0){
		kernel_size = 0;
		total_features = 0;
		total_input_channel = 0;
		num_of_none_zero_features = 0;
	}
};

#endif
