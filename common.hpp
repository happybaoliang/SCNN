#ifndef COMMON_HPP__
#define COMMON_HPP__

#include<ap_int.h>


#define USE_RANDOM_INPUT					1
#define INPUT_HALOS							1

#define ZERO_WIDTH							4
#define DATA_WIDTH							8

//固定IF乘积为３６以后，所有可能的方案中最节省bank的方式是4 x 9或者6 x 6，这两种方案每个PE存储feature和w所用的总bank数为６。
#define F									9
#define I									4

#define MAX_NUM_OF_DSP_AVAILABLE			2520

#define MAX_KERNEL_SIZE						12
#define MAX_INPUT_CHANNEL_NUM				64
#define MAX_OUTPUT_CHANNEL_NUM				64
#define MAX_FEATURE_DIMENSION				256
#define MAX_STRIDE							3
#define MAX_NUM_OF_LAYERS 					1000

#define NUM_OF_PEs							1//(MAX_OUTPUT_CHANNEL_CHUNK_NUM*MAX_FEATURE_CHUNK_NUM)

#define MAX_OUTPUT_CHANNEL_GROUP_SIZE		3

//当PE数为６４时，ZU9EG的寄存器资源最多只能容纳8x8的分块，这时accumulator占的寄存器资源达到５０％，最大可尝试的分块大小为11 x 11
#define MAX_FEATURES_ROW_PER_CHUNK			8
#define MAX_FEATURES_COL_PER_CHUNK			8


enum Port{
#ifdef INPUT_HALOS
	WEST_PORT = 0,
	EAST_PORT,
	NORTH_PORT,
	SOUTH_PORT,
	NORTH_WEST_PORT,
	NORTH_EAST_PORT,
	SOUTH_EAST_PORT,
	SOUTH_WEST_PORT,
#endif
	NUM_OF_PORTS
};


#define MAX_ZERO_COUNT						(1<<ZERO_WIDTH)
#define MAX_WEIGHT_VALUE					(1<<(DATA_WIDTH>>1))
#define MAX_FEATURE_VALUE					(1<<(DATA_WIDTH>>1))


//macro declare
#define CEIL_DIV(x,y)						(((x)+(y)-1)/(y))

#define NBITS2(n)							((n & 2) ? 1 : 0)
#define NBITS4(n)							((n & (0xC)) ? (2 + NBITS2(n >> 2)) : (NBITS2(n)))
#define NBITS8(n)							((n & 0xF0) ? (4 + NBITS4(n >> 4)) : (NBITS4(n)))
#define NBITS16(n)							((n & 0xFF00) ? (8 + NBITS8(n >> 8)) : (NBITS8(n)))
#define NBITS32(n)							((n & 0xFFFF0000) ? (16 + NBITS16(n >> 16)) : (NBITS16(n)))
//n用原码表示所需要的比特数，用补码表示需要+1
#define NBITS(n)							((n) == 0 ? 1 : NBITS32((n)) + 1)

#define MAX_FEATURE_ROW_CHUNK_NUM			CEIL_DIV(MAX_FEATURE_DIMENSION,MAX_FEATURES_ROW_PER_CHUNK)
#define MAX_FEATURE_COL_CHUNK_NUM			CEIL_DIV(MAX_FEATURE_DIMENSION,MAX_FEATURES_COL_PER_CHUNK)
#define MAX_NUM_OF_FEATURE_PER_CHUNK		(MAX_FEATURES_ROW_PER_CHUNK*MAX_FEATURES_COL_PER_CHUNK)
#define MAX_FEATURE_CHUNK_NUM				(MAX_FEATURE_ROW_CHUNK_NUM*MAX_FEATURE_COL_CHUNK_NUM)

#define MAX_OUTPUT_CHANNEL_CHUNK_NUM		CEIL_DIV(MAX_OUTPUT_CHANNEL_NUM,MAX_OUTPUT_CHANNEL_GROUP_SIZE)
#define MAX_NUM_OF_WEIGHTS_PER_CHUNK		(MAX_OUTPUT_CHANNEL_CHUNK_NUM*MAX_KERNEL_SIZE*MAX_KERNEL_SIZE)

#define NUM_OF_REQUESTS						(F*I+NUM_OF_PORTS)
#define NUM_OF_RESOURCES					(MAX_FEATURES_COL_PER_CHUNK*MAX_FEATURES_ROW_PER_CHUNK)


typedef ap_uint<1> valid_type;
typedef ap_uint<1> grant_type;
typedef ap_uint<1> carrier_type;
typedef ap_uint<1> request_type;
typedef ap_uint<1> priority_type;

typedef ap_uint<ZERO_WIDTH> zero_t;
typedef ap_int<DATA_WIDTH> weight_t;
typedef ap_int<DATA_WIDTH> feature_t;
typedef ap_int<2*DATA_WIDTH> product_t;
typedef ap_uint<NBITS(NUM_OF_PEs)> pe_t;
typedef ap_uint<NBITS(MAX_STRIDE)> stride_t;
typedef ap_int<NBITS(MAX_KERNEL_SIZE)>	kernel_t;
typedef ap_uint<NBITS(MAX_NUM_OF_LAYERS)> numlayers_t;
typedef ap_int<NBITS(MAX_FEATURE_DIMENSION)+1> dimension_t;
typedef ap_int<NBITS(MAX_FEATURE_DIMENSION)+1> feature_index_t;
typedef ap_uint<NBITS(MAX_INPUT_CHANNEL_NUM)> input_channel_t;
typedef ap_uint<NBITS(MAX_OUTPUT_CHANNEL_NUM)> output_channel_t;
typedef ap_int<NBITS(MAX_NUM_OF_WEIGHTS_PER_CHUNK)+1> weight_index_t;


#endif
