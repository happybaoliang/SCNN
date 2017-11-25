#ifndef NETWORK_HPP__
#define NETWORK_HPP__


#include"common.hpp"
#include"layer.hpp"


struct network_t {
  layer_t *layers;
  numlayers_t num_layers;

  void PrintNetwork();
  void AddLayer(layer_t layer);
  network_t(int total_layers): num_layers(0){
	  layers = new layer_t[total_layers];
	  if (layers==NULL){
		  cout<<"failed to allocate memory for network"<<endl;
		  exit(-1);
	  }
  }
};


network_t* CreateNetwork();


#endif
