#include"layer.hpp"


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
	cout<<"kernel:"<<config.config.kernel_size<<"x"<<config.config.kernel_size<<endl;
	cout<<endl<<endl;
}


void layer_t::CompressWeights(){
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (output_channel_t j=0;j<config.num_of_output_channel_groups;j++){
			index_t chunk_idx = 0;
			zero_t zero_count = 0;
			for (output_channel_t k=0;k<MAX_OUTPUT_CHANNEL_GROUP_SIZE;k++){
				for (kernel_t l=0;l<config.config.kernel_size;l++){
					for (kernel_t m=0;m<config.config.kernel_size;m++){
						if (weights[i][j*MAX_OUTPUT_CHANNEL_GROUP_SIZE+k][l][m]!=0){
							config.compressed_weights[i][j][chunk_idx] = weights[i][j*MAX_OUTPUT_CHANNEL_GROUP_SIZE+k][l][m];
							config.compressed_weight_index[i][j][chunk_idx] = zero_count;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}else{
							zero_count = zero_count + 1;
							if (zero_count==MAX_ZERO_COUNT){
								config.compressed_weight_index[i][j][chunk_idx] = zero_count;
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

	for (output_channel_t i=0;i<output_channels;i++){
		for (dimension_t j=0;j<vertical_output_feature_chunk_num;j++){
			for (dimension_t k=0;k<horizontal_output_feature_chunk_num;k++){
				index_t total_features_processed_in_chunk = 0;
				index_t chunk_id = j*horizontal_output_feature_chunk_num+k;
				for (dimension_t l=0;l<config.num_of_none_zero_output_features[i][chunk_id];l++){
					total_features_processed_in_chunk += config.compressed_output_feature_index[i][chunk_id][l]+1;
					dimension_t chunk_row = (total_features_processed_in_chunk-1) / MAX_FEATURES_COL_PER_CHUNK;
					dimension_t chunk_col = (total_features_processed_in_chunk-1) % MAX_FEATURES_COL_PER_CHUNK;
					output_features[i][MAX_FEATURES_ROW_PER_CHUNK*j+chunk_row]\
						[MAX_FEATURES_COL_PER_CHUNK*k+chunk_col] = config.compressed_output_features[i][chunk_id][l];
				}
			}
		}
	}
}


int layer_t::CheckDeCompressedConvolutionResults(){
	feature_t temp = 0;
	int error_count = 0;

	for (output_channel_t i=0;i<output_channels;i++){
		for (dimension_t k=0;k<output_height;k++){
			for (dimension_t l=0;l<output_width;l++){
				temp = 0;
				for (input_channel_t j=0;j<config.input_channels;j++){
					for (int m=-config.config.kernel_size/2;m<=config.config.kernel_size/2;m++){
						for (int n=-config.config.kernel_size/2;n<=config.config.kernel_size/2;n++){
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
				index_t chunk_idx = 0;
				zero_t zero_count = 0;
				index_t chunk_id = j*config.config.horizontal_input_feature_chunk_num+k;
				for (dimension_t l=0;l<MAX_FEATURES_ROW_PER_CHUNK;l++){
					for (dimension_t m=0;m<MAX_FEATURES_COL_PER_CHUNK;m++){
						dimension_t absolute_row = j*MAX_FEATURES_ROW_PER_CHUNK+l;
						dimension_t absolute_col = k*MAX_FEATURES_COL_PER_CHUNK+m;
						feature_t feature = input_features[i][absolute_row][absolute_col];
						if (feature!=0){
							config.compressed_input_feature_index[i][chunk_id][chunk_idx] = zero_count;
							config.compressed_input_features[i][chunk_id][chunk_idx] = feature;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}else{
							zero_count = zero_count + 1;
							if (zero_count == MAX_ZERO_COUNT){
								config.compressed_input_feature_index[i][chunk_id][chunk_idx] = zero_count;
								config.compressed_input_features[i][chunk_id][chunk_idx] = 0;
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
	if(output==NULL){
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
	if(input==NULL){
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
	if (input==NULL){
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
	if (output==NULL){
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

	config.num_of_none_zero_input_features = new index_t*[config.input_channels];
	if (config.num_of_none_zero_input_features == NULL){
		cout<<"failed to allocate memory for num_of_none_zero_input_features"<<endl;
		return false;
	}

	config.max_num_of_none_zero_input_features = new index_t[config.input_channels];
	if (config.max_num_of_none_zero_input_features == NULL){
		cout<<"failed to allocate memory for max_num_of_none_zero_input_features"<<endl;
		return false;
	}

	dimension_t total_chunk_num = config.config.vertical_input_feature_chunk_num * config.config.horizontal_input_feature_chunk_num;

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.compressed_input_features[i] = new feature_t*[total_chunk_num];
		if (config.compressed_input_features[i] == NULL){
			cout<<"failed to allocate memory for compressed_input_features["<<i<<"]"<<endl;
			return false;
		}
		for (index_t j=0;j<total_chunk_num;j++){
			config.compressed_input_features[i][j] = new feature_t[input_width*input_height];
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
		for (index_t j=0;j<total_chunk_num;j++){
			config.compressed_input_feature_index[i][j] = new zero_t[input_width*input_height];
			if (config.compressed_input_feature_index[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_input_feature_index["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.num_of_none_zero_input_features[i] = new index_t[total_chunk_num];
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

	config.num_of_none_zero_output_features = new index_t*[output_channels];
	if (config.num_of_none_zero_output_features == NULL){
		cout<<"failed to allocate memory for num_of_none_zero_output_features"<<endl;
		return false;
	}

	dimension_t total_chunk_num = vertical_output_feature_chunk_num * horizontal_output_feature_chunk_num;

	for (output_channel_t i=0;i<output_channels;i++){
		config.compressed_output_features[i] = new feature_t*[total_chunk_num];
		if (config.compressed_output_features[i] == NULL){
			cout<<"failed to allocate memory for compressed_output_features["<<i<<"]"<<endl;
			return false;
		}
		for (index_t j=0;j<total_chunk_num;j++){
			config.compressed_output_features[i][j] = new feature_t[output_width*output_height];
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
		for (index_t j=0;j<total_chunk_num;j++){
			config.compressed_output_feature_index[i][j] = new zero_t[output_width*output_height];
			if (config.compressed_output_feature_index[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_output_feature_index["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (output_channel_t i=0;i<output_channels;i++){
		config.num_of_none_zero_output_features[i] = new index_t[total_chunk_num];
		if (config.num_of_none_zero_output_features[i] == NULL){
			cout<<"failed to allocate memory for num_of_none_zero_output_features["<<i<<"]"<<endl;
			return false;
		}
	}

	return true;
}


bool layer_t::AllocateMemoryForCompressedWeight(){
	config.compressed_weights = new feature_t**[config.input_channels];
	if (config.compressed_weights == NULL){
		cout<<"failed to allocate memory for compressed_weights"<<endl;
		return false;
	}

	config.compressed_weight_index = new zero_t**[config.input_channels];
	if (config.compressed_weight_index == NULL){
		cout<<"failed to allocate memory for compressed_weight_index"<<endl;
		return false;
	}

	config.num_of_none_zero_weights = new index_t*[config.input_channels];
	if (config.num_of_none_zero_weights == NULL){
		cout<<"failed to allocate memory for num_of_none_zero_weights"<<endl;
		return false;
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.compressed_weights[i] = new feature_t*[config.num_of_output_channel_groups];
		if (config.compressed_weights[i] == NULL){
			cout<<"failed to allocate memory for compressed_weights["<<i<<"]"<<endl;
			return false;
		}
		for (output_channel_t j=0;j<config.num_of_output_channel_groups;j++){
			config.compressed_weights[i][j] = new feature_t[config.num_of_kernels_per_group];
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
			config.compressed_weight_index[i][j] = new zero_t[config.num_of_kernels_per_group];
			if (config.compressed_weight_index[i][j] == NULL){
				cout<<"failed to allocate memory for compressed_weight_index["<<i<<"]["<<j<<"]"<<endl;
				return false;
			}
		}
	}

	for (input_channel_t i=0;i<config.input_channels;i++){
		config.num_of_none_zero_weights[i] = new index_t[config.num_of_output_channel_groups];
		if (config.num_of_none_zero_weights[i] == NULL){
			cout<<"failed to allocate memory for num_of_none_zero_weights["<<i<<"]"<<endl;
			return false;
		}
	}

	return true;
}
