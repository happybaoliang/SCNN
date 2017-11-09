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
	int col;
	int row;
	int total_weights;
	int total_features;
	weight_type weight[F];
	xcoord_type GetXCoord();
	ycoord_type GetYCoord();
	ocoord_type GetOCoord();
	int GetOffsetInMatrix();
	void AccumulateProduct();
	zeros_type weightindex[F];
	int current_input_channel;
	void FetchNextIFeatureMap();
	feature_type feature_buf[I];
	int num_of_weights_per_kernel;
	zeros_type feature_index_buf[I];
	int num_of_none_zero_feature_fetched;
	int num_of_none_zero_features[INPUT_CHANNEL_NUM];
	feature_type featuremap[INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
	zeros_type featureindex[INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
	feature_type accumulator[OUTPUT_CHANNEL_NUM][FEATURES_ROW_PER_CHUNK][FEATURES_COL_PER_CHUNK];
	ProcessElement():col(0),row(0),total_weights(0),total_features(0),num_of_none_zero_feature_fetched(0){
		num_of_weights_per_kernel = KERNEL_SIZE*KERNEL_SIZE;
	}
	void ResetProcessElement(int row_id, int col_id){
		col = col_id;
		row = row_id;
		total_weights = 0;
		total_features = 0;
		num_of_none_zero_feature_fetched = 0;
		num_of_weights_per_kernel = KERNEL_SIZE*KERNEL_SIZE;
	}
};

#endif
