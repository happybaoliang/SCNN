#ifndef PROCESS_ELEMENT_HPP__
#define PROCESS_ELEMENT_HPP__

#include"cpu_top.hpp"

#include"accumulator_bank.hpp"


struct ProcessElement{
	bool stall;
	pe_coord_type col;
	pe_coord_type row;
	weight_type weight[F];
	size_type total_weights;
	ocoord_type GetOCoord();
	size_type total_features;
	void AccumulateProduct();
	zeros_type weightindex[F];
	void FetchNextIFeatureMap();
	feature_type feature_buf[I];
	col_coord_type GetColCoord();
	row_coord_type GetRowCoord();
	offset_type GetOffsetInMatrix();
	zeros_type feature_index_buf[I];
	channel_type current_input_channel;
	size_type num_of_processed_weights;
	size_type num_of_weights_per_kernel;
	size_type num_of_processed_features;
	void SetNextInputChannel(int channel);
	size_type num_of_none_zero_feature_fetched;
	size_type num_of_none_zero_features[INPUT_CHANNEL_NUM];
	struct accumulator_bank accumulator[OUTPUT_CHANNEL_CHUNK_SIZE];
	feature_type featuremap[INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
	zeros_type featureindex[INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
	ProcessElement():stall(false),total_weights(0),total_features(0),num_of_none_zero_feature_fetched(0){
		col=0;
		row=0;
		current_input_channel=0;
		num_of_processed_weights=0;
		num_of_processed_features = 0;
		num_of_weights_per_kernel = KERNEL_SIZE*KERNEL_SIZE;
	}
	void ResetProcessElement(pe_coord_type row_id, pe_coord_type col_id){
		col = col_id;
		row = row_id;
		stall = false;
		total_weights = 0;
		total_features = 0;
		current_input_channel=0;
		num_of_processed_weights=0;
		num_of_processed_features = 0;
		num_of_none_zero_feature_fetched = 0;
		num_of_weights_per_kernel = KERNEL_SIZE*KERNEL_SIZE;
	}
};

#endif
