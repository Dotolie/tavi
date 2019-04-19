#include <stdio.h>
#include <stdlib.h>
#include "vol.h"
#include "bitwrite.h"

#define W_MARKER()	bw_put1(bw, 1)

// all the following start_code(s) are 32 bits
#define VOS_START_CODE		0x000001b0
#define VOS_END_CODE		0x000001b1
#define VOL_START_CODE		0x00000120	
#define USERDATA_START_CODE	0x000001b2
#define GOV_START_CODE		0x000001b3
#define VO_START_CODE		0x000001b5		// Visual Object start code
#define VOP_START_CODE		0x000001b6
#define VO_START_CODE2		0x00000100

#define PROFILE_LEVEL_ID	0xf5			// Level 5 @ ASP

// To save some bits in encoding time_increment
void getMinScale(i32 *scale, i32 *rate)
{
	u32 iscale = *scale;

	if(iscale>1)
	{
		u32 i = iscale;
		u32 irate = *rate;		

		do{
			if(!(irate%i))
			{
				if(!(iscale%i))
				{
				  iscale /= i;
				  irate /= i;
				  i = iscale;
				  continue;
				}
			}
			i--;
		}while(i>1);

		if (irate > 65535)
		{
			iscale = (iscale * 65535 / irate);
			irate = 65535;
		}

		*scale = iscale;
		*rate = irate;
	}

}

/*********************************************
	write vos header
 *********************************************/

static void putVosHeader(BitWriter *bw, i32 flag)
{
	// video_object_sequence_start_code & vo_id
    bw_pad(bw);
	if(flag==0)
	{
	 bw_put(bw, VOS_START_CODE, 32);
     bw_put(bw, PROFILE_LEVEL_ID, 8);			
	}
	else
	{
	 bw_put(bw, VOS_END_CODE, 32);
	}
}

/*********************************************
	write vol header
 *********************************************/

static void putVolHeader(BitWriter * bw, i32 time_resolution, i32 time_res_bits, i32 time_incr, i32 width, i32 height)
{
	// video object_start_code & vo_id
    bw_pad(bw);
	bw_put(bw, VO_START_CODE2, 32);	

	// video_object_layer_start_code & vol_id
	bw_put(bw, VOL_START_CODE, 32);

	bw_put(bw, 0x8, 17);

	W_MARKER();
    
	// time_increment_resolution; 
	bw_put(bw, time_resolution, 16);

	W_MARKER();

	// fixed_vop_rate
	bw_put1(bw, 1);
	bw_put(bw, time_incr, time_res_bits);

	W_MARKER();
	width = ((width + 15)>>4)<<4;	// force multiple of 16
	bw_put(bw, width, 13);			// width
	W_MARKER();
	height = ((height + 15)>>4)<<4;	// force multiple of 16
	bw_put(bw, height, 13);			// height
	W_MARKER();
	
	bw_put(bw, 4, 4);

	bw_put(bw, 0xc, 5);
}

i32 MP43_DSI_Generator(u32 *in_buffer, i32 iScale, i32 iRate, i32 width, i32 height)						   
{	

	i32 time_increase, time_resolution;
	i32 time_res_bits;
	BitWriter bitw;

	BitWriter *bw = &bitw;

	time_increase = iScale;
	time_resolution = iRate;

    getMinScale(&time_increase, &time_resolution);

	{
		u32 bits, fbase = time_resolution;
		
	    for(bits=1; fbase > (1U<<bits); bits++);

		time_res_bits = bits;
	}

	bw_init(bw, in_buffer);

	putVosHeader(bw, 0);

	putVolHeader(bw, time_resolution, time_res_bits, time_increase, width, height);

	bw_pad(bw);

	return(bw_length(bw));
}