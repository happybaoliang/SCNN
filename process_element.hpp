#ifndef PROCESS_ELEMENT_HPP__
#define PROCESS_ELEMENT_HPP__

#include"accumulator.hpp"
#include"layer.hpp"


struct ProcessElement{
	bool stall;
	dimension_t col;
	dimension_t row;
	weight_t weight[F];
	kernel_t kernel_size;
	zero_t weightindex[F];
	weight_index_t total_weights;
	feature_index_t total_features;
	struct accumulator acc;
	feature_t feature_buf[I];
	bool input_queue_not_empty;
	zero_t feature_index_buf[I];
	hls::stream<Flit> flits[F][I];
	weight_index_t num_of_processed_weights;
	weight_index_t num_of_weights_per_kernel;
	feature_index_t num_of_processed_features;
	input_channel_t current_input_channel;
	feature_index_t num_of_none_zero_feature_fetched;
	hls::stream<Flit>* input_halos[NUM_OF_PORTS];
	hls::stream<Flit>* output_halos[NUM_OF_PORTS];
	dimension_t vertical_input_feature_chunk_num;
	dimension_t horizontal_input_feature_chunk_num;
	feature_index_t num_of_none_zero_features[MAX_INPUT_CHANNEL_NUM];
	feature_t featuremap[MAX_INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
	zero_t featureindex[MAX_INPUT_CHANNEL_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];

	void DrainOutProducts();
	void AccumulateProduct();
	dimension_t GetColCoord();
	dimension_t GetRowCoord();
	void FetchNextIFeatureMap();
	output_channel_t GetOCoord();
	output_channel_t GetOffsetInMatrix();
	void SetNextInputChannel(input_channel_t channel);

	void ResetProcessElement(dimension_t row_id, dimension_t col_id, struct pe_config& config){
		acc.reset();
		col = col_id;
		row = row_id;
		stall = false;
		total_weights = 0;
		total_features = 0;
		current_input_channel=0;
		num_of_processed_weights=0;
		input_queue_not_empty = false;
		num_of_processed_features = 0;
		kernel_size = config.kernel_size;
		num_of_none_zero_feature_fetched = 0;
		num_of_weights_per_kernel = kernel_size*kernel_size;
		vertical_input_feature_chunk_num = config.vertical_input_feature_chunk_num;
		horizontal_input_feature_chunk_num = config.horizontal_input_feature_chunk_num;
	}

	ProcessElement():stall(false),total_weights(0),total_features(0),num_of_none_zero_feature_fetched(0){
		col=0;
		row=0;
		kernel_size = 0;
		current_input_channel=0;
		num_of_processed_weights=0;
		num_of_processed_features = 0;
		input_queue_not_empty = false;
		vertical_input_feature_chunk_num = 0;
		horizontal_input_feature_chunk_num = 0;
		num_of_weights_per_kernel = kernel_size*kernel_size;
	}
};


#endif
