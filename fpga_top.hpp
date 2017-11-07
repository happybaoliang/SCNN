#ifndef FPGA_TOP_HPP__
#define FPGA_TOP_HPP__

#include"cpu_top.hpp"


int Accelerator(feature_type compressed_feature[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type feature_index[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				int num_of_none_zero_features[FEATURE_CHUNK_NUM],
				int total_input_channel,
				weight_type compressed_weight[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				zeros_type weight_index[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				int num_of_none_zero_weights[WEIGHT_CHUNK_NUM],
				int kernel_size,
				feature_type output_feature[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type output_index[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				int num_of_none_zero_output_features[FEATURE_CHUNK_NUM]);

#endif
