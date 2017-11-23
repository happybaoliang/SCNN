#include<iostream>
#include<cstring>
#include<cassert>

#include"fpga_top.hpp"

using namespace std;


struct ProcessElement PE[NUM_OF_PEs];

static size_type max_num_of_none_zero_features[INPUT_CHANNEL_NUM]={0};

hls::stream<Flit> left_halos_channel[VERTICAL_FEATURE_CHUNK_NUM][HORIZONTAL_FEATURE_CHUNK_NUM-1];
hls::stream<Flit> right_halos_channel[VERTICAL_FEATURE_CHUNK_NUM][HORIZONTAL_FEATURE_CHUNK_NUM-1];

hls::stream<Flit> down_halos_channel[VERTICAL_FEATURE_CHUNK_NUM-1][HORIZONTAL_FEATURE_CHUNK_NUM];
hls::stream<Flit> upper_halos_channel[VERTICAL_FEATURE_CHUNK_NUM-1][HORIZONTAL_FEATURE_CHUNK_NUM];

static size_type num_of_weights_per_chunk[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM];
static zeros_type zeros[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
static weight_type weight[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];

extern size_type num_of_none_zero_output_features[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM];
extern feature_type compressed_output_feature[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
extern zeros_type compressed_output_feature_index[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];


static inline void LoadFeatureMapForPEs(feature_type compressed_input_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type compressed_input_feature_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				size_type num_of_none_zero_input_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM]){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<NUM_OF_PEs;j++){
			PE[j].num_of_none_zero_features[i]=num_of_none_zero_input_features[i][j];
			memcpy(PE[j].featuremap[i],compressed_input_feature[i][j],sizeof(feature_type)*num_of_none_zero_input_features[i][j]);
			memcpy(PE[j].featureindex[i],compressed_input_feature_index[i][j],sizeof(zeros_type)*num_of_none_zero_input_features[i][j]);
		}
	}
}


static inline void LoadCompressedWeights(weight_type compressed_weight[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
		zeros_type compressed_weight_index[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
		size_type num_of_none_zero_weights[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM]){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<OUTPUT_CHANNEL_CHUNK_NUM;j++){
			num_of_weights_per_chunk[i][j] = num_of_none_zero_weights[i][j];
			memcpy(weight[i][j],compressed_weight[i][j],sizeof(weight_type)*num_of_none_zero_weights[i][j]);
			memcpy(zeros[i][j],compressed_weight_index[i][j],sizeof(zeros_type)*num_of_none_zero_weights[i][j]);
		}
	}
}


static inline void BroadcastWeights(weight_type weights[F], zeros_type index[F]){
	//cout<<"broadcast F weights"<<endl;
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


static inline void SetNextInputChannel(channel_type channel){
	//cout<<"next channel:"<<channel<<endl;
	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].SetNextInputChannel(channel);
	}
}


static void DrainOutProducts(){
	for (int i=0;i<NUM_OF_PEs;i++){
		cout<<"PE["<<i<<"] draining..."<<endl;
		PE[i].DrainOutProducts();
	}
}


static void CollectAndCompressResults(size_type chunk){
	DrainOutProducts();

	for(int i=0;i<NUM_OF_PEs;i++){
		for(int j=0;j<OUTPUT_CHANNEL_CHUNK_SIZE;j++){
			size_type chunk_idx = 0;
			size_type zero_count = 0;
			channel_type out = chunk*OUTPUT_CHANNEL_CHUNK_SIZE+j;
			for (int k=0;k<FEATURES_ROW_PER_CHUNK;k++){
				for (int l=0;l<FEATURES_COL_PER_CHUNK;l++){
					product_type product = PE[i].acc.get_and_clear(j,k,l);
					if (product){
						compressed_output_feature_index[out][i][chunk_idx] = zero_count;
						compressed_output_feature[out][i][chunk_idx]=product;
						chunk_idx = chunk_idx + 1;
						zero_count = 0;
					}else{
						zero_count = zero_count + 1;
						if (zero_count==MAX_ZERO_COUNT){
							compressed_output_feature_index[out][i][chunk_idx] = zero_count;
							compressed_output_feature[out][i][chunk_idx] = 0;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}
					}
				}
			}
			num_of_none_zero_output_features[out][i] = chunk_idx;
		}
	}
}


static void inline FetchNextIFeatureMap(){
	//cout<<"fetch next I featuremap"<<endl;
	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].FetchNextIFeatureMap();
	}
}


static void inline ConnectAllProcessElement(){
	for (pe_coord_type i=0;i<VERTICAL_FEATURE_CHUNK_NUM;i++){
		for (pe_coord_type j=0;j<HORIZONTAL_FEATURE_CHUNK_NUM;j++){
			if (j<(HORIZONTAL_FEATURE_CHUNK_NUM-1)){
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[RIGHT_PORT] = &right_halos_channel[i][j];
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j+1].output_halos[LEFT_PORT] = &right_halos_channel[i][j];
			}else{
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[RIGHT_PORT] = NULL;
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].output_halos[RIGHT_PORT] = NULL;
			}
			if (j>0){
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[LEFT_PORT] = &left_halos_channel[i][j-1];
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j-1].output_halos[RIGHT_PORT] = &left_halos_channel[i][j-1];
			}else{
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[LEFT_PORT] = NULL;
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].output_halos[LEFT_PORT] = NULL;
			}
			if (i>0){
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[UPPER_PORT] = &upper_halos_channel[i-1][j];
				PE[(i-1)*HORIZONTAL_FEATURE_CHUNK_NUM+j].output_halos[DOWN_PORT] = &upper_halos_channel[i-1][j];
			}else{
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[UPPER_PORT] = NULL;
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].output_halos[UPPER_PORT] = NULL;
			}
			if (i<(VERTICAL_FEATURE_CHUNK_NUM-1)){
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[DOWN_PORT] = &down_halos_channel[i][j];
				PE[(i+1)*HORIZONTAL_FEATURE_CHUNK_NUM+j].output_halos[UPPER_PORT] = &down_halos_channel[i][j];
			}else{
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].input_halos[DOWN_PORT] = NULL;
				PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].output_halos[DOWN_PORT] = NULL;
			}
		}
	}
}


static void inline ResetAllProcessElement(){
	for (pe_coord_type i=0;i<VERTICAL_FEATURE_CHUNK_NUM;i++){
		for (pe_coord_type j=0;j<HORIZONTAL_FEATURE_CHUNK_NUM;j++){
			PE[i*HORIZONTAL_FEATURE_CHUNK_NUM+j].ResetProcessElement(i,j);
		}
	}
}


int Accelerator(feature_type compressed_input_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type compressed_input_feature_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				size_type num_of_none_zero_input_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM],
				size_type max_none_zero_features[INPUT_CHANNEL_NUM],
				weight_type compressed_weight[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				zeros_type compressed_weight_index[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK],
				size_type num_of_none_zero_weights[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_CHUNK_NUM],
				feature_type compressed_output_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				zeros_type output_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK],
				size_type num_of_none_zero_output_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM]){

	zeros_type index_buf[F];
	weight_type weight_buf[F];

	ConnectAllProcessElement();

	ResetAllProcessElement();

	memcpy(max_num_of_none_zero_features,max_none_zero_features,INPUT_CHANNEL_NUM*sizeof(int));

	LoadCompressedWeights(compressed_weight,compressed_weight_index,num_of_none_zero_weights);

	LoadFeatureMapForPEs(compressed_input_feature,compressed_input_feature_index,num_of_none_zero_input_features);

	for (size_type i=0;i<OUTPUT_CHANNEL_CHUNK_NUM;i++){
		for (channel_type j=0;j<INPUT_CHANNEL_NUM;j++){
			SetNextInputChannel(j);
			for(size_type k=0;k<max_num_of_none_zero_features[j];k+=I){
				FetchNextIFeatureMap();
				for (size_type l=0;l<num_of_weights_per_chunk[j][i];l+=F){
					memcpy(weight_buf,weight[j][i]+l,sizeof(weight_type)*F);
					memcpy(index_buf,zeros[j][i]+l,sizeof(zeros_type)*F);
					BroadcastWeights(weight_buf,index_buf);
					AccumulateProduct();
				}
			}
		}
		CollectAndCompressResults(i);
	}

	return 0;
}
