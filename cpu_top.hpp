#ifndef __CPU_TOP_H__
#define __CPU_TOP_H__

#include<ap_int.h>


//macro declare
#define CEIL_DIV(x,y)		(((x)+(y)-1)/(y))

#define NBITS2(n) ((n & 2) ? 1 : 0)
#define NBITS4(n) ((n & (0xC)) ? (2 + NBITS2(n >> 2)) : (NBITS2(n)))
#define NBITS8(n) ((n & 0xF0) ? (4 + NBITS4(n >> 4)) : (NBITS4(n)))
#define NBITS16(n) ((n & 0xFF00) ? (8 + NBITS8(n >> 8)) : (NBITS8(n)))
#define NBITS32(n) ((n & 0xFFFF0000) ? (16 + NBITS16(n >> 16)) : (NBITS16(n)))
#define NBITS(n) ((n) == 0 ? 1 : NBITS32((n)) + 1)


//固定IF乘积为３６以后，所有可能的方案中最节省bank的方式是4 x 9或者6 x 6，这两种方案每个PE存储feature和w所用的总bank数为６。
#define F								9
#define I								4

#define NUM_OF_CROSSBAR_PORTS			(I*F)

#define KERNEL_SIZE						3
#define PADDING							(KERNEL_SIZE/2)
#define STRIDE							1

#define INPUT_CHANNEL_NUM				3
#define OUTPUT_CHANNEL_NUM				3

#define MAX_ZERO_COUNT					16

#define MAX_WEIGHT_VALUE				256

#define MAX_FEATURE_VALUE				256
#define INPUT_FEATURE_WIDTH				64
#define INPUT_FEATURE_HEIGHT			64

#define OUTPUT_FEATURE_WIDTH			CEIL_DIV(INPUT_FEATURE_WIDTH+2*PADDING-KERNEL_SIZE+1,STRIDE)
#define OUTPUT_FEATURE_HEIGHT			CEIL_DIV(INPUT_FEATURE_HEIGHT+2*PADDING-KERNEL_SIZE+1,STRIDE)

//当PE数为６４时，ZU9EG的寄存器资源最多只能容纳8x8的分块，这时accumulator占的寄存器资源达到５０％，最大可尝试的分块大小为11 x 11
#define HORIZONTAL_FEATURE_CHUNK_NUM	8
#define VERTICAL_FEATURE_CHUNK_NUM		8
#define FEATURE_CHUNK_NUM				(HORIZONTAL_FEATURE_CHUNK_NUM*VERTICAL_FEATURE_CHUNK_NUM)
#define FEATURES_ROW_PER_CHUNK			CEIL_DIV(INPUT_FEATURE_HEIGHT,VERTICAL_FEATURE_CHUNK_NUM)
#define FEATURES_COL_PER_CHUNK			CEIL_DIV(INPUT_FEATURE_WIDTH,HORIZONTAL_FEATURE_CHUNK_NUM)
#define MAX_NUM_OF_FEATURE_PER_CHUNK	FEATURES_ROW_PER_CHUNK*FEATURES_COL_PER_CHUNK

#define NUM_OF_PEs						FEATURE_CHUNK_NUM

#define OUTPUT_CHANNEL_CHUNK_NUM		3
#define OUTPUT_CHANNEL_CHUNK_SIZE		(OUTPUT_CHANNEL_NUM/OUTPUT_CHANNEL_CHUNK_NUM)
#define MAX_NUM_OF_WEIGHTS_PER_CHUNK	(OUTPUT_CHANNEL_CHUNK_SIZE*KERNEL_SIZE*KERNEL_SIZE)


#if 1
typedef int feature_type;
typedef int weight_type;
typedef int zeros_type;
typedef int size_type;
typedef int offset_type;
typedef int pe_coord_type;
typedef int pe_id_type;
typedef int channel_type;
typedef int col_coord_type;
typedef int row_coord_type;
typedef int ocoord_type;
typedef ap_int<64> product_type;
typedef ap_int<NBITS(I*F)> port_type;
#else
typedef ap_int<NBITS(I*F)> port_type;
#endif


#endif
