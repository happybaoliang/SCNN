#ifndef FPGA_TOP_HPP__
#define FPGA_TOP_HPP__

#include"cpu_top.hpp"


int Accelerator(feature_type compressed_input_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					zeros_type compressed_input_feature_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					size_type num_of_none_zero_input_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM],
					size_type max_none_zero_features[INPUT_CHANNEL_NUM],
					weight_type compressed_weight[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
					zeros_type compressed_weight_index[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
					size_type num_of_none_zero_weights[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM],
					feature_type compressed_output_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					zeros_type output_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
					size_type num_of_none_zero_output_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM]);

#endif
