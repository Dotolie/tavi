#include "jpeg.h"

#if 0
static void debug_break (void)
{
}
#define ASSERT(exp)					((void)((exp)?1:(printf ("ASSERT failed: line %d, file %s\n", __LINE__,__FILE__), debug_break(), 0)))
#define DEBUGMSG(cond,printf_exp)	((void)((cond)?(printf printf_exp),1:0))
#else
#define ASSERT(exp)
#define DEBUGMSG(cond,printf_exp)
#endif

static void jpeg_callback_src (j_decompress_ptr cinfo, int handle, JPEG_CALLBACK_TABLE *infile);
static jmp_buf jpegerror_jmp_buf;
static struct jpeg_error_mgr jerr;
static char last_jpeg_error_string[JMSG_LENGTH_MAX];

static void myjpeg_error_exit (j_common_ptr cinfo)
{
	DEBUGMSG (1, ("JPEG decode error: 0x%08lx\n", cinfo->err->msg_code));
	(*cinfo->err->format_message) (cinfo, last_jpeg_error_string);
	// display error message
	(*cinfo->err->output_message) (cinfo);
	// Let the memory manager delete any temp files before we die
	jpeg_destroy (cinfo);
	// jump
	longjmp (jpegerror_jmp_buf, cinfo->err->msg_code);
}

char *jpeg_get_last_error (void)
{
	return last_jpeg_error_string;
}

typedef struct tagjpeg_progress_mgr_ex
{
	struct jpeg_progress_mgr progress;
	JPEG_CALLBACK_TABLE *jpeg_callback_table;
} jpeg_progress_mgr_ex;

static void jpeg_progress_monitor (j_common_ptr cinfo)
{
	JPEG_CALLBACK_TABLE *jpeg_callback_table;
	int percent_done = (int) (cinfo->progress->pass_counter*100L/cinfo->progress->pass_limit);
	jpeg_callback_table = ((jpeg_progress_mgr_ex *)cinfo->progress)->jpeg_callback_table;
	jpeg_callback_table->jpeg_progress (percent_done, jpeg_callback_table->context);
}

int jpeg_real_width;
int jpeg_real_height;
int decode_jpeg_image_420 (char *filename, unsigned char *pY, unsigned char *pUV, unsigned long lengthY, unsigned long lengthUV, int *outW, int *outH, JPEG_CALLBACK_TABLE *CallbackTable)
{	
	jpeg_progress_mgr_ex progress;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int handle, err, yuvHeight, yuvWidth, width, height;
	int error=0;

	DEBUGMSG (1, ("decode_jpeg_image_420: %s\n", filename));

	memset (&cinfo, 0, sizeof(cinfo));
	handle = CallbackTable->fopen (filename, 0);
	if (handle == 0)
	{
		DEBUGMSG (1, ("decode_jpeg_image_420: could not open %s\n", filename));
		return -1;
	}

	cinfo.err = jpeg_std_error (&jerr);
	if ((err = setjmp (jpegerror_jmp_buf)))
	{
		DEBUGMSG (1, ("error occured during decode\n"));
		jpeg_destroy_decompress (&cinfo);
		CallbackTable->fclose (handle, 0);
		return JPEG_ERROR_DECODE;
	}

	// override error_exit function to use setjmp/longjmp
	cinfo.err->error_exit = myjpeg_error_exit; 
	jpeg_create_decompress (&cinfo);
	jpeg_callback_src (&cinfo, handle, CallbackTable);
	jpeg_read_header (&cinfo,TRUE);
	if (cinfo.progressive_mode)
	{
		DEBUGMSG (1, ("progressive JPEGs not supported.\n"));
		error = JPEG_ERROR_PROGRESSIVE;
		goto jpeg_420_error;
	}

	// set decompression parameters here
	if ((cinfo.jpeg_color_space == JCS_GRAYSCALE) ||
		(cinfo.jpeg_color_space == JCS_YCbCr)) 
	{
		cinfo.out_color_space = cinfo.jpeg_color_space;
	} 
	else 
	{
		DEBUGMSG (1, ("only gray scale and yuv jpegs supported.\n"));
		error = JPEG_ERROR_NOTSUPPORTED_COLORSPACE;
		goto jpeg_420_error;
	}
	cinfo.dct_method = JDCT_IFAST;
	cinfo.scale_num = 1;
	cinfo.scale_denom = 1;
	// XXX pal can be implemented better
	// scale if jpeg is > 720x480
	width = cinfo.image_width;
	height = cinfo.image_height;
	jpeg_real_width = width;
	jpeg_real_height = height;
	while ((width > 720) || (height > 480))
	{
		cinfo.scale_denom *= 2;
		width /= 2;
		height /= 2;
	}
	cinfo.quantize_colors = FALSE;
	jpeg_start_decompress (&cinfo);

	*outW = cinfo.output_width;
	*outH = cinfo.output_height;
	DEBUGMSG (1, ("decode_jpeg_image_420: %d x %d (out)\n", *outW, *outH));

	yuvHeight = (cinfo.output_height + 15) & ~0xF;
	yuvWidth  = (cinfo.output_width + 15) & ~0xF;
	DEBUGMSG (1, ("decode_jpeg_image_420: %d x %d (yuv)\n", yuvWidth, yuvHeight));

	progress.jpeg_callback_table = CallbackTable;
    progress.progress.progress_monitor = jpeg_progress_monitor;
    cinfo.progress = (struct jpeg_progress_mgr *)&progress;

	jpeg_read_image_420 (&cinfo, pY, pUV);

	jpeg_destroy_decompress (&cinfo);
	CallbackTable->fclose (handle, 0);
	return 0;

jpeg_420_error:
	jpeg_destroy_decompress (&cinfo);
	CallbackTable->fclose (handle, 0);
	return error;
}			 

#if 0
int decode_jpeg_init (char *filename, JPEG_DECODER *pJpegDecoder)
{		
	int err, row_stride;

	DEBUGMSG (1, ("decode_jpeg_init: %s\n", filename));
	
	memset (&pJpegDecoder->cinfo, 0, sizeof(pJpegDecoder->cinfo));
	pJpegDecoder->initialized = 0;
	pJpegDecoder->handle = pJpegDecoder->CallbackTable.fopen (filename, 0);
	if (pJpegDecoder->handle == 0)
	{
		DEBUGMSG (1, ("decode_jpeg_init: could not open %s\n", filename));
		return -1;
	}

	if ((err = setjmp (jpegerror_jmp_buf)))
	{
		goto decode_jpeg_init_error;
	}
	// override error_exit function to use setjmp/longjmp
	pJpegDecoder->cinfo.err = jpeg_std_error (&jerr);
	pJpegDecoder->cinfo.err->error_exit = myjpeg_error_exit; 	

	DEBUGMSG (1, ("decode_jpeg_init: jpeg_create_decompress\n"));
	jpeg_create_decompress (&pJpegDecoder->cinfo);
	jpeg_callback_src (&pJpegDecoder->cinfo, pJpegDecoder->handle, &pJpegDecoder->CallbackTable);
	jpeg_read_header (&pJpegDecoder->cinfo, TRUE);
	if (pJpegDecoder->cinfo.progressive_mode)
	{
		DEBUGMSG (1, ("decode_jpeg_init: progressive JPEGs not supported.\n"));
		goto decode_jpeg_init_error;
	}

	// set decompression parameters here
	DEBUGMSG (1, ("decode_jpeg_init: set decompression parameters\n"));
	if ((pJpegDecoder->cinfo.jpeg_color_space == JCS_GRAYSCALE) ||
		(pJpegDecoder->cinfo.jpeg_color_space == JCS_YCbCr)) 
	{
		pJpegDecoder->cinfo.out_color_space = pJpegDecoder->cinfo.jpeg_color_space;
	} 
	else 
	{
		DEBUGMSG (1, ("only gray scale and yuv jpegs supported.\n"));
		goto decode_jpeg_init_error;
	}
	pJpegDecoder->cinfo.dct_method = JDCT_IFAST;
	pJpegDecoder->cinfo.scale_num = 1;
	pJpegDecoder->cinfo.scale_denom = 1;
	pJpegDecoder->cinfo.quantize_colors = FALSE;	
	DEBUGMSG (1, ("jpeg_start_decompress ...\n"));
	jpeg_start_decompress (&pJpegDecoder->cinfo);

	row_stride = pJpegDecoder->cinfo.output_width * pJpegDecoder->cinfo.output_components;
	if (row_stride > pJpegDecoder->length)
	{
		DEBUGMSG (1, ("jpeg output buffer too small.\n"));
		goto decode_jpeg_init_error;
	}
	pJpegDecoder->outW = (pJpegDecoder->cinfo.output_width + 15) & ~0xF;
	pJpegDecoder->outH = (pJpegDecoder->cinfo.output_height + 15) & ~0xF;
	pJpegDecoder->initialized = 0xdeadbeef;
	return 0;

decode_jpeg_init_error:
	jpeg_destroy_decompress (&pJpegDecoder->cinfo);
	pJpegDecoder->CallbackTable.fclose (pJpegDecoder->handle, 0);
	return -1;
}

int decode_jpeg_exit (JPEG_DECODER *pJpegDecoder)
{
	if (pJpegDecoder->initialized == 0xdeadbeef)
	{
		jpeg_destroy_decompress (&pJpegDecoder->cinfo);
		pJpegDecoder->CallbackTable.fclose (pJpegDecoder->handle, 0);
		pJpegDecoder->initialized = 0;
		return 0;
	}
	return -1;
}

int decode_jpeg_schedule (JPEG_DECODER *pJpegDecoder)
{
	int err;
	if ((err = setjmp (jpegerror_jmp_buf)))
		return 0;
	if (pJpegDecoder->initialized == 0xdeadbeef)
	{
		JSAMPARRAY array[3];
		JSAMPROW scanlineY[16];
		JSAMPROW scanlineU[16];
		JSAMPROW scanlineV[16];
		JSAMPLE *pY = pJpegDecoder->pYUV;
		JSAMPLE *pU = pY + (pJpegDecoder->cinfo.output_width * 16);
		JSAMPLE *pV = pU + (pJpegDecoder->cinfo.output_width * 16);
		if ((pJpegDecoder->length < (pJpegDecoder->cinfo.output_width * 48)) ||
			(pJpegDecoder->cinfo.max_v_samp_factor * pJpegDecoder->cinfo.min_DCT_scaled_size > 16))
		{
			DEBUGMSG (1, ("*** jpeg output buffer too small: %d, %d\n",
				(int)pJpegDecoder->cinfo.output_width * 48, 
				(int)pJpegDecoder->cinfo.max_v_samp_factor * pJpegDecoder->cinfo.min_DCT_scaled_size));
			jpeg_destroy_decompress (&pJpegDecoder->cinfo);
			pJpegDecoder->CallbackTable.fclose (pJpegDecoder->handle, 0);
			pJpegDecoder->initialized = 0;
			return 0;
		}
		int i;
		for (i=0; i<16; i++)
		{
			scanlineY[i] = pY;
			pY += pJpegDecoder->cinfo.output_width;
		}
		for (i=0; i<16; i++)
		{
			scanlineU[i] = pU;
			pU += pJpegDecoder->cinfo.output_width;
		}
		for (i=0; i<16; i++)
		{
			scanlineV[i] = pV;
			pV += pJpegDecoder->cinfo.output_width;
		}
		array[0] = scanlineY;
		array[1] = scanlineU;
		array[2] = scanlineV;

		if (pJpegDecoder->cinfo.output_scanline < pJpegDecoder->cinfo.output_height)
		{
			jpeg_read_raw_data (&pJpegDecoder->cinfo, array, pJpegDecoder->cinfo.max_v_samp_factor * pJpegDecoder->cinfo.min_DCT_scaled_size);
		}
		else
		{
			jpeg_destroy_decompress (&pJpegDecoder->cinfo);
			pJpegDecoder->CallbackTable.fclose (pJpegDecoder->handle, 0);
			pJpegDecoder->initialized = 0;
			return 0;
		}
		return 1;
	}
	return 0;
}
#endif

///////////////////////////////////////////////////////////////
/*
 * jdatasrc.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains decompression data source routines for the case of
 * reading JPEG data from a file (or any stdio stream).  While these routines
 * are sufficient for most applications, some will want to use a different
 * source manager.
 * IMPORTANT: we assume that fread() will correctly transcribe an array of
 * JOCTETs from 8-bit-wide elements on external storage.  If char is wider
 * than 8 bits on your machine, you may need to do some tweaking.
 */

/* this is not a core library module, so it doesn't define JPEG_INTERNALS */
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"


/* Expanded data source object for stdio input */

typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  JPEG_CALLBACK_TABLE *infile;		/* source stream */
  JOCTET * buffer;		/* start of buffer */
  boolean start_of_file;	/* have we gotten any data yet? */
  int handle;
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

#define INPUT_BUF_SIZE  16*4096	/* choose an efficiently fread'able size */


/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  size_t nbytes;

//  nbytes = JFREAD(src->infile, src->buffer, INPUT_BUF_SIZE);
  nbytes = src->infile->fread (src->handle, src->buffer, INPUT_BUF_SIZE, src->infile->context);

  if (nbytes <= 0) {
    if (src->start_of_file)	/* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}


/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}


/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

static void jpeg_callback_src (j_decompress_ptr cinfo, int handle, JPEG_CALLBACK_TABLE *infile)
{
  my_src_ptr src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_stdio_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  SIZEOF(my_source_mgr));
    src = (my_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  INPUT_BUF_SIZE * SIZEOF(JOCTET));
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->infile = infile;
  src->handle = handle;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}

