#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cassert>
#include<ctime>

#include"cpu_top.hpp"
#include"fpga_top.hpp"

using namespace std;


static weight_type weights[INPUT_CHANNEL_NUM][OUTPUT_CHANNEL_NUM][KERNEL_SIZE][KERNEL_SIZE];

static int num_of_none_zero_weights[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM] = {0};
static weight_type compressed_weight[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
static zeros_type compressed_weight_index[INPUT_CHANNEL_NUM][WEIGHT_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];

static feature_type input_feature[INPUT_CHANNEL_NUM][INPUT_FEATURE_HEIGHT][INPUT_FEATURE_WIDTH];

static int num_of_none_zero_input_features[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM] = {0};
static feature_type compressed_input_feature[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
static zeros_type compressed_input_feature_index[INPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];

int num_of_none_zero_output_features[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM] = {0};
zeros_type compressed_output_feature_index[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
feature_type compressed_output_feature[OUTPUT_CHANNEL_NUM][FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];

static feature_type decompressed_output_feature[INPUT_CHANNEL_NUM][INPUT_FEATURE_WIDTH][INPUT_FEATURE_HEIGHT];


static inline int GetRandomNumber(int max_val){
	return rand()%max_val;
}


static void GenerateRandomWeight(){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<OUTPUT_CHANNEL_NUM;j++){
			for (int k=0;k<KERNEL_SIZE;k++){
				for (int l=0;l<KERNEL_SIZE;l++){
					weights[i][j][k][l] = GetRandomNumber(MAX_WEIGHT_VALUE);
				}
			}
		}
	}
}


static void DumpGeneratedWeight(const char* filename){
	ofstream output(filename);
	if(output==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<OUTPUT_CHANNEL_NUM;j++){
			for (int k=0;k<KERNEL_SIZE;k++){
				for (int l=0;l<KERNEL_SIZE;l++){
					output<<weights[i][j][k][l]<<" ";
				}
			}
			output<<endl;
		}
		output<<endl;
	}

	output.close();
}


static void LoadGeneratedWeight(const char* filename){
	ifstream input(filename);
	if(input==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<OUTPUT_CHANNEL_NUM;j++){
			for (int k=0;k<KERNEL_SIZE;k++){
				for (int l=0;l<KERNEL_SIZE;l++){
					input>>weights[i][j][k][l];
				}
			}
		}
	}

	input.close();
}


static void GenerateRandomFeatureMap(){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<INPUT_FEATURE_HEIGHT;j++){
			for (int k=0;k<INPUT_FEATURE_WIDTH;k++){
				input_feature[i][j][k] = GetRandomNumber(MAX_FEATURE_VALUE);
			}
		}
	}
}


static void DumpGeneratedFeatureMap(const char* filename){
	ofstream output(filename);
	if (output==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<INPUT_FEATURE_HEIGHT;j++){
			for (int k=0;k<INPUT_FEATURE_WIDTH;k++){
				output<<input_feature[i][j][k]<<" ";
			}
		}
		output<<endl;
	}

	output.close();
}


static void LoadGeneratedFeatureMap(const char* filename){
	ifstream input(filename);
	if (input==NULL){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<INPUT_FEATURE_HEIGHT;j++){
			for (int k=0;k<INPUT_FEATURE_WIDTH;k++){
				input>>input_feature[i][j][k];
			}
		}
	}

	input.close();
}


static void CompressInputFeatureMap(){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<VERTICAL_FEATURE_CHUNK_NUM;j++){
			for (int k=0;k<HORIZONTAL_FEATURE_CHUNK_NUM;k++){
				int chunk_idx = 0;
				int zero_count = 0;
				int chunk_id = j*HORIZONTAL_FEATURE_CHUNK_NUM+k;
				for (int l=0;l<FEATURES_ROW_PER_CHUNK;l++){
					for (int m=0;m<FEATURES_COL_PER_CHUNK;m++){
						int absolute_row = j*FEATURES_ROW_PER_CHUNK+l;
						int absolute_col = k*FEATURES_COL_PER_CHUNK+m;
						feature_type feature = input_feature[i][absolute_row][absolute_col];
						if (feature!=0){
							compressed_input_feature_index[chunk_id][i][chunk_idx] = zero_count;
							compressed_input_feature[chunk_id][i][chunk_idx] = feature;

							chunk_idx = chunk_idx + chunk_idx;
							zero_count = 0;
						}else{
							zero_count = zero_count + 1;
							if (zero_count == MAX_ZERO_COUNT){
								compressed_input_feature_index[chunk_id][i][chunk_idx] = zero_count;
								compressed_input_feature[chunk_id][i][chunk_idx] = 0;
								chunk_idx = chunk_idx + chunk_idx;
								zero_count = 0;
							}
						}
					}
				}

				num_of_none_zero_input_features[chunk_id][i] = chunk_idx;

				while((num_of_none_zero_input_features[chunk_id][i]%I)!=0){
					compressed_input_feature_index[chunk_id][i][chunk_idx] = 0;
					compressed_input_feature[chunk_id][i][chunk_idx] = 0;
					num_of_none_zero_input_features[chunk_id][i]++;
					chunk_idx = chunk_idx + chunk_idx;
				}
			}
		}
	}
}


static void CompressWeights(){
	for (int i=0;i<INPUT_CHANNEL_NUM;i++){
		for (int j=0;j<WEIGHT_CHUNK_NUM;j++){
			int chunk_idx = 0;
			int zero_count = 0;
			for (int k=0;k<WEIGHT_CHUNK_SIZE;k++){
				for (int l=0;l<KERNEL_SIZE;l++){
					for (int m=0;m<KERNEL_SIZE;m++){
						if (weights[i][j*WEIGHT_CHUNK_SIZE+k][l][m]!=0){
							compressed_weight[i][j][chunk_idx] = weights[i][j*WEIGHT_CHUNK_SIZE+k][l][m];
							compressed_weight_index[i][j][chunk_idx] = zero_count;

							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}else{
							zero_count = zero_count + 1;
							if (zero_count==MAX_ZERO_COUNT){
								compressed_weight_index[i][j][chunk_idx] = zero_count;
								compressed_weight[i][j][chunk_idx] = 0;
								chunk_idx = chunk_idx + 1;
								zero_count = 0;
							}
						}
					}
				}
			}

			num_of_none_zero_weights[i][j] = chunk_idx;

			while((num_of_none_zero_weights[i][j]%F)!=0){
				compressed_weight_index[i][j][chunk_idx] = 0;
				compressed_weight[i][j][chunk_idx] = 0;
				num_of_none_zero_weights[i][j] ++;
				chunk_idx = chunk_idx + 1;
			}
		}
	}
}


static void DeCompressOutputFeatureMap(){
	for (int i=0;i<OUTPUT_CHANNEL_NUM;i++){
		for (int j=0;j<OUTPUT_FEATURE_WIDTH;j++){
			for (int k=0;k<OUTPUT_FEATURE_HEIGHT;k++){
				decompressed_output_feature[i][j][k] = 0;
			}
		}
	}

	for (int i=0;i<OUTPUT_CHANNEL_NUM;i++){
		for (int j=0;j<VERTICAL_FEATURE_CHUNK_NUM;j++){
			for (int k=0;k<HORIZONTAL_FEATURE_CHUNK_NUM;k++){
				int total_features_processed_in_chunk = 0;
				int chunk_id = j*HORIZONTAL_FEATURE_CHUNK_NUM+k;
				for (int l=0;l<num_of_none_zero_output_features[i][chunk_id];l++){
					total_features_processed_in_chunk += compressed_output_feature_index[i][chunk_id][l]+1;
					int chunk_row = total_features_processed_in_chunk / FEATURES_COL_PER_CHUNK;
					int chunk_col = total_features_processed_in_chunk % FEATURES_COL_PER_CHUNK;
					decompressed_output_feature[i][FEATURES_ROW_PER_CHUNK*j+chunk_row]\
						[FEATURES_COL_PER_CHUNK*k+chunk_col] = compressed_output_feature[i][chunk_id][l];
				}
			}
		}
	}
}


static int CheckDeCompressedConvolutionResults(){
	int error_count = 0;

	for (int i=0;i<OUTPUT_CHANNEL_NUM;i++){
		for (int k=0;k<OUTPUT_FEATURE_HEIGHT;k++){
			for (int l=0;l<OUTPUT_FEATURE_WIDTH;l++){
				feature_type temp = 0;
				for (int j=0;j<INPUT_CHANNEL_NUM;j++){
					for (int m=-KERNEL_SIZE/2;m<=KERNEL_SIZE/2;m++){
						for (int n=-KERNEL_SIZE/2;n<=KERNEL_SIZE/2;n++){
							int xcoord = k+m;
							int ycoord = l+n;
							if (xcoord<0||xcoord>OUTPUT_FEATURE_HEIGHT)
								continue;
							if (ycoord<0||ycoord>OUTPUT_FEATURE_WIDTH)
								continue;
							temp += weights[j][i][m][n]*input_feature[j][k+m][l+n];
						}
					}
				}
				if (temp != decompressed_output_feature[i][k][l]){
					error_count ++;
				}
			}
		}
	}

	return error_count;
}


int main(){
	srand((unsigned)time(NULL));
	assert((MAX_NUM_OF_FEATURE_PER_CHUNK%I)==0);
	assert((MAX_NUM_OF_WEIGHTS_PER_CHUNK%F)==0);
	assert((OUTPUT_CHANNEL_NUM%WEIGHT_CHUNK_NUM)==0);

#if 0
	GenerateRandomWeight();
	GenerateRandomFeatureMap();
	DumpGeneratedWeight("../../../weights.bin");
	DumpGeneratedFeatureMap("../../../feature.bin");
#else
	LoadGeneratedWeight("../../../weights.bin");
	LoadGeneratedFeatureMap("../../../feature.bin");
#endif

	CompressWeights();
	CompressInputFeatureMap();

	Accelerator(compressed_input_feature,compressed_input_feature_index,num_of_none_zero_input_features,
			compressed_weight,compressed_weight_index,num_of_none_zero_weights,compressed_output_feature,
			compressed_output_feature_index,num_of_none_zero_output_features);

	DeCompressOutputFeatureMap();

	return CheckDeCompressedConvolutionResults();
}
