#include"arbiter.hpp"


static priority_type priority[NUM_OF_RESOURCES];


void Granter(priority_type& pri,carrier_type c_in,carrier_type& c_out,request_type req, grant_type& grant){
	grant = req & (c_in|pri);
	c_out = (~req) & (c_in|pri);
}


void Arbiter(request_type req[NUM_OF_REQUESTS],grant_type gnt[NUM_OF_REQUESTS]){
	priority_type any_grant = 0;
	carrier_type carrier[NUM_OF_RESOURCES];

	for (int i=0;i<NUM_OF_REQUESTS-1;i++){
		if (i==0){
			Granter(priority[0],carrier[NUM_OF_REQUESTS-1],carrier[1],req[0],gnt[0]);
		}else{
			Granter(priority[i],carrier[i],carrier[i+1],req[i],gnt[i]);
		}
	}

	for (int i=0;i<NUM_OF_REQUESTS;i++){
		any_grant |= priority[i];
	}

	if (any_grant){
		priority_type pri = priority[NUM_OF_REQUESTS-1];
		for (int i=NUM_OF_REQUESTS-1;i>0;i--){
			priority[i]=priority[i-1];
		}
		priority[0] = pri;
	}
}
