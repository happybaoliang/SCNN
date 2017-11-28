#include<iostream>
#include<fstream>
#include<cassert>
#include<cmath>
#include"layer.hpp"

using namespace std;


layer_t::layer_t(){
	config.config.stride = 0;
	config.config.pad = true;
	config.relu = true;
	weights = NULL;
	config.config.kernel_size = 0;
	config.next_layer_kernel_size = 0;
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
	config.vertical_output_feature_chunk_num = 0;
	config.horizontal_output_feature_chunk_num = 0;
}


layer_t::layer_t(const layer_t& layer){
	config.config.pad = layer.config.config.pad;
	config.relu = layer.config.relu;
	config.config.stride = layer.config.config.stride;
	weights = NULL;
	config.config.kernel_size = layer.config.config.kernel_size;
	input_width = layer.input_width;
	input_height = layer.input_height;
	config.input_channels = layer.config.input_channels;
	output_channels = layer.output_channels;
	input_features = NULL;
	output_features = NULL;
	config.next_layer_kernel_size = layer.config.next_layer_kernel_size;
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
	output_width = layer.output_width;
	output_height = layer.output_height;
	config.num_of_output_channel_groups = layer.config.num_of_output_channel_groups;
	config.config.vertical_input_feature_chunk_num = layer.config.config.vertical_input_feature_chunk_num;
	config.config.horizontal_input_feature_chunk_num = layer.config.config.horizontal_input_feature_chunk_num;
	config.vertical_output_feature_chunk_num = layer.config.vertical_output_feature_chunk_num;
	config.horizontal_output_feature_chunk_num = layer.config.horizontal_output_feature_chunk_num;
	config.num_of_kernels_per_group = layer.config.num_of_kernels_per_group;
}


layer_t::layer_t(dimension_t w, dimension_t h, input_channel_t ci, output_channel_t co,
		kernel_t k, kernel_t next_k, bool p, stride_t s, bool r, output_channel_t nk){
	assert((s>=1) && (s<=MAX_STRIDE));
	assert((k>=1) && (k<=MAX_KERNEL_SIZE));
	assert((ci>=1) && (ci<=MAX_INPUT_CHANNEL_NUM));
	assert((co>=1) && (co<=MAX_OUTPUT_CHANNEL_NUM));
	assert((nk>=1) && (nk<=MAX_OUTPUT_CHANNEL_NUM));
	assert((next_k>=1) && (next_k<=MAX_KERNEL_SIZE));
	assert((w>=MAX_FEATURES_COL_PER_CHUNK) && (w<=MAX_FEATURE_DIMENSION));
	assert((h>=MAX_FEATURES_ROW_PER_CHUNK) && (h<=MAX_FEATURE_DIMENSION));
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
	config.next_layer_kernel_size = next_k;
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
	dimension_t padding = config.config.pad ? (config.config.kernel_size>>1) : (kernel_t)0;
	output_width = (input_width + 2*padding - config.config.kernel_size + 1)/config.config.stride;
	output_height = (input_height + 2* padding - config.config.kernel_size + 1)/config.config.stride;
	assert(((nk*config.config.kernel_size*config.config.kernel_size)%F)==0);
	config.num_of_output_channel_groups = ceil(1.0*output_channels/nk);
	config.config.vertical_input_feature_chunk_num = ceil(1.0*input_height/MAX_FEATURES_ROW_PER_CHUNK);
	config.config.horizontal_input_feature_chunk_num = ceil(1.0*input_width/MAX_FEATURES_COL_PER_CHUNK);
	config.vertical_output_feature_chunk_num = ceil(1.0*output_height/MAX_FEATURES_ROW_PER_CHUNK);
	config.horizontal_output_feature_chunk_num = ceil(1.0*output_width/MAX_FEATURES_COL_PER_CHUNK);
	config.num_of_kernels_per_group = nk;
}


inline weight_t layer_t::GetRandomWeight(int max_val){
	return rand()%max_val;
}


inline feature_t layer_t::GetRandomFeature(int max_val){
	return rand()%max_val;
}


void layer_t::PrintLayer(){
	cout<<"input channels:"<<config.input_channels<<endl;
	cout<<"input width: "<<input_width<<endl;
	cout<<"input height:"<<input_height<<endl;
	cout<<"output width: "<<output_width<<endl;
	cout<<"output height:"<<output_height<<endl;
	cout<<"output channel:"<<output_channels<<endl;
	cout<<"stride:"<<config.config.stride<<endl;
	cout<<"relu:"<<config.relu<<endl;
	cout<<"padding:"<<config.config.pad<<endl;
	cout<<"next_layer_kernel:"<<config.next_layer_kernel_size<<endl;
	cout<<"kernel:"<<config.config.kernel_size<<endl;
	cout<<endl<<endl;
}


void layer_t::CompressWeights(){
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (output_channel_t j=0;j<config.num_of_output_channel_groups;j++){
			weight_index_t chunk_idx = 0;
			zero_t zero_count = 0;
			for (output_channel_t k=0;k<config.num_of_kernels_per_group;k++){
				for (kernel_t l=0;l<config.config.kernel_size;l++){
					for (kernel_t m=0;m<config.config.kernel_size;m++){
						if (weights[i][j*config.num_of_kernels_per_group+k][l][m]!=0){
							config.compressed_weights[i][j][chunk_idx] = weights[i][j*config.num_of_kernels_per_group+k][l][m];
							cout<<"zero_count="<<zero_count<<" weight="<<config.compressed_weights[i][j][chunk_idx]<<endl;
							config.compressed_weight_index[i][j][chunk_idx] = zero_count;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}else{
							zero_count = zero_count + 1;
							if (zero_count==MAX_ZERO_COUNT){
								config.compressed_weight_index[i][j][chunk_idx] = zero_count;
								cout<<"zero_count="<<zero_count<<" weight="<<0<<endl;
								config.compressed_weights[i][j][chunk_idx] = 0;
								chunk_idx = chunk_idx + 1;
								zero_count = 0;
							}
						}
					}
				}
			}
			config.num_of_none_zero_weights[i][j] = chunk_idx;
			while((config.num_of_none_zero_weights[i][j]%F)!=0){
				config.compressed_weight_index[i][j][chunk_idx] = 0;
				config.compressed_weights[i][j][chunk_idx] = 0;
				config.num_of_none_zero_weights[i][j] ++;
				cout<<"insert zero in weights: input channel "<<i<<" output channel "<<j<<endl;
				chunk_idx = chunk_idx + 1;
			}
		}
	}
}


void layer_t::GenerateRandomWeight(){
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (output_channel_t j=0;j<output_channels;j++){
			for (kernel_t k=0;k<config.config.kernel_size;k++){
				for (kernel_t l=0;l<config.config.kernel_size;l++){
					weights[i][j][k][l] = GetRandomWeight(MAX_WEIGHT_VALUE);
				}
			}
		}
	}
}


bool layer_t::AllocateMemoryForWeight(){
	weights = new weight_t***[config.input_channels];
	if (weights == NULL){
		cout<<"Error: failed to allocate memory(weights)"<<endl;
		return false;
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		weights[i] = new weight_t**[output_channels];
		if (weights[i] == NULL){
			cout<<"Error: failed to allocate memory(weights["<<i<<"])"<<endl;
			return false;
		}else{
			for (output_channel_t j=0;j<output_channels;j++){
				weights[i][j] = new weight_t*[config.config.kernel_size];
				if (weights[i][j] == NULL){
					cout<<"Error: failed to allocate memory(weights["<<i<<"]["<<j<<"])"<<endl;
					return false;
				}else{
					for (kernel_t k=0;k<config.config.kernel_size;k++){
						weights[i][j][k] = new weight_t[config.config.kernel_size];
						if (weights[i][j][k] == NULL){
							cout<<"Error: failed to allocate memory(weights["<<i<<"]["<<j<<"]["<<k<<"])"<<endl;
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}


bool layer_t::AllocateMemoryForInputFeature(){
	input_features = new feature_t**[config.input_channels];
	if (input_features == NULL){
		cout<<"Error: failed to allocate memory(input_features)"<<endl;
		return false;
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		input_features[i] = new feature_t*[input_height];
		if (input_features[i] == NULL){
			cout<<"Error: failed to allocate memory(input_features["<<i<<"])"<<endl;
			return false;
		}else{
			for (dimension_t j=0;j<input_height;j++){
				input_features[i][j] = new feature_t[input_width];
				if (input_features[i][j] == NULL){
					cout<<"Error: failed to allocate memory(input_features["<<i<<"]["<<j<<"])"<<endl;
					return false;
				}
			}
		}
	}

	return true;
}


bool layer_t::AllocateMemoryForOutputFeature(){
	output_features = new feature_t**[output_channels];
	if (output_features == NULL){
		cout<<"Error: failed to allocate memory(output_features)"<<endl;
		return false;
	}

	for (output_channel_t i=0;i<output_channels;i++){
		output_features[i] = new feature_t*[output_height];
		if (output_features[i] == NULL){
			cout<<"Error: failed to allocate memory(output_features["<<i<<"])"<<endl;
			return false;
		}else{
			for (dimension_t j=0;j<output_height;j++){
				output_features[i][j] = new feature_t[output_width];
				if (output_features[i][j] == NULL){
					cout<<"Error: failed to allocate memory(output_features["<<i<<"]["<<j<<"])"<<endl;
					return false;
				}
			}
		}
	}

	return true;
}


void layer_t::DeCompressOutputFeatureMap(){
	for (output_channel_t i=0;i<output_channels;i++){
		for (dimension_t j=0;j<output_height;j++){
			for (dimension_t k=0;k<output_width;k++){
				output_features[i][j][k] = 0;
			}
		}
	}
#ifdef INPUT_HALOS
	for (output_channel_t i=0;i<output_channels;i++){
		for (dimension_t j=0;j<config.vertical_output_feature_chunk_num;j++){
			for (dimension_t k=0;k<config.horizontal_output_feature_chunk_num;k++){
				feature_index_t total_features_processed_in_chunk = 0;
				feature_index_t chunk_id = j*config.horizontal_output_feature_chunk_num+k;
				for (dimension_t l=0;l<config.num_of_none_zero_output_features[i][chunk_id];l++){
					total_features_processed_in_chunk += config.compressed_output_feature_index[i][chunk_id][l]+1;
					dimension_t chunk_row = (total_features_processed_in_chunk-1) / MAX_FEATURES_COL_PER_CHUNK;
					dimension_t chunk_col = (total_features_processed_in_chunk-1) % MAX_FEATURES_COL_PER_CHUNK;
					output_features[i][MAX_FEATURES_ROW_PER_CHUNK*j+chunk_row][MAX_FEATURES_COL_PER_CHUNK*k+chunk_col] = \
						config.compressed_output_features[i][chunk_id][l];
				}
			}
		}
	}
#else
	dimension_t column = (MAX_FEATURES_COL_PER_CHUNK+2*(config.next_layer_kernel_size/2));
	for (output_channel_t i=0;i<output_channels;i++){
		for (dimension_t j=0;j<config.vertical_output_feature_chunk_num;j++){
			for (dimension_t k=0;k<config.horizontal_output_feature_chunk_num;k++){
				feature_index_t total_features_processed_in_chunk = 0;
				feature_index_t chunk_id = j*config.horizontal_output_feature_chunk_num+k;
				for (dimension_t l=0;l<config.num_of_none_zero_output_features[i][chunk_id];l++){
					total_features_processed_in_chunk += config.compressed_output_feature_index[i][chunk_id][l]+1;
					dimension_t chunk_row = (total_features_processed_in_chunk-1) / column - config.next_layer_kernel_size/2;
					dimension_t chunk_col = (total_features_processed_in_chunk-1) % column - config.next_layer_kernel_size/2;
					if (chunk_row<0||chunk_row>=MAX_FEATURES_ROW_PER_CHUNK){
						continue;
					}
					if (chunk_col<0||chunk_col>=MAX_FEATURES_COL_PER_CHUNK){
						continue;
					}
					output_features[i][MAX_FEATURES_ROW_PER_CHUNK*j+chunk_row][MAX_FEATURES_COL_PER_CHUNK*k+chunk_col] = config.compressed_output_features[i][chunk_id][l];
				}
			}
		}
	}
#endif
}


int layer_t::CheckDeCompressedConvolutionResults(){
	feature_t temp = 0;
	int error_count = 0;

	for (output_channel_t i=0;i<output_channels;i++){
		for (dimension_t k=0;k<output_height;k++){
			for (dimension_t l=0;l<output_width;l++){
				temp = 0;
				for (input_channel_t j=0;j<config.input_channels;j++){
					for (kernel_t m=-config.config.kernel_size/2;m<=config.config.kernel_size/2;m++){
						for (kernel_t n=-config.config.kernel_size/2;n<=config.config.kernel_size/2;n++){
							dimension_t row = k+m;
							dimension_t col = l+n;
							if (col<0||col>=output_width)
								continue;
							if (row<0||row>=output_height)
								continue;
							temp += weights[j][i][config.config.kernel_size/2+m][config.config.kernel_size/2+n]*input_features[j][row][col];
						}
					}
				}
				if (temp != output_features[i][k][l]){
					cout<<"["<<i<<"]["<<k<<"]["<<l<<"]: expect "<<temp<<" got ";
					cout<<output_features[i][k][l]<<endl;
					error_count ++;
				}else{
					//cout<<"["<<i<<"]["<<k<<"]["<<l<<"]: "<<temp<<endl;
				}
			}
		}
	}

	return error_count;
}


void layer_t::CompressInputFeatureMap(){
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (dimension_t j=0;j<config.config.vertical_input_feature_chunk_num;j++){
			for (dimension_t k=0;k<config.config.horizontal_input_feature_chunk_num;k++){
				zero_t zero_count = 0;
				feature_index_t chunk_idx = 0;
				feature_index_t chunk_id = j*config.config.horizontal_input_feature_chunk_num+k;
#ifdef INPUT_HALOS
				for (dimension_t l=0;l<MAX_FEATURES_ROW_PER_CHUNK;l++){
					for (dimension_t m=0;m<MAX_FEATURES_COL_PER_CHUNK;m++){
						dimension_t absolute_row = j*MAX_FEATURES_ROW_PER_CHUNK+l;
						dimension_t absolute_col = k*MAX_FEATURES_COL_PER_CHUNK+m;
						if (absolute_row>=input_height || absolute_col>input_width) continue;
						feature_t feature = input_features[i][absolute_row][absolute_col];
#else
				kernel_t ksize = config.config.kernel_size;
				for (dimension_t l=-ksize/2;l<MAX_FEATURES_ROW_PER_CHUNK+ksize/2;l++){
					for (dimension_t m=-ksize/2;m<MAX_FEATURES_COL_PER_CHUNK+ksize/2;m++){
						feature_t feature = 0;
						dimension_t absolute_row = j*MAX_FEATURES_ROW_PER_CHUNK+l;
						dimension_t absolute_col = k*MAX_FEATURES_COL_PER_CHUNK+m;
						if (absolute_row<0 || absolute_row>=input_height) feature = 0;
						else if (absolute_col<0 || absolute_col>=input_width) feature = 0;
						else feature = input_features[i][absolute_row][absolute_col];
#endif
						if (feature!=0){
							config.compressed_input_feature_index[i][chunk_id][chunk_idx] = zero_count;
							config.compressed_input_features[i][chunk_id][chunk_idx] = feature;
							//cout<<"zero_count="<<zero_count<<" :feature="<<feature<<endl;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}else{
							zero_count = zero_count + 1;
							if (zero_count == MAX_ZERO_COUNT){
								config.compressed_input_feature_index[i][chunk_id][chunk_idx] = zero_count;
								config.compressed_input_features[i][chunk_id][chunk_idx] = 0;
								//cout<<"zero_count="<<zero_count<<" :feature="<<0<<endl;
								chunk_idx = chunk_idx + 1;
								zero_count = 0;
							}
						}
					}
				}
				config.num_of_none_zero_input_features[i][chunk_id] = chunk_idx;
				while((config.num_of_none_zero_input_features[i][chunk_id]%I)!=0){
					config.compressed_input_feature_index[i][chunk_id][chunk_idx] = 0;
					config.compressed_input_features[i][chunk_id][chunk_idx] = 0;
					config.num_of_none_zero_input_features[i][chunk_id]++;
					cout<<"insert zero in feature: input channel "<<i<<" block["<<j<<"]["<<k<<"]"<<endl;
					chunk_idx = chunk_idx + 1;
				}
			}
		}

		for (dimension_t j=0;j<config.config.vertical_input_feature_chunk_num*config.config.horizontal_input_feature_chunk_num;j++){
			if (config.num_of_none_zero_input_features[i][j]>config.max_num_of_none_zero_input_features[i]){
				config.max_num_of_none_zero_input_features[i] = config.num_of_none_zero_input_features[i][j];
			}
		}
	}
}


void layer_t::GenerateRandomFeatureMap(){
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (dimension_t j=0;j<input_height;j++){
			for (dimension_t k=0;k<input_width;k++){
				input_features[i][j][k] = GetRandomFeature(MAX_FEATURE_VALUE);
			}
		}
	}
}


void layer_t::DumpGeneratedWeight(const char* filename){
	ofstream output(filename);
	if(!output){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		for (output_channel_t j=0;j<output_channels;j++){
			for (kernel_t k=0;k<config.config.kernel_size;k++){
				for (kernel_t l=0;l<config.config.kernel_size;l++){
					output<<weights[i][j][k][l]<<" ";
				}
			}
			output<<endl;
		}
	}

	output.close();
}


void layer_t::LoadGeneratedWeight(const char* filename){
	ifstream input(filename);
	if(!input){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	cout<<"weight:"<<endl;
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (output_channel_t j=0;j<output_channels;j++){
			for (kernel_t k=0;k<config.config.kernel_size;k++){
				for (kernel_t l=0;l<config.config.kernel_size;l++){
					input>>weights[i][j][k][l];
					cout<<weights[i][j][k][l]<<" ";
				}
				cout<<endl;
			}
			cout<<endl;
		}
		cout<<endl;
	}
	cout<<endl;

	input.close();
}


void layer_t::LoadGeneratedFeatureMap(const char* filename){
	ifstream input(filename);
	if (!input){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	cout<<"input feature:"<<endl;
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (dimension_t j=0;j<input_height;j++){
			for (dimension_t k=0;k<input_width;k++){
				input>>input_features[i][j][k];
				cout<<input_features[i][j][k]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;
	}
	cout<<endl;

	input.close();
}


void layer_t::DumpGeneratedFeatureMap(const char* filename){
	ofstream output(filename);
	if (!output){
		cout<<"failed to open "<<filename<<endl;
		return;
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		for (dimension_t j=0;j<input_height;j++){
			for (dimension_t k=0;k<input_width;k++){
				output<<input_features[i][j][k]<<" ";
			}
		}
		output<<endl;
	}

	output.close();
}


bool layer_t::AllocateMemoryForCompressedInputFeature(){
	config.compressed_input_features = new feature_t**[config.input_channels];
	if (config.compressed_input_features == NULL){
		cout<<"failed to allocate memory for compressed_input_feature"<<endl;
		return false;
	}

	config.compressed_input_feature_index = new zero_t**[config.input_channels];
	if (config.compressed_input_feature_index == NULL){
		cout<<"failed to allocate memory for compressed_input_feature_index"<<endl;
		return false;
	}

	config.num_of_none_zero_input_features = new feature_index_t*[config.input_channels];
	if (config.num_of_none_zero_input_features == NULL){
		cout<<"failed to allocate memory for num_of_none_zero_input_features"<<endl;
		return false;
	}

	config.max_num_of_none_zero_input_features = new feature_index_t[config.input_channels];
	if (config.max_num_of_none_zero_input_features == NULL){
		cout<<"failed to allocate memory for max_num_of_none_zero_input_features"<<endl;
		return false;
	}

	dimension_t total_chunk_num = config.config.vertical_input_feature_chunk_num * config.config.horizontal_input_feature_chunk_num;
#ifndef INPUT_HALOS
	dimension_t total_input_features_row_per_chunk = ceil(1.0*input_height/config.config.vertical_input_feature_chunk_num) + 2*(config.config.kernel_size/2);
	dimension_t total_input_features_col_per_chunk = ceil(1.0*input_width/config.config.horizontal_input_feature_chunk_num) + 2*(config.config.kernel_size/2);
#else
	dimension_t total_input_features_row_per_chunk = ceil(1.0*input_height/config.config.vertical_input_feature_chunk_num);
	dimension_t total_input_features_col_per_chunk = ceil(1.0*input_width/config.config.horizontal_input_feature_chunk_num);
#endif
	dimension_t total_input_features_per_chunk = total_input_features_row_per_chunk* total_input_features_col_per_chunk;

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.compressed_input_features[i] = new feature_t*[total_chunk_num];
		if (config.compressed_input_features[i] == NULL){
			cout<<"failed to allocate memory for compressed_input_features["<<i<<"]"<<endl;
			return false;
		}
		for (dimension_t j=0;j<total_chunk_num;j++){
			config.compressed_input_features[i][j] = new feature_t[total_input_features_per_chunk];
			if (config.compressed_input_features[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_input_features["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.compressed_input_feature_index[i] = new zero_t*[total_chunk_num];
		if (config.compressed_input_feature_index[i] == NULL){
			cout<<"failed to allocate memory for compressed_input_feature_index["<<i<<"]"<<endl;
			return false;
		}
		for (dimension_t j=0;j<total_chunk_num;j++){
			config.compressed_input_feature_index[i][j] = new zero_t[total_input_features_per_chunk];
			if (config.compressed_input_feature_index[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_input_feature_index["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.num_of_none_zero_input_features[i] = new feature_index_t[total_chunk_num];
		if (config.num_of_none_zero_input_features[i] == NULL){
			cout<<"failed to allocate memory for num_of_none_zero_input_features["<<i<<"]"<<endl;
			return false;
		}
	}

	return true;
}


bool layer_t::AllocateMemoryForCompressedOutputFeature(){
	config.compressed_output_features = new feature_t**[output_channels];
	if (config.compressed_output_features == NULL){
		cout<<"failed to allocate memory for compressed_output_features"<<endl;
		return false;
	}

	config.compressed_output_feature_index = new zero_t**[output_channels];
	if (config.compressed_output_feature_index == NULL){
		cout<<"failed to allocate memory for compressed_output_feature_index"<<endl;
		return false;
	}

	config.num_of_none_zero_output_features = new feature_index_t*[output_channels];
	if (config.num_of_none_zero_output_features == NULL){
		cout<<"failed to allocate memory for num_of_none_zero_output_features"<<endl;
		return false;
	}

	dimension_t total_chunk_num = config.vertical_output_feature_chunk_num * config.horizontal_output_feature_chunk_num;
#ifndef INPUT_HALOS
	dimension_t horizontal_output_features_per_chunk = ceil(1.0*output_width/config.horizontal_output_feature_chunk_num) + 2*(config.next_layer_kernel_size/2);
	dimension_t vertical_output_features_per_chunk = ceil(1.0*output_height/config.vertical_output_feature_chunk_num) + 2*(config.next_layer_kernel_size/2);
#else
	dimension_t horizontal_output_features_per_chunk = ceil(1.0*output_width/config.horizontal_output_feature_chunk_num);
	dimension_t vertical_output_features_per_chunk = ceil(1.0*output_height/config.vertical_output_feature_chunk_num);
#endif
	dimension_t total_features_per_chunk = horizontal_output_features_per_chunk*vertical_output_features_per_chunk;

	for (output_channel_t i=0;i<output_channels;i++){
		config.compressed_output_features[i] = new feature_t*[total_chunk_num];
		if (config.compressed_output_features[i] == NULL){
			cout<<"failed to allocate memory for compressed_output_features["<<i<<"]"<<endl;
			return false;
		}
		for (dimension_t j=0;j<total_chunk_num;j++){
			config.compressed_output_features[i][j] = new feature_t[total_features_per_chunk];
			if (config.compressed_output_features[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_output_features["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (output_channel_t i=0;i<output_channels;i++){
		config.compressed_output_feature_index[i] = new zero_t*[total_chunk_num];
		if (config.compressed_output_feature_index[i] == NULL){
			cout<<"failed to allocate memory for compressed_output_feature_index["<<i<<"]"<<endl;
			return false;
		}
		for (dimension_t j=0;j<total_chunk_num;j++){
			config.compressed_output_feature_index[i][j] = new zero_t[total_features_per_chunk];
			if (config.compressed_output_feature_index[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_output_feature_index["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (output_channel_t i=0;i<output_channels;i++){
		config.num_of_none_zero_output_features[i] = new feature_index_t[total_chunk_num];
		if (config.num_of_none_zero_output_features[i] == NULL){
			cout<<"failed to allocate memory for num_of_none_zero_output_features["<<i<<"]"<<endl;
			return false;
		}
	}

	return true;
}


bool layer_t::AllocateMemoryForCompressedWeight(){
	config.compressed_weights = new weight_t**[config.input_channels];
	if (config.compressed_weights == NULL){
		cout<<"failed to allocate memory for compressed_weights"<<endl;
		return false;
	}

	config.compressed_weight_index = new zero_t**[config.input_channels];
	if (config.compressed_weight_index == NULL){
		cout<<"failed to allocate memory for compressed_weight_index"<<endl;
		return false;
	}

	config.num_of_none_zero_weights = new weight_index_t*[config.input_channels];
	if (config.num_of_none_zero_weights == NULL){
		cout<<"failed to allocate memory for num_of_none_zero_weights"<<endl;
		return false;
	}

	weight_index_t num_of_weights_per_kernel = config.config.kernel_size*config.config.kernel_size;
	weight_index_t max_num_of_weights_per_group = config.num_of_kernels_per_group*num_of_weights_per_kernel;

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.compressed_weights[i] = new weight_t*[config.num_of_output_channel_groups];
		if (config.compressed_weights[i] == NULL){
			cout<<"failed to allocate memory for compressed_weights["<<i<<"]"<<endl;
			return false;
		}
		for (output_channel_t j=0;j<config.num_of_output_channel_groups;j++){
			config.compressed_weights[i][j] = new weight_t[max_num_of_weights_per_group];
			if (config.compressed_weights[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_weights["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.compressed_weight_index[i] = new zero_t*[config.num_of_output_channel_groups];
		if (config.compressed_weight_index[i] == NULL){
			cout<<"failed to allocate memory for compressed_weight_index["<<i<<"]"<<endl;
			return false;
		}
		for (output_channel_t j=0;j<config.num_of_output_channel_groups;j++){
			config.compressed_weight_index[i][j] = new zero_t[max_num_of_weights_per_group];
			if (config.compressed_weight_index[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_weight_index["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.num_of_none_zero_weights[i] = new weight_index_t[config.num_of_output_channel_groups];
		if (config.num_of_none_zero_weights[i] == NULL){
			cout<<"failed to allocate memory for num_of_none_zero_weights["<<i<<"]"<<endl;
			return false;
		}
	}

	return true;
}
