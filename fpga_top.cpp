#include<cstring>
#include<cassert>
#include"fpga_top.hpp"
#include"ProcessElement.hpp"


static struct ProcessElement PE[NUM_OF_PEs];

static int num_of_weights_per_chunk[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM];
static zeros_type zeros[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
static weight_type weight[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];

extern int num_of_none_zero_output_features[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM];
extern feature_type compressed_output_feature[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
extern zeros_type compressed_output_feature_index[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];


static inline void LoadFeatureMapForPEs(feature_type compressed_input_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type compressed_input_feature_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				int num_of_none_zero_input_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM]){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<NUM_OF_PEs;j++){
			PE[j].num_of_none_zero_features[i]=num_of_none_zero_input_features[i][j];
			memcpy(PE[j].featuremap[i],compressed_input_feature[i][j],sizeof(feature_type)*num_of_none_zero_input_features[i][j]);
			memcpy(PE[j].featureindex[i],compressed_input_feature_index[i][j],sizeof(zeros_type)*num_of_none_zero_input_features[i][j]);
		}
	}
}


static inline void LoadCompressedWeights(weight_type compressed_weight[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
		zeros_type compressed_weight_index[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
		int num_of_none_zero_weights[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM]){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<WEIGHT_CHUNK_NUM;j++){
			num_of_weights_per_chunk[i][j] = num_of_none_zero_weights[i][j];
			memcpy(weight[i][j],compressed_weight[i][j],sizeof(weight_type)*num_of_none_zero_weights[i][j]);
			memcpy(zeros[i][j],compressed_weight_index[i][j],sizeof(zeros_type)*num_of_none_zero_weights[i][j]);
		}
	}
}


static inline void BroadcastWeights(weight_type weights[F], zeros_type index[F]){
	for (int i=0;i<NUM_OF_PEs;i++){
		memcpy(PE[i].weight,weights,F*sizeof(weight_type));
		memcpy(PE[i].weightindex,index,F*sizeof(zeros_type));
	}
}


static inline void AccumulateProduct(){
	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].AccumulateProduct();
	}
}


static inline void SetCurrentInputChannel(int channel){
	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].current_input_channel = channel;
	}
}


static void CollectAndCompressResults(){
	for(int i=0;i<NUM_OF_PEs;i++){
		for(int j=0;j<OUTPUT_CHANNEL_NUM;j++){
			int chunk_idx = 0;
			int zero_count = 0;
			for (int k=0;k<FEATURES_ROW_PER_CHUNK;k++){
				for (int l=0;l<FEATURES_COL_PER_CHUNK;l++){
					if (PE[i].accumulaor[j][k][l]){
						compressed_output_feature[j][i][chunk_idx]=PE[i].accumulaor[j][k][l];
						compressed_output_feature_index[j][i][chunk_idx] = zero_count;
						chunk_idx = chunk_idx + 1;
						zero_count = 0;
					}else{
						zero_count = zero_count + 1;
						if (zero_count==MAX_ZERO_COUNT){
							compressed_output_feature_index[j][i][chunk_idx] = zero_count;
							compressed_output_feature[j][i][chunk_idx] = 0;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}
					}
				}
			}
			num_of_none_zero_output_features[j][i] = chunk_idx;
		}
	}
}


int Accelerator(feature_type compressed_input_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type compressed_input_feature_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				int num_of_none_zero_input_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM],
				weight_type compressed_weight[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				zeros_type compressed_weight_index[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				int num_of_none_zero_weights[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM],
				feature_type compressed_output_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type output_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				int num_of_none_zero_output_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM]){

	zeros_type index_buf[F];
	weight_type weight_buf[F];

	LoadCompressedWeights(compressed_weight,compressed_weight_index,num_of_none_zero_weights);

	LoadFeatureMapForPEs(compressed_input_feature,compressed_input_feature_index,num_of_none_zero_input_features);

	for (int i=0;i<WEIGHT_CHUNK_NUM;i++){
		for (int j=0;j<INPUT_CHANNEL_NUM;j++){
			SetCurrentInputChannel(j);
			for (int k=0;k<num_of_weights_per_chunk[j][i];k+=F){
				memcpy(weight_buf,weight[j][i]+k,sizeof(weight_type)*F);
				memcpy(index_buf,zeros[j][i]+k,sizeof(zeros_type)*F);
				BroadcastWeights(weight_buf,index_buf);
				AccumulateProduct();
			}
		}
		CollectAndCompressResults();
	}

	return 0;
}
