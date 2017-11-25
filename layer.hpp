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
	index_t** num_of_none_zero_weights;

	dimension_t num_of_kernels_per_group;

	feature_t*** compressed_output_features;
	zero_t*** compressed_output_feature_index;
	index_t** num_of_none_zero_output_features;

	feature_t*** compressed_input_features;
	zero_t*** compressed_input_feature_index;
	index_t** num_of_none_zero_input_features;
	index_t* max_num_of_none_zero_input_features;
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

	layer_t(){
		config.config.stride = 0;
		config.config.pad = true;
		config.relu = true;
		weights = NULL;
		config.config.kernel_size = 0;
		input_width = 0;
		input_height = 0;
		output_width = 0;
		output_height = 0;
		config.input_channels = 0;
		output_channels = 0;
		input_features = NULL;
		output_features = NULL;
		config.compressed_weights = NULL;
		config.num_of_kernels_per_group = 0;
		config.num_of_output_channel_groups = 0;
		config.compressed_weight_index = NULL;
		config.num_of_none_zero_weights = NULL;
		config.compressed_input_features = NULL;
		config.compressed_input_feature_index = NULL;
		config.num_of_none_zero_input_features = NULL;
		config.max_num_of_none_zero_input_features = NULL;
		config.compressed_output_features = NULL;
		config.compressed_output_feature_index = NULL;
		config.num_of_none_zero_output_features = NULL;
		config.config.vertical_input_feature_chunk_num = 0;
		config.config.horizontal_input_feature_chunk_num = 0;
		vertical_output_feature_chunk_num = 0;
		horizontal_output_feature_chunk_num = 0;
	}

	layer_t(dimension_t w, dimension_t h, input_channel_t ci, output_channel_t co,
			kernel_t k, bool p, stride_t s, bool r, output_channel_t nk){
		config.config.pad = p;
		config.relu = r;
		config.config.stride = s;
		weights = NULL;
		config.config.kernel_size = k;
		input_width = w;
		input_height = h;
		config.input_channels = ci;
		output_channels = co;
		input_features = NULL;
		output_features = NULL;
		config.compressed_weights = NULL;
		config.compressed_weight_index = NULL;
		config.num_of_none_zero_weights = NULL;
		config.compressed_input_features = NULL;
		config.compressed_output_features = NULL;
		config.compressed_input_feature_index = NULL;
		config.max_num_of_none_zero_input_features = NULL;
		config.num_of_none_zero_input_features = NULL;
		config.compressed_output_feature_index = NULL;
		config.num_of_none_zero_output_features = NULL;
		dimension_t padding = config.config.pad ? (config.config.kernel_size) : (kernel_t)0;
		output_width = (input_width + 2*padding - config.config.kernel_size + 1)/config.config.stride;
		output_height = (input_height + 2* padding - config.config.kernel_size + 1)/config.config.stride;
		assert(((MAX_OUTPUT_CHANNEL_GROUP_SIZE*config.config.kernel_size*config.config.kernel_size)%F)==0);
		config.num_of_output_channel_groups = ceil(1.0*output_channels/MAX_OUTPUT_CHANNEL_GROUP_SIZE);
		config.config.vertical_input_feature_chunk_num = ceil(1.0*input_height/MAX_FEATURES_ROW_PER_CHUNK);
		config.config.horizontal_input_feature_chunk_num = ceil(1.0*input_width/MAX_FEATURES_COL_PER_CHUNK);
		vertical_output_feature_chunk_num = ceil(1.0*output_height/MAX_FEATURES_ROW_PER_CHUNK);
		horizontal_output_feature_chunk_num = ceil(1.0*output_width/MAX_FEATURES_COL_PER_CHUNK);
		config.num_of_kernels_per_group = nk;
	}
};


#endif
