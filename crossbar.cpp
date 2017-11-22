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
}


bool crossbar::queueing(hls::stream<Flit> (&products)[F][I],hls::stream<Flit>* input_halos[NUM_OF_PORTS]){
	Flit flit[NUM_OF_REQUESTS];
	bool is_input_queue_empty = false;
	valid_type valid[NUM_OF_REQUESTS]={0};
	grant_type grant[NUM_OF_RESOURCES][NUM_OF_REQUESTS]={1};
	request_type request[NUM_OF_RESOURCES][NUM_OF_REQUESTS]={0};

	for (int i=0;i<F;i++){
		for (int j=0;j<I;j++){
			grant_type gnt = 0;
			for (int k=0;k<NUM_OF_RESOURCES;k++){
				gnt |= grant[k][i*I+j];
			}
			if (gnt){
				if (!products[i][j].empty()){
					products[i][j].read(flit[i*I+j]);
					valid[i*I+j]=1;
				}else{
					valid[i*I+j]=0;
				}
			}
		}
	}

	for (int i=0;i<NUM_OF_PORTS;i++){
		if (input_halos[i]!=NULL){
			grant_type gnt = 0;
			for (int j=0;j<NUM_OF_RESOURCES;j++){
				gnt |= grant[j][I*F+i];
			}
			if (gnt){
				if (!input_halos[i]->empty()){
					input_halos[i]->read(flit[I*F+i]);
					valid[i]=1;
				}else{
					valid[i]=0;
				}
			}
		}
	}

	for (int i=0;i<NUM_OF_RESOURCES;i++){
		for (int j=0;j<NUM_OF_REQUESTS;j++){
			request[i][j]=0;
		}
	}
	for (int i=0;i<NUM_OF_REQUESTS;i++){
		if (valid[i]){
			request[flit[i].ochannel*flit[i].row][i] = 1;
		}
	}

/*
	cout<<"request"<<endl;
	for (int i=0;i<NUM_OF_REQUESTS;i++){
		cout<<request[i]<<" ";
	}
	cout<<endl;
*/
	for (int i=0;i<NUM_OF_RESOURCES;i++){
		arb[i].arbitrate(request[i],grant[i]);
	}
/*
	cout<<"crossbar::grant"<<endl;
	for (int i=0;i<NUM_OF_REQUESTS;i++){
		cout<<grant[i]<<" ";
	}
	cout<<endl;
*/

	for (int i=0;i<NUM_OF_REQUESTS;i++){
		grant_type gnt = 0;
		for (int j=0;j<NUM_OF_RESOURCES;j++){
			gnt |= grant[j][i];
		}
		if (gnt){
			//cout<<"ochannel="<<flit[i].ochannel;
			//cout<<" row="<<flit[i].row<<" col="<<flit[i].col;
			acc[flit[i].ochannel][flit[i].row].adder(flit[i].col,flit[i].product);
		}
	}

	for (int i=0;i<NUM_OF_REQUESTS;i++){
		if (valid[i]){
			is_input_queue_empty = true;
		}
	}

	return is_input_queue_empty;
}
