#include<iostream>
#include<cstring>
#include<cassert>
#include"process_element.hpp"

using namespace std;


void ProcessElement::DrainOutProducts(){
	while(input_queue_not_empty){
		//cout<<"draining PE["<<row<<"]["<<col<<"]"<<endl;
#ifdef INPUT_HALOS
		input_queue_not_empty = acc.queueing(flits,input_halos);
#else
		input_queue_not_empty = acc.queueing(flits);
#endif
	}
}


inline output_channel_t ProcessElement::GetOffsetInMatrix(){
	return (total_weights+num_of_processed_weights-1) % num_of_weights_per_kernel;
}


void ProcessElement::SetNextInputChannel(input_channel_t channel){
	stall = false;
	total_weights = 0;
	total_features = 0;
	num_of_processed_weights=0;
	num_of_processed_features = 0;
	current_input_channel = channel;
	num_of_none_zero_feature_fetched = 0;
}


inline output_channel_t ProcessElement::GetOCoord(){
	output_channel_t ocoord = (total_weights+num_of_processed_weights-1)/num_of_weights_per_kernel;
	//cout<<"ocoord=("<<total_weights<<"+"<<num_of_processed_weights<<"-1)"<<"/"<<num_of_weights_per_kernel<<"="<<ocoord<<endl;
	return ocoord;
}


void ProcessElement::FetchNextIFeatureMap(){
	if (num_of_none_zero_feature_fetched<num_of_none_zero_features[current_input_channel]){
		memcpy(feature_buf,featuremap[current_input_channel]+num_of_none_zero_feature_fetched,I*sizeof(feature_t));
		memcpy(feature_index_buf,featureindex[current_input_channel]+num_of_none_zero_feature_fetched,I*sizeof(zero_t));
		num_of_none_zero_feature_fetched = num_of_none_zero_feature_fetched + I;
		total_features += num_of_processed_features;
		num_of_processed_features = 0;
		num_of_processed_weights = 0;
		total_weights = 0;
		stall = false;
	}else{
		stall = true;
		cout<<"PE"<<(horizontal_input_feature_chunk_num*row+col)<<" stalled:";
		cout<<" total features:"<<num_of_none_zero_feature_fetched<<endl;
	}
}


#ifdef INPUT_HALOS
inline dimension_t ProcessElement::GetColCoord(){
	dimension_t wcol = GetOffsetInMatrix()%kernel_size-kernel_size/2;
	dimension_t xcoord = (total_features+num_of_processed_features-1)%MAX_FEATURES_COL_PER_CHUNK-wcol;
	assert((xcoord>=-kernel_size/2) && (xcoord<MAX_FEATURES_ROW_PER_CHUNK+kernel_size/2));
	return xcoord;
}


inline dimension_t ProcessElement::GetRowCoord(){
	dimension_t wrow = GetOffsetInMatrix()/kernel_size-kernel_size/2;
	dimension_t ycoord = (total_features+num_of_processed_features-1)/MAX_FEATURES_COL_PER_CHUNK-wrow;
	assert((ycoord>=-kernel_size/2) && (ycoord<MAX_FEATURES_COL_PER_CHUNK+kernel_size/2));
	return ycoord;
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
			output_channel_t ocoord = GetOCoord();
			assert(ocoord>=0 && ocoord<MAX_OUTPUT_CHANNEL_NUM);
			//cout<<"ocoord="<<ocoord<<endl;

			dimension_t col_coord = GetColCoord();
			pe_t col_id = col;
			if (col_coord<0){
				col_id = col_id -1;
			}else if (col_coord>=MAX_FEATURES_ROW_PER_CHUNK){
				col_id = col_id + 1;
			}
			//cout<<"col:"<<col<<"->"<<col_id<<endl;
			if (col_id<0||col_id>=vertical_input_feature_chunk_num){
				//cout<<"col_id="<<col_id<<endl;
				continue;
			}
			//cout<<"col_coord:"<<col_coord;
			col_coord = (col_coord + MAX_FEATURES_COL_PER_CHUNK)%MAX_FEATURES_COL_PER_CHUNK;
			assert(col_coord>=0 && col_coord <MAX_FEATURES_COL_PER_CHUNK);
			//cout<<"->"<<col_coord<<endl;

			dimension_t row_coord = GetRowCoord();
			pe_t row_id = row;
			if(row_coord<0){
				row_id = row_id - 1;
			}else if(row_coord>=MAX_FEATURES_COL_PER_CHUNK){
				row_id = row_id + 1;
			}
			if (row_id<0||row_id>=horizontal_input_feature_chunk_num){
				//cout<<"("<<row_id<<","<<col_id<<")"<<endl;
				//cout<<"row_id="<<row_id<<endl;
				continue;
			}
			//cout<<"row:"<<row<<"->"<<row_id<<endl;

			//cout<<"row_coord:"<<row_coord;
			row_coord = (row_coord + MAX_FEATURES_ROW_PER_CHUNK)%MAX_FEATURES_ROW_PER_CHUNK;
			assert(row_coord>=0 && row_coord <MAX_FEATURES_ROW_PER_CHUNK);
			//cout<<"->"<<row_coord<<endl;

			product_t prod = weight[j]*feature_buf[i];

			if ((row_id<row) && (col_id==col)){
				//assert(output_halos[NORTH_PORT]!=NULL);
				output_halos[NORTH_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[UPPER]->"<<row_coord<<","<<col_coord<<endl;
			}else if ((row_id<row) && (col_id<col)){
				//assert(output_halos[NORTH_WEST_PORT]!=NULL);
				output_halos[NORTH_WEST_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[NORTH_WEST]->"<<row_coord<<","<<col_coord<<endl;
			}else if ((row_id<row) && (col_id>col)){
				//assert(output_halos[NORTH_EAST_PORT]!=NULL);
				output_halos[NORTH_EAST_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[NORTH_EAST]->"<<row_coord<<","<<col_coord<<endl;
			}else if ((row_id==row) && (col_id<col)){
				//assert(output_halos[WEST_PORT]!=NULL);
				output_halos[WEST_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[LEFT]->"<<row_coord<<","<<col_coord<<endl;
			}else if ((row_id==row) && (col_id>col)){
				//assert(output_halos[EAST_PORT]!=NULL);
				output_halos[EAST_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[RIGHT]->"<<row_coord<<","<<col_coord<<endl;
			}else if ((row_id>row) && (col_id==col)){
				//assert(output_halos[SOUTH_PORT]!=NULL);
				output_halos[SOUTH_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[DOWN]->"<<row_coord<<","<<col_coord<<endl;
			}else if ((row_id>row) && (col_id>col)){
				//assert(output_halos[SOUTH_EAST_PORT]!=NULL);
				output_halos[SOUTH_EAST_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[SOUTH_EAST]->"<<row_coord<<","<<col_coord<<endl;
			}else if ((row_id>row) && (col_id<col)){
				//assert(output_halos[SOUTH_WEST_PORT]!=NULL);
				output_halos[SOUTH_WEST_PORT]->write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[SOUTH_WEST]->"<<row_coord<<","<<col_coord<<endl;
			}else{
				flits[j][i].write(Flit(ocoord,row_coord,col_coord,prod));
				//cout<<"PE["<<row<<"]["<<col<<"]:"<<GetRowCoord()<<","<<GetColCoord()<<"-[LOCAL]->"<<row_coord<<","<<col_coord<<endl;
			}
		}
	}
	input_queue_not_empty = acc.queueing(flits,input_halos);

	total_weights += num_of_processed_weights;
}
#else
inline dimension_t ProcessElement::GetColCoord(){
	dimension_t wcol = GetOffsetInMatrix()%kernel_size;
	dimension_t actual_cols_per_chunk = MAX_FEATURES_COL_PER_CHUNK + 2*(kernel_size/2);
	dimension_t col_coord = (total_features+num_of_processed_features-1)%actual_cols_per_chunk - wcol;
	//cout<<"wcol="<<wcol<<" col_coord="<<(total_features+num_of_processed_features-1)<<"%"<<actual_cols_per_chunk<<"="<<col_coord<<endl;
	return col_coord;
}


inline dimension_t ProcessElement::GetRowCoord(){
	dimension_t wrow = GetOffsetInMatrix()/kernel_size;
	dimension_t actual_cols_per_chunk = MAX_FEATURES_ROW_PER_CHUNK + 2*(kernel_size/2);
	dimension_t row_coord = (total_features+num_of_processed_features-1)/actual_cols_per_chunk - wrow;
	//cout<<"wrow="<<wrow<<" row_coord="<<(total_features+num_of_processed_features-1)<<"/"<<actual_cols_per_chunk<<"="<<row_coord<<endl;
	return row_coord;
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
			output_channel_t ocoord = GetOCoord();
			assert(ocoord>=0 && ocoord<MAX_OUTPUT_CHANNEL_NUM);

			dimension_t col_coord = GetColCoord();
			if (col_coord<0 || col_coord>=MAX_FEATURES_COL_PER_CHUNK){
				continue;
			}
			assert((col_coord>=0) && (col_coord<MAX_FEATURES_COL_PER_CHUNK));

			dimension_t row_coord = GetRowCoord();
			if (row_coord<0 || row_coord>=MAX_FEATURES_ROW_PER_CHUNK){
				continue;
			}
			assert((row_coord>=0) && (row_coord<MAX_FEATURES_ROW_PER_CHUNK));

			product_t prod = weight[j]*feature_buf[i];
			//cout<<"["<<ocoord<<"]["<<row_coord<<"]["<<col_coord<<"]="<<prod<<endl;

			flits[j][i].write(Flit(ocoord,row_coord,col_coord,prod));
		}
	}
	input_queue_not_empty = acc.queueing(flits);

	total_weights += num_of_processed_weights;
}
#endif
