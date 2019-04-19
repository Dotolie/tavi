#ifndef _RMBASIC_H
#define _RMBASIC_H




typedef unsigned char		RMuint8;
typedef unsigned short		RMuint16;
typedef unsigned long		RMuint32;
typedef char				RMint8;
typedef short int			RMint16;
typedef long int			RMint32;

#ifdef _WIN32
typedef unsigned __int64	RMuint64;
typedef __int64				RMint64;
typedef unsigned char		RMbool;
#else
typedef unsigned long long	RMuint64;
typedef long long			RMint64;
typedef int					RMbool;
#endif

#endif
