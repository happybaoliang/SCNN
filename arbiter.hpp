#ifndef ARBITER_HPP__
#define ARBITER_HPP__


#include"common.hpp"


struct arbiter{
	void reset();
	priority_type priority[NUM_OF_REQUESTS];
	void arbitrate(request_type req[NUM_OF_REQUESTS],grant_type (&gnt)[NUM_OF_REQUESTS]);
	void granter(priority_type& pri,carrier_type& c_in,carrier_type& c_out,request_type& req, grant_type& grant);
};


#endif
