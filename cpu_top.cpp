#include"common.hpp"
#include"cpu_top.hpp"
#include"fpga_top.hpp"
#include"network.hpp"
#include"layer.hpp"


int main(){
	srand((unsigned)time(NULL));

	assert((MAX_NUM_OF_FEATURE_PER_CHUNK%I)==0);

	cout<<"variable stride is not supported"<<endl;

	network_t *net = CreateNetwork();

	for (int i=0;i<net->num_layers;i++){
		net->layers[i].AllocateMemoryForWeight();
		net->layers[i].AllocateMemoryForCompressedWeight();

		net->layers[i].AllocateMemoryForInputFeature();
		net->layers[i].AllocateMemoryForOutputFeature();
		net->layers[i].AllocateMemoryForCompressedInputFeature();
		net->layers[i].AllocateMemoryForCompressedOutputFeature();

#if 1
		net->layers[i].GenerateRandomWeight();
		net->layers[i].GenerateRandomFeatureMap();
		net->layers[i].DumpGeneratedWeight("../../../weights.bin");
		net->layers[i].DumpGeneratedFeatureMap("../../../features.bin");
#else
		net->layers[i].LoadGeneratedWeight("../../../weights.bin");
		net->layers[i].LoadGeneratedFeatureMap("../../../features.bin");
#endif

		net->layers[i].CompressWeights();
		net->layers[i].CompressInputFeatureMap();
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
