#include"crossbar.hpp"

using namespace std;


void crossbar::reset(){
	for (int i=0;i<OUTPUT_CHANNEL_CHUNK_SIZE;i++){
		for (int j=0;j<FEATURES_ROW_PER_CHUNK;j++){
			acc[i][j].reset();
		}
	}

	for (int i=0;i<NUM_OF_RESOURCES;i++){
		arb[i].reset();
	}

	for (int i=0;i<NUM_OF_REQUESTS;i++){
		valid[i] = 0;
	}
}


bool crossbar::queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]){
	bool input_queue_empty_n = false;
	grant_type grant[NUM_OF_RESOURCES][NUM_OF_REQUESTS]={1};
	request_type request[NUM_OF_RESOURCES][NUM_OF_REQUESTS]={0};

	for (int i=0;i<F;i++){
		for (int j=0;j<I;j++){
			if (valid[i*I+j]==0){
				if (!products[i][j].empty()){
					products[i][j].read(flit[i*I+j]);
					valid[i*I+j] = 1;
				}
			}
		}
	}

	for (int i=0;i<NUM_OF_PORTS;i++){
		if (input_halos[i]!=NULL){
			if (valid[I*F+i]==0){
				if (!input_halos[i]->empty()){
					input_halos[i]->read(flit[I*F+i]);
					valid[I*F+i] = 1;
				}
			}
		}
	}

	for (int i=0;i<NUM_OF_RESOURCES;i++){
		for (int j=0;j<NUM_OF_REQUESTS;j++){
			if (flit[i].ochannel*flit[i].row==i){
				if(valid[j]){
					request[i][j] = 1;
				}else{
					request[i][j] = 0;
				}
			}else{
				request[i][j] = 0;
			}
		}
	}

#ifdef DEBUG_ARBITER
	cout<<"crossbar::request"<<endl;
	for (int i=0;i<NUM_OF_RESOURCES;i++){
		for (int j=0;j<NUM_OF_REQUESTS;j++){
			cout<<request[i][j]<<" ";
		}
		cout<<endl;
	}
#endif

	for (int i=0;i<NUM_OF_RESOURCES;i++){
		arb[i].arbitrate(request[i],grant[i]);
	}

#ifdef DEBUG_ARBITER
	cout<<"crossbar::grant"<<endl;
	for (int i=0;i<NUM_OF_RESOURCES;i++){
		for (int j=0;j<NUM_OF_REQUESTS;j++){
			cout<<grant[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
#endif

	for (int i=0;i<NUM_OF_REQUESTS;i++){
		grant_type gnt = 0;
		for (int j=0;j<NUM_OF_RESOURCES;j++){
			gnt |= grant[j][i];
		}
		if (gnt){
			valid[i] = 0;
			//cout<<"ochannel="<<flit[i].ochannel;
			//cout<<" row="<<flit[i].row<<" col="<<flit[i].col;
			acc[flit[i].ochannel][flit[i].row].adder(flit[i].col,flit[i].product);
			//cout<<endl;
		}
	}

	for (int i=0;i<F;i++){
		for (int j=0;j<I;j++){
			if (!products[i][j].empty()||(valid[i*I+j]==1)){
				input_queue_empty_n = true;
			}
		}
	}
	for (int i=0;i<NUM_OF_PORTS;i++){
		if (input_halos[i]!=NULL){
			if (!input_halos[i]->empty()){
				input_queue_empty_n = true;
			}
		}
	}

	return input_queue_empty_n;
}
