#include"network.hpp"


network_t* CreateNetwork() {
	network_t *net = new network_t(27);

    //Layer Attributes:  (W,   H,   CI,  CO, K, P, 	  S, R,	  1)
	net->AddLayer(layer_t(256, 256, 3,   64, 3, true, 1, true,0));

	return net;
}


void network_t::PrintNetwork() {
	for (int i = 0; i < num_layers; i++) {
		cout<<"layer: "<<i<<endl;
		layers[i].PrintLayer();
	}
}


inline void network_t::AddLayer(layer_t layer) {
	layers[num_layers] = layer;
	num_layers++;
};



