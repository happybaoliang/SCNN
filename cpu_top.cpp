#include<iostream>
#include<cassert>
#include<cstdlib>
#include<ctime>
#include"network.hpp"
#include"cpu_top.hpp"
#include"fpga_top.hpp"

using namespace std;


int main(){
#ifndef INPUT_HALOS
	assert(NUM_OF_PORTS==0);
#endif

	srand((unsigned)time(NULL));

	assert(MAX_ZERO_COUNT<=(1<<ZERO_WIDTH));

	assert((MAX_NUM_OF_FEATURE_PER_CHUNK%I)==0);

	assert(NBITS(MAX_WEIGHT_VALUE/2)<=DATA_WIDTH);

	assert(NBITS(MAX_FEATURE_VALUE/2)<=DATA_WIDTH);

	assert((I*F*NUM_OF_PEs)<=MAX_NUM_OF_DSP_AVAILABLE);

	assert(MAX_FEATURES_ROW_PER_CHUNK<MAX_FEATURE_DIMENSION);

	assert(MAX_FEATURES_COL_PER_CHUNK<MAX_FEATURE_DIMENSION);

	cout<<"PE is under utilized"<<endl;
	cout<<"memory is not released"<<endl;
	cout<<"data overflow is not correctly solved"<<endl;
	cout<<"variable stride is not supported"<<endl;
	cout<<"fully connected layer is not implemented"<<endl;

	network_t *net = CreateNetwork();

	for (numlayers_t i=0;i<net->num_layers;i++){
		net->layers[i].AllocateMemoryForWeight();
		net->layers[i].AllocateMemoryForInputFeature();
		net->layers[i].AllocateMemoryForOutputFeature();
#if 1
		net->layers[i].GenerateRandomWeight();
		net->layers[i].GenerateRandomFeatureMap();
		net->layers[i].DumpGeneratedWeight("../../../weights.bin");
		net->layers[i].DumpGeneratedFeatureMap("../../../features.bin");
#else
		net->layers[i].LoadGeneratedWeight("../../../weights.bin");
		net->layers[i].LoadGeneratedFeatureMap("../../../features.bin");
#endif
		net->layers[i].AllocateMemoryForCompressedWeight();
		net->layers[i].CompressWeights();

		net->layers[i].AllocateMemoryForCompressedInputFeature();
		net->layers[i].CompressInputFeatureMap();

		net->layers[i].AllocateMemoryForCompressedOutputFeature();
	}

	for (int i=0;i<net->num_layers;i++){
		Accelerator(net->layers[i].config);
		net->layers[i].DeCompressOutputFeatureMap();
		if (net->layers[i].CheckDeCompressedConvolutionResults()){
			return -1;
		}
	}

	return 0;
}
