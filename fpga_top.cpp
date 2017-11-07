#include<cstring>
#include<cassert>
#include"fpga_top.hpp"
#include"ProcessElement.hpp"


struct ProcessElement PE[NUM_OF_PEs];

extern int num_of_none_zero_output_features[FEATURE_CHUNK_NUM];
extern zeros_type output_index[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
extern feature_type output_feature[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];


int num_of_weights_per_chunk[WEIGHT_CHUNK_NUM];
zeros_type zeros[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
weight_type weight[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];


inline void LoadFeatureMapForPE(feature_type featuremap[MAX_NUM_OF_FEATURE_PER_CHUNK], int pe){
	memcpy(PE[pe].featuremap,featuremap,PE[pe].num_of_none_zero_features*sizeof(feature_type));
}


inline void LoadFeatureIndexForPE(zeros_type featureindex[MAX_NUM_OF_FEATURE_PER_CHUNK], int pe){
	memcpy(PE[pe].featureindex,featureindex,PE[pe].num_of_none_zero_features*sizeof(zeros_type));
}


inline void BroadcastFWeights(weight_type* weights, zeros_type* index,int start){
	for (int i=0;i<NUM_OF_PEs;i++){
		memcpy(PE[i].weight,weights+start,F*sizeof(weight_type));
		memcpy(PE[i].weightindex,index+start,F*sizeof(zeros_type));
	}
}


void CollectResults(int pe){
	int chunk_idx = 0;
	int zero_count = 0;
	for (int k=0;k<OUTPUT_CHANNEL_NUM;k++){
		for (int l=0;l<FEATURES_ROW_PER_CHUNK;l++){
			for (int m=0;m<FEATURES_COL_PER_CHUNK;m++){
				if(PE[pe].output_feature[k][l][m]){
					output_feature[pe][chunk_idx] = PE[pe].output_feature[k][l][m];
					output_index[pe][chunk_idx] = zero_count;
					chunk_idx = chunk_idx + 1;
					zero_count = 0;
				}else{
					zero_count ++;
					if (zero_count==MAX_ZERO_COUNT){
						output_index[pe][chunk_idx] = zero_count;
						output_feature[pe][chunk_idx]=0;
						chunk_idx = chunk_idx + 1;
						zero_count = 0;
					}
				}
			}
		}
	}
	num_of_none_zero_output_features[pe] = chunk_idx;
}


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
				int num_of_none_zero_output_features[FEATURE_CHUNK_NUM]){

	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].num_of_none_zero_features = num_of_none_zero_features[i];
		PE[i].total_input_channel = total_input_channel;
		LoadFeatureMapForPE(compressed_feature[i],i);
		LoadFeatureIndexForPE(feature_index[i],i);
		PE[i].kernel_size = kernel_size;
	}

	for (int i=0;i<WEIGHT_CHUNK_NUM;i++){
		num_of_weights_per_chunk[i] = num_of_none_zero_weights[i];
		memcpy(zeros[i],weight_index[i],sizeof(zeros_type)*num_of_none_zero_weights[i]);
		memcpy(weight[i],compressed_weight[i],sizeof(weight_type)*num_of_none_zero_weights[i]);
	}

	for (int i=0;i<WEIGHT_CHUNK_NUM;i++){
		assert((num_of_none_zero_weights[i]%F)==0);
		for (int j=0;j<num_of_none_zero_weights[i];j+=F){
			BroadcastFWeights(weight[i],zeros[i],j);
		}
	}

	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].AccumulateProduct();
		CollectResults(i);
	}

	return 0;
}
