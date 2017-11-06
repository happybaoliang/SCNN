#include"MultiplyArray.hpp"


void MultiplyAlltoAll(weight_type w[F], feature_type a[I]){
	feature_type product[I];
	for (int i=0;i<F;i++){
		for (int j=0;j<I;j++){
			product[j]=w[i]*a[j];
		}
	}
}
