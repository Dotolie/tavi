//********************** Copyright(C) New Media Life Inc. *********************
//
// Description : util.cpp
// 
// $Source: /home/cvsroot/TAVI020/tavi/util.cpp,v $
// $Revision: 1.1.1.1 $
// $Date: 2006/02/22 12:33:31 $
// $Author: zstein $
//
//*****************************************************************************
// $Log: util.cpp,v $
// Revision 1.1.1.1  2006/02/22 12:33:31  zstein
// TAVI020 REBUILD
//
// Revision 1.6  2006/02/14 03:20:32  zstein
// - added Execute(...)
//
// Revision 1.5  2005/12/01 03:30:05  zstein
// - changed loadmodule(name) to loadmodule(name, arg).
//
// Revision 1.4  2005/10/13 01:25:21  etnlwind
// Update for Header Info
//
//*****************************************************************************

#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "util.h"

#if 0
static void debug_break (void)
{
	fflush (stdout);
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("***ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp, debug_break(), 0):1))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif


int loadmodule (char *module, char* arg )
{
	pid_t pid;
	char *my_argv[4] = {INSMOD_PROG, module, arg, NULL};
	char *my_envp[1] = {NULL};
	int status;

	if ((pid = vfork()) == 0) 
	{                       
		//this is the child do the execve
		execve (INSMOD_PATH""INSMOD_PROG, my_argv, my_envp);
		_exit (1);
	} 
	else 
	{
		// this is the parent.
		pid = waitpid (pid, &status, 0);
		if (WIFEXITED (status) && (WEXITSTATUS (status) != 0))
		{
			return 1;
		} 
		else if (WIFEXITED (status) && (WEXITSTATUS (status) == 0))
		{
			return 0;
		} 
		else 
		{
			printf ("Seems that an error happened during module loading\n");
			return 1;
		}
	}
	return 0;
}

int gettime_ms (void)
{
	struct timeval tv1;
	struct timezone tz;
	gettimeofday (&tv1, &tz);
//	printf( "gettime_ms( %d, %d)\n", tv1.tv_sec, tv1.tv_usec );
	return tv1.tv_sec * 1000 + tv1.tv_usec/1000;
}

long gettime_sec(void)
{
	struct timeval tv1;
	struct timezone tz;
	gettimeofday (&tv1, &tz);
	return tv1.tv_sec;
}

void ParseVOL (RMuint8 *pBuf, RMuint32 length, RMint32 *pGMC, RMint32 *pQPEL)
{
	int err, marker_bit;

	if (pGMC)
		*pGMC = 0;
	if (pQPEL)
		*pQPEL = 0;

	BitStream bitstream (pBuf, length, &err);
	// video_object_layer_start_code	
	unsigned int startcode = bitstream.getbits (32, &err);
	DEBUGMSG (1, ("video_object_layer_start_code: %08lx\n", startcode));
	ASSERT (startcode == 0x00000120);
	int video_object_layer_verid = -1;
	int random_accessible_vol = bitstream.getbits (1, &err);
	DEBUGMSG (1, ("  random_accessible_vol: %d\n", random_accessible_vol));
	int video_object_type_indication = bitstream.getbits (8, &err);
	DEBUGMSG (1, ("  video_object_type_indication: %d\n", video_object_type_indication));
	int is_object_layer_identifier = bitstream.getbits (1, &err);
	DEBUGMSG (1, ("  is_object_layer_identifier: %d\n", is_object_layer_identifier));
	if (is_object_layer_identifier)
	{
		video_object_layer_verid = bitstream.getbits (4, &err);
		DEBUGMSG (1, ("video_object_layer_verid: %d\n", video_object_layer_verid));
		int video_object_priority = bitstream.getbits (3, &err);
		DEBUGMSG (1, ("video_object_priority: %d\n", video_object_priority));
	}
//// warning here!
	if (video_object_layer_verid == -1)
	{
		DEBUGMSG (1, ("WARNING: video_object_layer_verid was not specified: defaulting to 1\n"));
		video_object_layer_verid = 1;
	}
////////////////
	char *aspect_ratio_info_table[] = 
	{
		"Forbidden", "1:1 (Square)", "12:11 (625-type for 4:3 picture)",
		"10:11 (525-type for 4:3 picture)", "16:11 (625-type stretched for 16:9 picture",
		"40:33 (525-type stretched for 16:9 picture", "Reserved", "Reserved", "Reserved",
		"Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Extended PAR"
	};
	int aspect_ratio_info = bitstream.getbits (4, &err);
	DEBUGMSG (1, ("  aspect ratio: %s\n", aspect_ratio_info_table[aspect_ratio_info]));
	if (aspect_ratio_info == 0xf)
	{
		int par_width = bitstream.getbits (8, &err);
		int par_height = bitstream.getbits (8, &err);
		DEBUGMSG (1, ("  par_width = %d, par_height = %d\n", par_width, par_height));
	}
	int vol_control_parameters = bitstream.getbits (1, &err);
	DEBUGMSG (1, ("  vol_control_parameters: %d\n", vol_control_parameters));
	if (vol_control_parameters)
	{
		int chroma_format = bitstream.getbits (2, &err);
		int low_delay = bitstream.getbits (1, &err);
		int vbv_parameters = bitstream.getbits (1, &err);
		if (vbv_parameters)
		{
			int first_half_bit_rate = bitstream.getbits (15, &err);
			marker_bit = bitstream.getbits (1, &err);
			ASSERT (marker_bit);
			int latter_half_bit_rate = bitstream.getbits (15, &err);
			marker_bit = bitstream.getbits (1, &err);
			ASSERT (marker_bit);
			int first_half_vbv_buffer_size = bitstream.getbits (15, &err);
			marker_bit = bitstream.getbits (1, &err);
			ASSERT (marker_bit);
			int latter_half_vbv_buffer_size = bitstream.getbits (3, &err);
			int first_half_vbv_occupancy = bitstream.getbits (11, &err);
			marker_bit = bitstream.getbits (1, &err);
			ASSERT (marker_bit);
			int latter_half_vbv_occupancy = bitstream.getbits (15, &err);
			marker_bit = bitstream.getbits (1, &err);
			ASSERT (marker_bit);
		}
	}
	int video_object_layer_shape = bitstream.getbits (2, &err);
	char *video_object_layer_shape_table[] =
	{
		"rectangular", "binary", "binary only", "grayscale"
	};
	DEBUGMSG (1, ("  video_object_layer_shape = %s\n", 
		video_object_layer_shape_table[video_object_layer_shape]));
	if ((video_object_layer_shape == 3) && (video_object_layer_verid != 1))
	{
		ASSERT (video_object_layer_verid != -1);
		int video_object_layer_shape_extension = bitstream.getbits (4, &err);
	}
	marker_bit = bitstream.getbits (1, &err);
	DEBUGMSG (1, ("  marker_bit: %d\n", marker_bit));
	ASSERT (marker_bit);
	int vop_time_increment_resolution = bitstream.getbits (16, &err);
	DEBUGMSG (1, ("  vop_time_increment_resolution = %d\n", vop_time_increment_resolution));
	ASSERT (vop_time_increment_resolution);
	marker_bit = bitstream.getbits (1, &err);
	DEBUGMSG (1, ("  marker_bit: %d\n", marker_bit));
	ASSERT (marker_bit);
	int fixed_vop_rate = bitstream.getbits (1, &err);
	DEBUGMSG (1, ("  fixed_vop_rate = %d\n", fixed_vop_rate));
	if (fixed_vop_rate)
	{
		int nbits;
		for (nbits=16; nbits>0; nbits--)
		{
			if ((vop_time_increment_resolution >> nbits) == 1)
				break;
		}
		nbits++;
		int fixed_vop_time_increment = bitstream.getbits (nbits, &err);
		DEBUGMSG (1, ("  fixed_vop_time_increment = %d\n", fixed_vop_time_increment));
	}
	if (video_object_layer_shape != 2)
	{
		if (video_object_layer_shape == 0)
		{
			marker_bit = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  marker_bit: %d\n", marker_bit));
			ASSERT (marker_bit);
			int video_object_layer_width = bitstream.getbits (13, &err);
			DEBUGMSG (1, ("  video_object_layer_width = %d\n", video_object_layer_width));
			marker_bit = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  marker_bit: %d\n", marker_bit));
			ASSERT (marker_bit);
			int video_object_layer_height = bitstream.getbits (13, &err);
			DEBUGMSG (1, ("  video_object_layer_height = %d\n", video_object_layer_height));
			marker_bit = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  marker_bit: %d\n", marker_bit));
			ASSERT (marker_bit);
		}
		int Interlaced = bitstream.getbits (1, &err);
		DEBUGMSG (1, ("  Interlaced = %d\n", Interlaced));
		int obmc_disable = bitstream.getbits (1, &err);
		DEBUGMSG (1, ("  obmc_disable: %d\n", obmc_disable));
		ASSERT (video_object_layer_verid != -1);
		int sprite_enable;
		if (video_object_layer_verid == 1)
		{
			sprite_enable = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  sprite_enable: %d\n", sprite_enable));
		}
		else
		{
			sprite_enable = bitstream.getbits (2, &err);
			DEBUGMSG (1, ("  sprite_enable: %d\n", sprite_enable));
		}
		// sprite_enable = GMC
		DEBUGMSG (1, ("GMC: %d\n", sprite_enable));
		if (pGMC)
			*pGMC = sprite_enable;
		if ((sprite_enable == 1) || (sprite_enable == 2))
		{
			if (sprite_enable != 2)
			{
				int sprite_width = bitstream.getbits (13, &err);	 
				marker_bit = bitstream.getbits (1, &err);
				ASSERT (marker_bit);
				int sprite_height = bitstream.getbits (13, &err);	 
				marker_bit = bitstream.getbits (1, &err);
				ASSERT (marker_bit);
				int sprite_left_coordinate = bitstream.getbits (13, &err);
				marker_bit = bitstream.getbits (1, &err);
				ASSERT (marker_bit);
				int sprite_top_coordinate = bitstream.getbits (13, &err);
				marker_bit = bitstream.getbits (1, &err);
				ASSERT (marker_bit);
			}
			int no_of_sprite_warping_points = bitstream.getbits (6, &err);
			int sprite_warping_accuracy = bitstream.getbits (2, &err);
			int sprite_brightness_change = bitstream.getbits (1, &err);
			if (sprite_enable != 2)
			{
				int low_latency_sprite_enable = bitstream.getbits (1, &err);
			}
		}
		if ((video_object_layer_verid != 1) && (video_object_layer_shape != 0))
		{
			int sadct_disable = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  sadct_disable: %d\n", sadct_disable));
		}
		int not_8_bit = bitstream.getbits (1, &err);
		DEBUGMSG (1, ("  not_8_bit: %d\n", not_8_bit));
		if (not_8_bit)
		{
			int quant_precision = bitstream.getbits (4, &err);
			DEBUGMSG (1, ("  quant_precision: %d\n", quant_precision));
			int bits_per_pixel = bitstream.getbits (4, &err);
			DEBUGMSG (1, ("  bits_per_pixel: %d\n", bits_per_pixel));
		}
		if (video_object_layer_shape == 3)
		{
			int no_gray_quant_update = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  no_gray_quant_update: %d\n", no_gray_quant_update));
			int composition_method = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  composition_method: %d\n", composition_method));
			int linear_composition = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  linear_composition: %d\n", linear_composition));
		}
		int quant_type = bitstream.getbits (1, &err);
		DEBUGMSG (1, ("  quant_type: %d\n", quant_type));
		if (quant_type)
		{
			int load_intra_quant_mat = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  load_intra_quant_mat: %d\n", load_intra_quant_mat));
			if (load_intra_quant_mat)
			{
				int intra_quant_mat_value = bitstream.getbits (8, &err);
				ASSERT (intra_quant_mat_value == 8);
				while (intra_quant_mat_value != 0)
					intra_quant_mat_value = bitstream.getbits (8, &err);
			}
			int load_nonintra_quant_mat = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("  load_nonintra_quant_mat: %d\n", load_nonintra_quant_mat));
			if (load_nonintra_quant_mat)
			{
				int intra_nonquant_mat_value = bitstream.getbits (8, &err);
				ASSERT (intra_nonquant_mat_value);
				while (intra_nonquant_mat_value != 0)
					intra_nonquant_mat_value = bitstream.getbits (8, &err);
			}
			if (video_object_layer_shape == 3)
			{			
				DEBUGMSG (1, ("video_object_layer_shape = grayscale not supported\n"));
				// see page 111 mpeg4 verification model
				ASSERT (0);
				return;
			}					
		}
		if (video_object_layer_verid != 1)
		{
			int quarter_sample = bitstream.getbits (1, &err);
			DEBUGMSG (1, ("QPEL: %d\n", quarter_sample));
			if (pQPEL)
				*pQPEL = quarter_sample;
		}
	}		
}

/* ++

    Execute( const char* program_name, char* const argv[] )

Description:
    Run program.

Arguments:
    program_name: program name path
    argv: arguments for program_name

Return:
    None
    
-- */
void Execute( const char* program_name, char* const argv[] )
{
    pid_t pid;

    pid = vfork();
    if( pid < 0 ) {
        printf( "failed to vfork()..\n" );
    }
    else if( pid == 0 ) { // child
        execvp( program_name, argv ); 		
        _exit( 0 );
    }
}
