#include<iostream>
#include"network.hpp"

using namespace std;


network_t* CreateNetwork() {
	network_t *net = new network_t(27);

    //Layer Attributes:  (W,   H,   CI,  CO, K, P, 	  S, R,	  kernels_per_group)
	net->AddLayer(layer_t(64,  64,  3,   3,  3, true, 1, true,1));

	return net;
}


void network_t::PrintNetwork() {
	for (int i = 0; i < num_layers; i++) {
		cout<<"layer: "<<i<<endl;
		layers[i].PrintLayer();
	}
}


inline void network_t::AddLayer(layer_t layer) {
	assert((num_layers>=0) && (num_layers<MAX_NUM_OF_LAYERS));
	layers[num_layers] = layer;
	num_layers++;
};



network_t::network_t(int total_layers){
	  assert((total_layers>=1) && (total_layers<=MAX_NUM_OF_LAYERS));
	  layers = new layer_t[total_layers];
	  if (layers==NULL){
		  cout<<"failed to allocate memory for network"<<endl;
		  exit(-1);
	  }
	  num_layers = 0;
}
