#ifndef __CPU_TOP_H__
#define __CPU_TOP_H__

#define F	32
#define I	32


#define CEIL_DIV(x,y)		(((x)+(y)-1)/(y))


#define KERNEL_SIZE						3

#define INPUT_CHANNEL_NUM				3
#define OUTPUT_CHANNEL_NUM				3

#define MAX_WEIGHT_VALUE				256
#define WEIGHT_CHUNK_NUM				1
#define WEIGHT_CHUNK_SIZE				CEIL_DIV(OUTPUT_CHANNEL_NUM,WEIGHT_CHUNK_NUM)
#define MAX_NUM_OF_WEIGHTS_PER_CHUNK	(WEIGHT_CHUNK_SIZE*KERNEL_SIZE*KERNEL_SIZE)

#define FEATURE_WIDTH					1024
#define FEATURE_HEIGHT					1024
#define MAX_FEATURE_VALUE				256
#define FEATURE_CHUNK_PER_COL			1
#define FEATURE_CHUNK_PER_ROW			1
#define FEATURES_ROW_PER_CHUNK			CEIL_DIV(FEATURE_WIDTH,FEATURE_CHUNK_PER_ROW)
#define FEATURES_COL_PER_CHUNK			CEIL_DIV(FEATURE_HEIGHT,FEATURE_CHUNK_PER_COL)
#define FEATURE_CHUNK_NUM				(FEATURE_CHUNK_PER_COL*FEATURE_CHUNK_PER_ROW)
#define MAX_NUM_OF_FEATURE_PER_CHUNK	CEIL_DIV(FEATURE_WIDTH*FEATURE_HEIGHT*INPUT_CHANNEL_NUM,FEATURE_CHUNK_NUM)

//not a perfect implementation
#define NUM_OF_PEs						FEATURE_CHUNK_NUM

#define MAX_ZERO_COUNT					16


#if 1
typedef float feature_type;
typedef float weight_type;
typedef int channel_type;
typedef int size_type;
typedef int index_type;
typedef int zeros_type;
#else
#endif


#endif
