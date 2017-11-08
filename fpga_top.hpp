#ifndef FPGA_TOP_HPP__
#define FPGA_TOP_HPP__

#include"cpu_top.hpp"


int Accelerator(feature_type compressed_input_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					zeros_type compressed_input_feature_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					int num_of_none_zero_input_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM],
					weight_type compressed_weight[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
					zeros_type compressed_weight_index[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
					int num_of_none_zero_weights[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM],
					feature_type compressed_output_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					zeros_type output_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					int num_of_none_zero_output_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM]);

#endif
