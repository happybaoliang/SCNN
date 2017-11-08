#include<cassert>
#include<cmath>
#include"ProcessElement.hpp"

/*
inline unsigned char ProcessElement::GetMatrixCount(){//start from 1
	return ceil(1.0*total_weights/num_of_weights_per_kernel);
}


inline unsigned char ProcessElement::GetOffsetInMatrix(){
	return total_weights % num_of_weights_per_kernel;
}


inline unsigned char ProcessElement::GetMatrixInputChannel(){
	unsigned char count = GetMatrixCount();
	return (count % total_input_channel);
}


inline unsigned char ProcessElement::GetMatrixOutputChannel(){
	unsigned char count = GetMatrixCount();
	return (count / total_input_channel);
}


inline unsigned char ProcessElement::GetFeatureInputChannel(){
	unsigned char channel = total_features / NUM_OF_FEATURES_PER_CHANNEL;
	return channel;
}


inline xcoord_type ProcessElement::GetXCoord(){
	unsigned char matrix_id = GetMatrixCount();
	assert(matrix_id !=0);
	unsigned char offset = GetOffsetInMatrix();
	unsigned char wcol = offset / kernel_size-kernel_size/2;
	return total_features%FEATURES_ROW_PER_CHUNK-wcol;
}


inline ycoord_type ProcessElement::GetYCoord(){
	unsigned char matrix_id = GetMatrixCount();
	assert(matrix_id !=0);
	unsigned char offset = GetOffsetInMatrix();
	unsigned char wrow = offset % kernel_size-kernel_size/2;
	return total_features%FEATURES_COL_PER_CHUNK-wrow;
}


inline ocoord_type ProcessElement::GetOCoord(){
	ocoord_type ocoord = 0;
	unsigned char matrix_id = GetMatrixCount();
	ocoord = matrix_id/total_input_channel;
	assert(ocoord>=0 && ocoord<OUTPUT_CHANNEL_NUM);
	return ocoord;
}
*/

void ProcessElement::AccumulateProduct(){
	feature_type feature_buf[I];
	zeros_type feature_index_buf[I];

	for (int i=0;i<I;i++){
		for (int j=0;j<F;j++){

		}
	}
	/*
	for (int i=0;i<I;i++){
		total_features+=featureindex[i]+1;
		unsigned char feature_input_channel = GetFeatureInputChannel();
		for (int j=0;j<F;j++){
			total_weights+=weightindex[j]+1;
			unsigned char weight_input_channel = GetMatrixInputChannel();
			if (feature_input_channel != weight_input_channel){
				continue;
			}
			xcoord_type xcoord = GetXCoord();
			ycoord_type ycoord = GetYCoord();
			ocoord_type ocoord = GetOCoord();
			feature_type product=weight[j]*featuremap[i];
			//assert(xcoord<0 || xcoord>FEATURES_ROW_PER_CHUNK);
			//assert(ycoord<0 || ycoord>FEATURES_COL_PER_CHUNK);
			//output_feature[ocoord][xcoord][ycoord] += product;
		}
	}
	*/
}
