#ifndef LAYER_HPP__
#define LAYER_HPP__

#include"common.hpp"


struct pe_config{
	bool pad;
	stride_t stride;
	kernel_t kernel_size;
	dimension_t vertical_input_feature_chunk_num;
	dimension_t horizontal_input_feature_chunk_num;
};


struct fpga_config{
	bool relu;
	struct pe_config config;
	input_channel_t input_channels;

	weight_t*** compressed_weights;
	zero_t*** compressed_weight_index;
	weight_index_t** num_of_none_zero_weights;

	dimension_t num_of_kernels_per_group;

	feature_t*** compressed_output_features;
	zero_t*** compressed_output_feature_index;
	feature_index_t** num_of_none_zero_output_features;

	feature_t*** compressed_input_features;
	zero_t*** compressed_input_feature_index;
	feature_index_t** num_of_none_zero_input_features;
	feature_index_t* max_num_of_none_zero_input_features;
	output_channel_t num_of_output_channel_groups;
};


struct layer_t {
	weight_t**** weights;

	dimension_t input_width;
	dimension_t input_height;

	dimension_t output_width;
	dimension_t output_height;

	struct fpga_config config;

	output_channel_t output_channels;

	feature_t*** input_features;
	feature_t*** output_features;

	dimension_t vertical_output_feature_chunk_num;
	dimension_t horizontal_output_feature_chunk_num;

	layer_t();
	layer_t(const layer_t& layer);
	layer_t(dimension_t w, dimension_t h,
			input_channel_t ci, output_channel_t co, kernel_t k,
			bool p, stride_t s, bool r, output_channel_t nk);

	void PrintLayer();

	void CompressWeights();
	void GenerateRandomWeight();
	bool AllocateMemoryForWeight();
	weight_t GetRandomWeight(int max_val);
	bool AllocateMemoryForCompressedWeight();
	void DumpGeneratedWeight(const char* filename);
	void LoadGeneratedWeight(const char* filename);

	void CompressInputFeatureMap();
	void GenerateRandomFeatureMap();
	void DeCompressOutputFeatureMap();
	bool AllocateMemoryForInputFeature();
	bool AllocateMemoryForOutputFeature();
	feature_t GetRandomFeature(int max_val);
	int CheckDeCompressedConvolutionResults();
	bool AllocateMemoryForCompressedInputFeature();
	bool AllocateMemoryForCompressedOutputFeature();
	void LoadGeneratedFeatureMap(const char* filename);
	void DumpGeneratedFeatureMap(const char* filename);
};


#endif
