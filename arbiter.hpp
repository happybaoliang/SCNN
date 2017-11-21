#ifndef ARBITER_HPP__
#define ARBITER_HPP__


#include<ap_int.h>
#include"cpu_top.hpp"


#define NUM_OF_REQUESTS		(F*I)
#define NUM_OF_RESOURCES	(2*F*I)


typedef ap_int<1> grant_type;
typedef ap_int<1> carrier_type;
typedef ap_int<1> request_type;
typedef ap_int<1> priority_type;


#endif
