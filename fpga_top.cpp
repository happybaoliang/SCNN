#include"common.hpp"
#include"fpga_top.hpp"
#include"process_element.hpp"
#include"layer.hpp"


struct ProcessElement PE[NUM_OF_PEs];

static index_t max_num_of_none_zero_features[MAX_INPUT_CHANNEL_NUM]={0};

hls::stream<Flit> west_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM][MAX_HORIZONTAL_FEATURE_CHUNK_NUM-1];
hls::stream<Flit> east_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM][MAX_HORIZONTAL_FEATURE_CHUNK_NUM-1];

hls::stream<Flit> south_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM-1][MAX_HORIZONTAL_FEATURE_CHUNK_NUM];
hls::stream<Flit> north_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM-1][MAX_HORIZONTAL_FEATURE_CHUNK_NUM];

hls::stream<Flit> south_west_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM-1][MAX_HORIZONTAL_FEATURE_CHUNK_NUM-1];
hls::stream<Flit> south_east_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM-1][MAX_HORIZONTAL_FEATURE_CHUNK_NUM-1];

hls::stream<Flit> north_west_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM-1][MAX_HORIZONTAL_FEATURE_CHUNK_NUM-1];
hls::stream<Flit> north_east_halos_channel[MAX_VERTICAL_FEATURE_CHUNK_NUM-1][MAX_HORIZONTAL_FEATURE_CHUNK_NUM-1];

static index_t num_of_weights_per_chunk[MAX_INPUT_CHANNEL_NUM][MAX_OUTPUT_CHANNEL_CHUNK_NUM];
static zero_t zeros[MAX_INPUT_CHANNEL_NUM][MAX_OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];
static weight_t weight[MAX_INPUT_CHANNEL_NUM][MAX_OUTPUT_CHANNEL_CHUNK_NUM][MAX_NUM_OF_WEIGHTS_PER_CHUNK];

extern index_t num_of_none_zero_output_features[MAX_OUTPUT_CHANNEL_NUM][MAX_FEATURE_CHUNK_NUM];
extern feature_t compressed_output_feature[MAX_OUTPUT_CHANNEL_NUM][MAX_FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];
extern zero_t compressed_output_feature_index[MAX_OUTPUT_CHANNEL_NUM][MAX_FEATURE_CHUNK_NUM][MAX_NUM_OF_FEATURE_PER_CHUNK];


static inline void LoadFeatureMapForPEs(struct fpga_config config){
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (pe_t j=0;j<NUM_OF_PEs;j++){
			PE[j].num_of_none_zero_features[i]=config.num_of_none_zero_input_features[i][j];
			memcpy(PE[j].featuremap[i],config.compressed_input_features[i][j],sizeof(feature_t)*config.num_of_none_zero_input_features[i][j]);
			memcpy(PE[j].featureindex[i],config.compressed_input_feature_index[i][j],sizeof(zero_t)*config.num_of_none_zero_input_features[i][j]);
		}
	}
}


static inline void LoadCompressedWeights(struct fpga_config config){
	for (input_channel_t i=0;i<config.input_channels;i++){
		for (output_channel_t j=0;j<config.num_of_output_channel_groups;j++){
			num_of_weights_per_chunk[i][j] = config.num_of_none_zero_weights[i][j];
			memcpy(weight[i][j],config.compressed_weights[i][j],sizeof(weight_t)*config.num_of_none_zero_weights[i][j]);
			memcpy(zeros[i][j],config.compressed_weight_index[i][j],sizeof(zero_t)*config.num_of_none_zero_weights[i][j]);
		}
	}
}


static inline void BroadcastWeights(weight_t weights[F], zero_t index[F]){
	//cout<<"broadcast F weights"<<endl;
	for (pe_t i=0;i<NUM_OF_PEs;i++){
		memcpy(PE[i].weight,weights,F*sizeof(weight_t));
		memcpy(PE[i].weightindex,index,F*sizeof(zero_t));
	}
}


static inline void AccumulateProduct(){
	for (int i=0;i<NUM_OF_PEs;i++){
		PE[i].AccumulateProduct();
	}
}


static inline void SetNextInputChannel(input_channel_t channel){
	//cout<<"next channel:"<<channel<<endl;
	for (pe_t i=0;i<NUM_OF_PEs;i++){
		PE[i].SetNextInputChannel(channel);
	}
}


static void DrainOutProducts(){
	for (pe_t i=0;i<NUM_OF_PEs;i++){
		PE[i].DrainOutProducts();
	}
}


static void CollectAndCompressResults(struct fpga_config& config, index_t chunk){
	DrainOutProducts();

	for(pe_t i=0;i<NUM_OF_PEs;i++){
		for(output_channel_t j=0;j<config.num_of_kernels_per_group;j++){
			index_t chunk_idx = 0;
			index_t zero_count = 0;
			output_channel_t out = chunk*config.num_of_kernels_per_group+j;
			for (int k=0;k<MAX_FEATURES_ROW_PER_CHUNK;k++){
				for (int l=0;l<MAX_FEATURES_COL_PER_CHUNK;l++){
					product_t product = PE[i].acc.get_and_clear(j,k,l);
					//cout<<"["<<j<<"]["<<k<<"]["<<l<<"]:"<<product<<endl;
					if (product){
						config.compressed_output_feature_index[out][i][chunk_idx] = zero_count;
						config.compressed_output_features[out][i][chunk_idx]=product;
						chunk_idx = chunk_idx + 1;
						zero_count = 0;
					}else{
						zero_count = zero_count + 1;
						if (zero_count==MAX_ZERO_COUNT){
							config.compressed_output_feature_index[out][i][chunk_idx] = zero_count;
							config.compressed_output_features[out][i][chunk_idx] = 0;
							chunk_idx = chunk_idx + 1;
							zero_count = 0;
						}
					}
				}
			}
			config.num_of_none_zero_output_features[out][i] = chunk_idx;
		}
	}
}


static void inline FetchNextIFeatureMap(){
	//cout<<"fetch next I featuremap"<<endl;
	for (pe_t i=0;i<NUM_OF_PEs;i++){
		PE[i].FetchNextIFeatureMap();
	}
}


static void inline ConnectAllProcessElement(struct pe_config& config){
	for (pe_t i=0;i<config.vertical_input_feature_chunk_num;i++){
		for (pe_t j=0;j<config.horizontal_input_feature_chunk_num;j++){
			if (j<(config.horizontal_input_feature_chunk_num-1)){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[EAST_PORT] = &east_halos_channel[i][j];
				PE[i*config.horizontal_input_feature_chunk_num+j+1].output_halos[WEST_PORT] = &east_halos_channel[i][j];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[EAST_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[EAST_PORT] = NULL;
			}
			if (j>0){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[WEST_PORT] = &west_halos_channel[i][j-1];
				PE[i*config.horizontal_input_feature_chunk_num+j-1].output_halos[EAST_PORT] = &west_halos_channel[i][j-1];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[WEST_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[WEST_PORT] = NULL;
			}
			if (i>0){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[NORTH_PORT] = &north_halos_channel[i-1][j];
				PE[(i-1)*config.horizontal_input_feature_chunk_num+j].output_halos[SOUTH_PORT] = &north_halos_channel[i-1][j];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[NORTH_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[NORTH_PORT] = NULL;
			}
			if (i<(config.vertical_input_feature_chunk_num-1)){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[SOUTH_PORT] = &south_halos_channel[i][j];
				PE[(i+1)*config.horizontal_input_feature_chunk_num+j].output_halos[NORTH_PORT] = &south_halos_channel[i][j];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[SOUTH_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[SOUTH_PORT] = NULL;
			}
		}
	}

	for (pe_t i=0;i<config.vertical_input_feature_chunk_num;i++){
		for (pe_t j=0;j<config.horizontal_input_feature_chunk_num;j++){
			if ((i<(config.vertical_input_feature_chunk_num-1)) && (j<(config.horizontal_input_feature_chunk_num-1))){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[SOUTH_EAST_PORT] = &south_east_halos_channel[i][j];
				PE[(i+1)*config.horizontal_input_feature_chunk_num+j+1].output_halos[NORTH_WEST_PORT] = &south_east_halos_channel[i][j];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[SOUTH_EAST_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[SOUTH_EAST_PORT] = NULL;
			}
			if ((i>0) && (j>0)){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[NORTH_WEST_PORT] = &north_west_halos_channel[i-1][j-1];
				PE[(i-1)*config.horizontal_input_feature_chunk_num+j-1].output_halos[SOUTH_EAST_PORT] = &north_west_halos_channel[i-1][j-1];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[NORTH_WEST_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[NORTH_WEST_PORT] = NULL;
			}
			if ((i>0) && (j<(config.horizontal_input_feature_chunk_num-1))){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[NORTH_EAST_PORT] = &north_east_halos_channel[i-1][j];
				PE[(i-1)*config.horizontal_input_feature_chunk_num+j+1].output_halos[SOUTH_WEST_PORT] = &north_east_halos_channel[i-1][j];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[NORTH_EAST_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[NORTH_EAST_PORT] = NULL;
			}
			if ((i<(config.vertical_input_feature_chunk_num-1)) && (j>0)){
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[SOUTH_WEST_PORT] = &south_west_halos_channel[i][j-1];
				PE[(i+1)*config.horizontal_input_feature_chunk_num+j-1].output_halos[NORTH_EAST_PORT] = &south_west_halos_channel[i][j-1];
			}else{
				PE[i*config.horizontal_input_feature_chunk_num+j].input_halos[SOUTH_WEST_PORT] = NULL;
				PE[i*config.horizontal_input_feature_chunk_num+j].output_halos[SOUTH_WEST_PORT] = NULL;
			}
		}
	}
}


static void inline ResetAllProcessElement(pe_config& config){
	for (pe_t i=0;i<config.vertical_input_feature_chunk_num;i++){
		for (pe_t j=0;j<config.horizontal_input_feature_chunk_num;j++){
			PE[i*config.horizontal_input_feature_chunk_num+j].ResetProcessElement(i,j,config);
		}
	}
}


int Accelerator(struct fpga_config& config){

	zero_t index_buf[F];
	weight_t weight_buf[F];

	ConnectAllProcessElement(config.config);

	ResetAllProcessElement(config.config);

	assert(config.config.kernel_size < MAX_KERNEL_SIZE);

	memcpy(max_num_of_none_zero_features,config.max_num_of_none_zero_input_features,config.input_channels*sizeof(index_t));

	LoadCompressedWeights(config);

	LoadFeatureMapForPEs(config);

	for (index_t i=0;i<config.num_of_output_channel_groups;i++){
		for (input_channel_t j=0;j<config.input_channels;j++){
			SetNextInputChannel(j);
			for(index_t k=0;k<max_num_of_none_zero_features[j];k+=I){
				FetchNextIFeatureMap();
				for (index_t l=0;l<num_of_weights_per_chunk[j][i];l+=F){
					memcpy(weight_buf,weight[j][i]+l,sizeof(weight_t)*F);
					memcpy(index_buf,zeros[j][i]+l,sizeof(zero_t)*F);
					BroadcastWeights(weight_buf,index_buf);
					AccumulateProduct();
				}
			}
		}
		CollectAndCompressResults(config,i);
	}

	return 0;
}
