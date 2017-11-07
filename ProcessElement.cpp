#include<cassert>
#include<cmath>
#include"ProcessElement.hpp"


inline unsigned char ProcessElement::GetMatrixId(){//start from 1
	unsigned char num_of_weights_per_kernel = kernel_size*kernel_size;
	return ceil(1.0*total_weights/num_of_weights_per_kernel);
}


inline xcoord_type ProcessElement::GetXCoord(){
	unsigned char matrix_id = GetMatrixId();
	assert(matrix_id !=0);
	unsigned char wcol = (total_weights%matrix_id)%kernel_size-kernel_size/2;
	return total_features%FEATURE_HEIGHT-wcol;
}


inline ycoord_type ProcessElement::GetYCoord(){
	unsigned char matrix_id = GetMatrixId();
	assert(matrix_id !=0);
	unsigned char wrow = ceil(1.0*(total_weights%matrix_id)/kernel_size)-kernel_size/2;
	return total_features%FEATURE_WIDTH-wrow;
}


inline ocoord_type ProcessElement::GetOCoord(){
	unsigned char num_of_weights_per_kernel = kernel_size*kernel_size;
	unsigned char matrix_id = total_weights/num_of_weights_per_kernel;
	return matrix_id/total_input_channel;
}


void ProcessElement::AccumulateProduct(){
	for (int i=0;i<I;i++){
		total_features+=featureindex[i]+1;
		for (int j=0;j<F;j++){
			total_weights+=weightindex[j]+1;
			xcoord_type xcoord = GetXCoord();
			ycoord_type ycoord = GetYCoord();
			ocoord_type ocoord = GetOCoord();
			feature_type product=weight[j]*featuremap[i];
			assert(ocoord<0 || ocoord>OUTPUT_CHANNEL_NUM);
			//assert(xcoord<0 || xcoord>FEATURES_ROW_PER_CHUNK);
			//assert(ycoord<0 || ycoord>FEATURES_COL_PER_CHUNK);

			//output_feature[ocoord][xcoord][ycoord] += product;
		}
	}
}
