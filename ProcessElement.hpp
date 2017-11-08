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
	int total_weights;
	int total_features;
	weight_type weight[F];
	xcoord_type GetXCoord();
	ycoord_type GetYCoord();
	ocoord_type GetOCoord();
	void AccumulateProduct();
	zeros_type weightindex[F];
	int current_input_channel;
	int num_of_weights_per_kernel;
	unsigned char GetMatrixCount();
	unsigned char GetOffsetInMatrix();
	unsigned char GetMatrixInputChannel();
	unsigned char GetMatrixOutputChannel();
	unsigned char GetFeatureInputChannel();
	int num_of_none_zero_features[INPUT_CHANNEL_NUM];
	feature_type featuremap[INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
	zeros_type featureindex[INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
	feature_type accumulaor[OUTPUT_CHANNEL_NUM][FEATURES_ROW_PER_CHUNK][FEATURES_COL_PER_CHUNK];
	ProcessElement():total_weights(0),total_features(0){
		num_of_weights_per_kernel = KERNEL_SIZE*KERNEL_SIZE;
	}
	void ResetProcessElement(){
		total_weights = 0;
		total_features = 0;
		num_of_weights_per_kernel = KERNEL_SIZE*KERNEL_SIZE;
	}
};

#endif
