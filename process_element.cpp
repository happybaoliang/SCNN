#include<iostream>
#include<cassert>
#include<cstring>
#include<cmath>

#include"process_element.hpp"

using namespace std;


extern struct ProcessElement PE[NUM_OF_PEs];


inline offset_type ProcessElement::GetOffsetInMatrix(){
	return (total_weights+num_of_processed_weights-1) % num_of_weights_per_kernel;
}


void ProcessElement::SetNextInputChannel(channel_type channel){
	stall = false;
	total_weights = 0;
	total_features = 0;
	num_of_processed_weights=0;
	num_of_processed_features = 0;
	current_input_channel = channel;
	num_of_none_zero_feature_fetched = 0;
}


inline ocoord_type ProcessElement::GetOCoord(){
	ocoord_type ocoord = (total_weights+num_of_processed_weights-1)/num_of_weights_per_kernel;
	//cout<<"ocoord=("<<total_weights<<"+"<<num_of_processed_weights<<"-1)"<<"/"<<num_of_weights_per_kernel<<"="<<ocoord<<endl;
	return ocoord;
}


inline col_coord_type ProcessElement::GetColCoord(){
	offset_type wcol = GetOffsetInMatrix()%KERNEL_SIZE-KERNEL_SIZE/2;
	col_coord_type xcoord = (total_features+num_of_processed_features-1)%FEATURES_COL_PER_CHUNK - wcol;
	//if (!(xcoord>=-KERNEL_SIZE/2 && xcoord<FEATURES_COL_PER_CHUNK+KERNEL_SIZE/2))
	//	cout<<"PE"<<(HORIZONTAL_FEATURE_CHUNK_NUM*row+col)<<": wcol="<<wcol<<" xcoord=("<<total_features<<"+"<<num_of_processed_features<<"-1)/"<<FEATURES_ROW_PER_CHUNK<<"-("<<wcol<<")="<<xcoord<<endl;
	assert(xcoord>=-KERNEL_SIZE/2 && xcoord<FEATURES_ROW_PER_CHUNK+KERNEL_SIZE/2);
	return xcoord;
}


inline row_coord_type ProcessElement::GetRowCoord(){
	offset_type wrow = GetOffsetInMatrix()/KERNEL_SIZE-KERNEL_SIZE/2;
	row_coord_type ycoord = (total_features+num_of_processed_features-1)/FEATURES_COL_PER_CHUNK-wrow;
	//if (!(ycoord>=-KERNEL_SIZE/2 && ycoord<FEATURES_COL_PER_CHUNK+KERNEL_SIZE/2))
	//	cout<<"PE"<<(HORIZONTAL_FEATURE_CHUNK_NUM*row+col)<<": wrow="<<wrow<<" ycoord=("<<total_features<<"+"<<num_of_processed_features<<"-1)/"<<FEATURES_COL_PER_CHUNK<<"-("<<wrow<<")="<<ycoord<<endl;
	assert(ycoord>=-KERNEL_SIZE/2 && ycoord<FEATURES_COL_PER_CHUNK+KERNEL_SIZE/2);
	return ycoord;
}


void ProcessElement::FetchNextIFeatureMap(){
	if (num_of_none_zero_feature_fetched<num_of_none_zero_features[current_input_channel]){
		memcpy(feature_buf,featuremap[current_input_channel]+num_of_none_zero_feature_fetched,I*sizeof(feature_type));
		memcpy(feature_index_buf,featureindex[current_input_channel]+num_of_none_zero_feature_fetched,I*sizeof(zeros_type));
		num_of_none_zero_feature_fetched = num_of_none_zero_feature_fetched + I;
		total_features += num_of_processed_features;
		num_of_processed_features = 0;
		num_of_processed_weights = 0;
		total_weights = 0;
		stall = false;
	}else{
		stall = true;
		//cout<<"PE"<<(HORIZONTAL_FEATURE_CHUNK_NUM*row+col)<<" stalled: total features:"<<num_of_none_zero_feature_fetched<<endl;
	}
}


void ProcessElement::AccumulateProduct(){
	if (stall) return;
	num_of_processed_features = 0;
	for (int i=0;i<I;i++){
		num_of_processed_weights = 0;
		num_of_processed_features+=feature_index_buf[i]+1;
		for (int j=0;j<F;j++){
			num_of_processed_weights+=weightindex[j]+1;
			if (weight[j]==0 || feature_buf[i]==0){
				continue;
			}
			col_coord_type col_coord = GetColCoord();
			row_coord_type row_coord = GetRowCoord();
			ocoord_type ocoord = GetOCoord();
			product_type product=weight[j]*feature_buf[i];
			assert(ocoord>=0 && ocoord<OUTPUT_CHANNEL_NUM);
			pe_coord_type col_id = col;
			pe_coord_type row_id = row;
			if (col_coord<0){
				col_id = col_id -1;
			}else if (col_coord>=FEATURES_ROW_PER_CHUNK){
				col_id = col_id + 1;
			}
			//cout<<"col:"<<col<<"->"<<col_id<<endl;
			if (col_id<0||col_id>=VERTICAL_FEATURE_CHUNK_NUM){
				continue;
			}
			//cout<<"col_coord:"<<col_coord;
			col_coord = (col_coord + FEATURES_COL_PER_CHUNK)%FEATURES_COL_PER_CHUNK;
			assert(col_coord>=0 && col_coord <FEATURES_COL_PER_CHUNK);
			//cout<<"->"<<col_coord<<endl;

			if(row_coord<0){
				row_id = row_id - 1;
			}else if(row_coord>=FEATURES_COL_PER_CHUNK){
				row_id = row_id + 1;
			}
			if (row_id<0||row_id>=HORIZONTAL_FEATURE_CHUNK_NUM){
				continue;
			}
			//cout<<"row:"<<row<<"->"<<row_id<<endl;
			//cout<<"row_coord:"<<row_coord;
			row_coord = (row_coord + FEATURES_ROW_PER_CHUNK)%FEATURES_ROW_PER_CHUNK;
			assert(row_coord>=0 && row_coord <FEATURES_ROW_PER_CHUNK);
			//cout<<"->"<<row_coord<<endl;
			pe_id_type dest_pe = row_id*HORIZONTAL_FEATURE_CHUNK_NUM+col_id;
			assert(dest_pe>=0 && dest_pe<NUM_OF_PEs);
			//cout<<"pe:"<<(row*HORIZONTAL_FEATURE_CHUNK_NUM+col)<<"->"<<dest_pe<<endl;
			PE[dest_pe].accumulator[ocoord].adder(row_coord, col_coord, product);
		}
	}
	total_weights += num_of_processed_weights;
}
