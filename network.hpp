#ifndef NETWORK_HPP__
#define NETWORK_HPP__

#include"layer.hpp"


struct network_t {
  layer_t *layers;
  numlayers_t num_layers;

  void PrintNetwork();
  network_t(int total_layers);
  void AddLayer(layer_t layer);
};


network_t* CreateNetwork();


#endif
