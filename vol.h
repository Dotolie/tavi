#ifndef _VOL_H
#define _VOL_H

#include "rmbasic.h"

typedef RMint32		i32;
typedef RMuint32	u32;
typedef RMuint8		u8;

i32 MP43_DSI_Generator (u32 *in_buffer, i32 iScale, i32 iRate, i32 width, i32 height);

#endif
