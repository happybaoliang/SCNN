#include<cstring>
#include<cassert>
#include"fpga_top.hpp"
#include"ProcessElement.hpp"


struct ProcessElement PE[NUM_OF_PEs];

int num_of_weights_per_chunk[WEIGHT_CHUNK_NUM];
zeros_type zeros[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
weight_type weight[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];


inline void LoadFeatureMapForPE(feature_type featuremap[MAX_NUM_OF_FEATURE_PER_CHUNK], int pe){
	memcpy(PE[pe].featuremap,featuremap,PE[pe].num_of_none_zero_features*sizeof(feature_type));
}


inline void LoadFeatureIndexForPE(zeros_type featureindex[MAX_NUM_OF_FEATURE_PER_CHUNK], int pe){
	memcpy(PE[pe].featureindex,featureindex,PE[pe].num_of_none_zero_features*sizeof(zeros_type));
}


inline void BroadcastWeight(weight_type* weights, zeros_type* index,int start){
	for (int i=0;i<NUM_OF_PEs;i++){
		memcpy(PE[i].weight,weights+start*sizeof(weight_type),F*sizeof(weight_type));
		memcpy(PE[i].weightindex,index+start*sizeof(zeros_type),F*sizeof(zeros_type));
	}

	for(int i=0;i<NUM_OF_PEs;i++){
		PE[i].AccumulateProduct();
	}
}


void CollectResults(int chunk_id){

}


int Accelerator(feature_type compressed_feature[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type feature_index[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				int num_of_none_zero_features,
				weight_type compressed_weight[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				zeros_type weight_index[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				int num_of_none_zero_weights[WEIGHT_CHUNK_NUM],
				feature_type output_feature[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type output_index[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				int num_of_none_zero_output_features){
	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].num_of_none_zero_features = num_of_none_zero_features;
		LoadFeatureMapForPE(compressed_feature[i],i);
		LoadFeatureIndexForPE(feature_index[i],i);
	}

	for (int i=0;i<WEIGHT_CHUNK_NUM;i++){
		num_of_weights_per_chunk[i] = num_of_none_zero_weights[i];
		memcpy(zeros[i],weight_index[i],sizeof(zeros_type)*num_of_none_zero_weights[i]);
		memcpy(weight[i],compressed_weight[i],sizeof(weight_type)*num_of_none_zero_weights[i]);
	}

	for (int i=0;i<WEIGHT_CHUNK_NUM;i++){
		assert((num_of_none_zero_weights[i]%F)==0);
		for (int j=0;j<num_of_none_zero_weights[i];j+=F){
			BroadcastWeight(weight[i],zeros[i],j);
		}
		CollectResults(i);
	}

	return 0;
}
