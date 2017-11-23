#ifndef ARBITER_HPP__
#define ARBITER_HPP__


#include"cpu_top.hpp"


enum Port{
	WEST_PORT = 0,
	NORTH_WEST_PORT,
	EAST_PORT,
	NORTH_EAST_PORT,
	NORTH_PORT,
	SOUTH_EAST_PORT,
	SOUTH_PORT,
	SOUTH_WEST_PORT,
	NUM_OF_PORTS
};


#define NUM_OF_REQUESTS		(F*I+NUM_OF_PORTS)
#define NUM_OF_RESOURCES	(FEATURES_ROW_PER_CHUNK*OUTPUT_CHANNEL_CHUNK_SIZE)


typedef ap_uint<1> valid_type;
typedef ap_uint<1> grant_type;
typedef ap_uint<1> carrier_type;
typedef ap_uint<1> request_type;
typedef ap_uint<1> priority_type;


struct arbiter{
	void reset();
	priority_type priority[NUM_OF_REQUESTS];
	void arbitrate(request_type req[NUM_OF_REQUESTS],grant_type (&gnt)[NUM_OF_REQUESTS]);
	void granter(priority_type& pri,carrier_type& c_in,carrier_type& c_out,request_type& req, grant_type& grant);
};


#endif
