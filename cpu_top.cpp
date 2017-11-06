#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cmath>
#include<ctime>
#include"cpu_top.hpp"

using namespace std;


int num_of_none_zero_features = 0;
zeros_type feature_index[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
feature_type input_feature[INPUT_CHANNEL_NUM][FEATURE_WIDTH][FEATURE_HEIGHT];
feature_type compressed_feature[FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];

int num_of_none_zero_weights = 0;
zeros_type weight_index[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
weight_type compressed_weight[WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
weight_type weights[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_NUM][KERNEL_SIZE][KERNEL_SIZE];


int GetRandomNumber(int max_val){
	return rand()%max_val;
}


void GenerateRandomWeight(channel_type num_in_channel, channel_type num_out_channel, int kernel_size){
	for (int i=0;i<num_in_channel;i++){
		for (int j=0;j<num_out_channel;j++){
			for (int k=0;k<kernel_size;k++){
				for (int l=0;l<kernel_size;l++){
					weights[i][j][k][l] = GetRandomNumber(MAX_WEIGHT_VALUE);
				}
			}
		}
	}
}


void DumpGeneratedWeight(const char* filename, channel_type num_in_channel, channel_type num_out_channel, int kernel_size){
	ofstream output(filename);
	if(output==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<num_in_channel;i++){
		for (int j=0;j<num_out_channel;j++){
			for (int k=0;k<kernel_size;k++){
				for (int l=0;l<kernel_size;l++){
					output<<weights[i][j][k][l]<<" ";
				}
			}
			output<<endl;
		}
		output<<endl;
	}

	output.close();
}


void LoadGeneratedWeight(const char* filename, channel_type num_in_channel, channel_type num_out_channel, int kernel_size){
	ifstream input(filename);
	if(input==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<num_in_channel;i++){
		for (int j=0;j<num_out_channel;j++){
			for (int k=0;k<kernel_size;k++){
				for (int l=0;l<kernel_size;l++){
					input>>weights[i][j][k][l];
				}
			}
		}
	}

	input.close();
}


void GenerateRandomFeatureMap(channel_type num_channel, size_type num_width, size_type num_height){
	for (int i=0;i<num_channel;i++){
		for (int j=0;j<num_width;j++){
			for (int k=0;k<num_height;k++){
				input_feature[i][j][k] = GetRandomNumber(MAX_FEATURE_VALUE);
			}
		}
	}
}


void DumpGeneratedFeatureMap(const char* filename, channel_type num_channel, size_type num_width, size_type num_height){
	ofstream output(filename);
	if (output==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<num_channel;i++){
		for (int j=0;j<num_width;j++){
			for (int k=0;k<num_height;k++){
				output<<input_feature[i][j][k]<<" ";
			}
		}
		output<<endl;
	}

	output.close();
}


void LoadGeneratedFeatureMap(const char* filename, channel_type num_channel, size_type num_width, size_type num_height){
	ifstream input(filename);
	if (input==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<num_channel;i++){
		for (int j=0;j<num_width;j++){
			for (int k=0;k<num_height;k++){
				input>>input_feature[i][j][k];
			}
		}
	}

	input.close();
}


void CompressWeights(channel_type num_in_channel, channel_type num_out_channel, int chunk_size, int kernel_size){
	for (int i=0;i<CEIL_DIV(num_out_channel,chunk_size);i++){
		int chunk_idx = 0;
		int zero_count = 0;
		for (int k=0;k<num_in_channel;k++){
			for (int j=0;j<chunk_size;j++){
				for (int l=0;l<kernel_size;l++){
					for (int m=0;m<kernel_size;m++){
						if (weights[k][i*chunk_size+j][l][m]!=0){
							compressed_weight[i][chunk_idx]=weights[k][i*chunk_size+j][l][m];
							weight_index[i][chunk_idx]=zero_count;
							num_of_none_zero_weights ++;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}else{
							zero_count ++;
							if (zero_count==MAX_ZERO_COUNT){
								weight_index[i][chunk_idx] = zero_count;
								compressed_weight[i][chunk_idx]=0;
								num_of_none_zero_weights++;
								chunk_idx = chunk_idx + 1;
								zero_count = 0;
							}
						}
					}
				}
			}
		}
	}
}


void CompressFeatureMap(int num_in_channel, int width, int height, int chunk_per_col, int chunk_per_row){
	int chunk_id = 0;
	for (int i=0;i<chunk_per_row;i++){
		for (int j=0;j<chunk_per_col;j++){
			int chunk_idx = 0;
			int zero_count = 0;
			chunk_id = chunk_id + 1;
			for (int k=0;k<num_in_channel;k++){
				for (int l=0;l<chunk_per_row;l++){
					for (int m=0;m<chunk_per_col;m++){
						if(input_feature[k][i*chunk_per_row+l][j*chunk_per_col+m]){
							compressed_feature[chunk_id][chunk_idx] = input_feature[k][i*chunk_per_row+l][j*chunk_per_col+m];
							feature_index[chunk_id][chunk_idx] = zero_count;
							num_of_none_zero_features ++;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}else{
							zero_count ++;
							if (zero_count==MAX_ZERO_COUNT){
								feature_index[chunk_id][chunk_idx] = zero_count;
								compressed_feature[chunk_id][chunk_idx]=0;
								num_of_none_zero_features ++;
								chunk_idx = chunk_idx + 1;
								zero_count = 0;
							}
						}
					}
				}
			}
		}
	}
}


int main(){
	srand((unsigned)time(NULL));

#if 0
	GenerateRandomWeight(INPUT_CHANNEL_NUM,OUTPUT_CHANNEL_NUM,KERNEL_SIZE);
	DumpGeneratedWeight("../../../weights.bin",INPUT_CHANNEL_NUM,OUTPUT_CHANNEL_NUM,KERNEL_SIZE);
	GenerateRandomFeatureMap(INPUT_CHANNEL_NUM,FEATURE_WIDTH,FEATURE_HEIGHT);
	DumpGeneratedFeatureMap("../../../feature.bin",INPUT_CHANNEL_NUM,FEATURE_WIDTH,FEATURE_HEIGHT);
#else
	LoadGeneratedWeight("../../../weights.bin",INPUT_CHANNEL_NUM,OUTPUT_CHANNEL_NUM,KERNEL_SIZE);
	LoadGeneratedFeatureMap("../../../feature.bin",INPUT_CHANNEL_NUM,FEATURE_WIDTH,FEATURE_HEIGHT);
#endif

	CompressWeights(INPUT_CHANNEL_NUM,OUTPUT_CHANNEL_NUM,WEIGHT_CHUNK_SIZE,KERNEL_SIZE);
	CompressFeatureMap(INPUT_CHANNEL_NUM,FEATURE_WIDTH,FEATURE_HEIGHT,FEATURE_CHUNK_PER_COL,FEATURE_CHUNK_PER_ROW);

	return 0;
}
