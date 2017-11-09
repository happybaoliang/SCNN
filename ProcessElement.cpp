#include<iostream>
#include<cassert>
#include<cstring>
#include<cmath>
#include"ProcessElement.hpp"

using namespace std;


extern struct ProcessElement PE[NUM_OF_PEs];


inline int ProcessElement::GetOffsetInMatrix(){
	return total_weights % num_of_weights_per_kernel;
}


inline ocoord_type ProcessElement::GetOCoord(){
	return ceil(1.0*total_weights/num_of_weights_per_kernel);
}


inline xcoord_type ProcessElement::GetXCoord(){
	unsigned char wcol = GetOffsetInMatrix()%KERNEL_SIZE-KERNEL_SIZE/2;
	xcoord_type xcoord = total_features%FEATURES_COL_PER_CHUNK - wcol;
	assert(xcoord>=-KERNEL_SIZE/2 && xcoord<FEATURES_ROW_PER_CHUNK+KERNEL_SIZE/2);
	return xcoord;
}


inline ycoord_type ProcessElement::GetYCoord(){
	unsigned char wrow = GetOffsetInMatrix()/KERNEL_SIZE-KERNEL_SIZE/2;
	ycoord_type ycoord = total_features/FEATURES_COL_PER_CHUNK-wrow;
	assert(ycoord>=-KERNEL_SIZE/2 && ycoord<FEATURES_COL_PER_CHUNK+KERNEL_SIZE/2);
	return ycoord;
}


void ProcessElement::FetchNextIFeatureMap(){
	if (num_of_none_zero_feature_fetched<num_of_none_zero_features[current_input_channel]){
		memcpy(feature_buf,featuremap[current_input_channel]+num_of_none_zero_feature_fetched,I*sizeof(feature_type));
		memcpy(feature_index_buf,featureindex[current_input_channel]+num_of_none_zero_feature_fetched,I*sizeof(zeros_type));
		num_of_none_zero_feature_fetched = num_of_none_zero_feature_fetched + I;
	}
}


void ProcessElement::AccumulateProduct(){
	for (int i=0;i<I;i++){
		total_features+=feature_index_buf[i]+1;
		for (int j=0;j<F;j++){
			total_weights+=weightindex[j]+1;
			xcoord_type xcoord = GetXCoord();
			ycoord_type ycoord = GetYCoord();
			ocoord_type ocoord = GetOCoord();
			feature_type product=weight[j]*feature_buf[i];
			assert(ocoord>=0 && ocoord<OUTPUT_CHANNEL_NUM);
			int col_id = col;
			int row_id = row;
			if (xcoord<0){
				col_id = col_id -1;
			}else if (xcoord>=FEATURES_ROW_PER_CHUNK){
				col_id = col_id + 1;
			}
			cout<<"current col_id:"<<col<<" dest col_id:"<<col_id<<endl;
			if (col_id<0||col_id>=VERTICAL_FEATURE_CHUNK_NUM){
				continue;
			}
			cout<<"current xcoord:"<<xcoord<<" ";
			xcoord = (xcoord + FEATURES_COL_PER_CHUNK)%FEATURES_COL_PER_CHUNK;
			assert(xcoord>=0 && xcoord <FEATURES_COL_PER_CHUNK);
			cout<<"dest xcoord:"<<xcoord<<endl;

			if(ycoord<0){
				row_id = row_id - 1;
			}else if(ycoord>=FEATURES_COL_PER_CHUNK){
				row_id = row_id + 1;
			}
			if (row_id<0||row_id>=HORIZONTAL_FEATURE_CHUNK_NUM){
				continue;
			}
			cout<<"current row_id:"<<row<<" dest　row_id:"<<row_id<<endl;
			cout<<"current ycoord:"<<ycoord<<endl;
			ycoord = (ycoord + FEATURES_ROW_PER_CHUNK)%FEATURES_ROW_PER_CHUNK;
			assert(ycoord>=0 && ycoord <FEATURES_ROW_PER_CHUNK);
			cout<<" dest ycoord:"<<ycoord<<endl;
			int dest_pe = row_id*HORIZONTAL_FEATURE_CHUNK_NUM+col_id;
			assert(dest_pe>=0 && dest_pe<NUM_OF_PEs);
			cout<<"current pe:"<<(row*HORIZONTAL_FEATURE_CHUNK_NUM+col)<<" dest pe:"<<dest_pe<<endl;
			PE[dest_pe].accumulator[ocoord][xcoord][ycoord] += product;
		}
	}
}
