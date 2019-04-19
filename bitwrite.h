/************************************************************
	Copyright (C) 2002

	Sigma Designs, Inc. All Rights Reserved
	Proprietary and Confidential

    Author: David Weiguo Zheng
			<david_zheng@sdesigns.com>

    File Name: bitWrite.h

	Purpose: All functions to handle bit writing

**************************************************************/

#ifndef _BITWRITE_H_
#define _BITWRITE_H_

typedef struct
{
	i32 pos;
	u32 reg;

	u32 * buf;
	u32 * ptr;
}BitWriter;

static void bw_init(BitWriter * const bw, void * const pointer);
static u32 bw_length(BitWriter * const bw);
static void bw_put1(BitWriter * const bw, const u32 bit);
static void bw_put(BitWriter * const bw, const u32 value, const i32 size);
static void bw_pad(BitWriter * const bw);


/* initialize bitstream structure */

static void bw_init(BitWriter * const bw, void * const pointer)
{
    bw->reg = bw->pos = 0;
	bw->buf = (u32 *)pointer;
	bw->ptr = bw->buf;
}

/* bitstream length (unit bits) */

static u32 bw_length(BitWriter * const bw)
{
	u32 len = (u32) bw->ptr - (u32) bw->buf;

    if (bw->pos)
    {
		u32 ui32 = bw->reg;
		ui32 = (ui32<<16) | (ui32>>16);

		*bw->ptr = ((ui32<<8)&0xff00ff00) | ((ui32>>8)&0xff00ff);

		len += ((bw->pos + 7) >> 3);
    }

	return len;
}


/* write single bit to bitstream */

static void bw_put1(BitWriter * const bw, const u32 bit)
{
    if (bit)
		bw->reg |= (0x80000000 >> bw->pos);

    bw->pos ++;

    if (bw->pos > 31)
    {
		u32 ui32 = bw->reg;
		ui32 = (ui32<<16) | (ui32>>16);
		bw->reg = 0;
		*bw->ptr++ = ((ui32<<8)&0xff00ff00) | ((ui32>>8)&0xff00ff);
		
		bw->pos -= 32;
    }

}


/* put n bits to bitwriter */
static void bw_put(BitWriter * const bw, const u32 value, const i32 size)
{
  if(size)
  {
	i32 shift = 32 - bw->pos - size;

	if (shift>0) {		// there is more than enough bits available to insert the data
		bw->reg |= (value << shift);
		bw->pos += size;
		return;
	} 
	else
	{
	   if(shift)    // no enough bits to insert data	
	   {
		// 32bit cache buffer full, need to flush the cache and update again
		i32 room = 32-bw->pos;
		i32 first_shift = size-room;

		u32 ui32 = bw->reg | (value >> first_shift);		
		ui32 = (ui32<<16) | (ui32>>16);
		room = 32-first_shift;
		*bw->ptr++ = ((ui32<<8)&0xff00ff00) | ((ui32>>8)&0xff00ff);
		bw->reg = (value << room);		
		bw->pos = first_shift;
		return;
	   }
	   else//	if(shift == 0)
	   {
		u32 ui32 = bw->reg | (value << shift);

		ui32 = (ui32<<16) | (ui32>>16);
		
		*bw->ptr++ = ((ui32<<8)&0xff00ff00) | ((ui32>>8)&0xff00ff);

		bw->pos = 0;
		bw->reg = 0;

		return;
	   }

	}
  }
}


/* pad bitwriter bitstream to align byte boundary */
static void bw_pad(BitWriter * const bw)
{
	i32 remainder = bw->pos & 7;

    if (remainder)
    {		
		if(remainder < 8)
		{
		bw_put1(bw, 0);
		remainder ++;
		}

		// still needs padding
		if(remainder < 8)
		{
		  u32 padding_bits = 0xff >> remainder;
		  bw_put(bw, padding_bits, 8-remainder);
		}

    }

}

#endif /* _BITWRITE_H_ */
