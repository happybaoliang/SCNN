#ifndef __CPU_TOP_H__
#define __CPU_TOP_H__


#define F	3
#define I	4


#define CEIL_DIV(x,y)		(((x)+(y)-1)/(y))


#define KERNEL_SIZE						3
#define PADDING							(KERNEL_SIZE/2)
#define STRIDE							1

#define INPUT_CHANNEL_NUM				1
#define OUTPUT_CHANNEL_NUM				2

#define MAX_ZERO_COUNT					16

#define MAX_WEIGHT_VALUE				256

#define MAX_FEATURE_VALUE				256
#define INPUT_FEATURE_WIDTH				4
#define INPUT_FEATURE_HEIGHT			4

#define OUTPUT_FEATURE_WIDTH			CEIL_DIV(INPUT_FEATURE_WIDTH+2*PADDING-KERNEL_SIZE+1,STRIDE)
#define OUTPUT_FEATURE_HEIGHT			CEIL_DIV(INPUT_FEATURE_HEIGHT+2*PADDING-KERNEL_SIZE+1,STRIDE)

#define HORIZONTAL_FEATURE_CHUNK_NUM	1
#define VERTICAL_FEATURE_CHUNK_NUM		1
#define FEATURE_CHUNK_NUM				(HORIZONTAL_FEATURE_CHUNK_NUM*VERTICAL_FEATURE_CHUNK_NUM)
#define FEATURES_ROW_PER_CHUNK			CEIL_DIV(INPUT_FEATURE_HEIGHT,VERTICAL_FEATURE_CHUNK_NUM)
#define FEATURES_COL_PER_CHUNK			CEIL_DIV(INPUT_FEATURE_WIDTH,HORIZONTAL_FEATURE_CHUNK_NUM)
#define MAX_NUM_OF_FEATURE_PER_CHUNK	FEATURES_ROW_PER_CHUNK*FEATURES_COL_PER_CHUNK

#define NUM_OF_PEs						FEATURE_CHUNK_NUM

#define WEIGHT_CHUNK_NUM				2
#define WEIGHT_CHUNK_SIZE				(OUTPUT_CHANNEL_NUM/WEIGHT_CHUNK_NUM)
#define MAX_NUM_OF_WEIGHTS_PER_CHUNK	(WEIGHT_CHUNK_SIZE*KERNEL_SIZE*KERNEL_SIZE)


#if 1
typedef int feature_type;
typedef int weight_type;
typedef int zeros_type;
typedef int size_type;
typedef int offset_type;
typedef int pe_coord_type;
typedef int pe_id_type;
typedef int channel_type;
typedef int xcoord_type;
typedef int ycoord_type;
typedef int ocoord_type;
#else

#endif


#endif
