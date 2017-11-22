#include"arbiter.hpp"


void arbiter::reset(){
	priority[0]=1;
	for (int i=1;i<NUM_OF_REQUESTS;i++){
		priority[i]=0;
	}
}


void arbiter::granter(priority_type& pri,carrier_type& c_in,carrier_type& c_out,request_type& req, grant_type& grant){
	grant = req & (c_in|pri);
	c_out = (~req) & (c_in|pri);
	//cout<<grant<<" ";
}


void arbiter::arbitrate(request_type req[NUM_OF_REQUESTS],grant_type (&gnt)[NUM_OF_REQUESTS]){
	priority_type any_grant = 0;
	carrier_type carrier[NUM_OF_REQUESTS]={1};

	//cout<<"granter::grant"<<endl;
	for (int i=0;i<NUM_OF_REQUESTS;i++){
		if (i==0){
			granter(priority[i],carrier[NUM_OF_REQUESTS-1],carrier[i],req[i],gnt[i]);
		}else{
			granter(priority[i],carrier[i-1],carrier[i],req[i],gnt[i]);
		}
	}
	//cout<<endl;

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
/*
	cout<<"arbiter::grant"<<endl;
	for (int i=0;i<NUM_OF_REQUESTS;i++){
		cout<<gnt[i]<<" ";
	}
	cout<<endl;
*/
}
