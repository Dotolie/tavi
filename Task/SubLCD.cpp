#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <Lib/LibText.h>

#include "SubLCD.h"

#define pPIO0_DIR							( volatile int* )( 0x00500608 )
#define pPIO0_DATA							( volatile int* )( 0x00500604 )

#define RS_LOW		( *pPIO0_DATA = 0x00100000 )
#define RS_HIGH		( *pPIO0_DATA = 0x00100010 )

/* lcd command */
#define DISPLAY_ONOFF			0xAE
#define INITIAL_DISPLAY_LINE	0x40
#define LCD_BIAS					0xA2
#define ADC_SELECT				0xA0
#define REVERSE_DISPLAY			0xA6
#define REGULATOR_RES_SEL		0x20
#define SET_COLUMN1			0x10
#define SET_COLUMN0			0x00
#define SET_PAGE				0xB0
#define ENTIRE_DISPLAY_ONOFF	0xA4
#define POWER_CONTROL			0x28
#define REF_VOLTAGE_SELECT		0x81
#define SHL_SELECT				0xC0
#define RESET					0xE2

#define PLAY_ICON_X				2
#define PLAY_ICON_Y				4

#define LOGO_ICON_WIDTH		57
#define LOGO_ICON_HEIGHT		10

extern CTextEngine_BDF* pT;

// Flash memory address
#define LCDBUF					*(( volatile char* )(0x400000-4) )

// Local variables

static char TAVI_Logo[] = {
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 
};

#define SHUTDOWN_ICON_WIDTH		12
#define SHUTDOWN_ICON_HEIGHT		12
static char ShutdownIcon[] = {
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 
	0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 
	0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 
	0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};


static struct {
	int xres;
	int yres;
	int xvirt;
	int yvirt;
	int planes;
	char* fb;
} SubLCD_Property;

static void SubLCD_Cmd( char data )
{
	RS_LOW;
	LCDBUF = data;
	RS_HIGH;
}

static void SubLCD_SetPage( int page )
{
	SubLCD_Cmd( SET_PAGE | page );
}

static void SubLCD_SetColumn( int col )
{
	if( col < 0 || col > SubLCD_Property.xres ) return;
	// The column is reversed. and width is 128 not 132.
	// So, the start address of column is 3 not 0.
	char lo = (col&0x0f) | 0x03; 
	char hi = (col&0xf0)>>4;

	SubLCD_Cmd( SET_COLUMN1|hi );
	SubLCD_Cmd( SET_COLUMN0|lo );
}

void SubLCD_Contrast( int contrast )
{
	if( contrast > 63 || contrast < 0 ) return;
	SubLCD_Cmd( contrast );
}

void SubLCD_ON( bool onoff )
{
	SubLCD_Cmd( DISPLAY_ONOFF | (int)onoff );
}

void SubLCD_Flush( int plane )
{
	if( plane < 0 || plane > SubLCD_Property.planes ) return;

	char* pBase = SubLCD_Property.fb+plane*SubLCD_Property.xres*SubLCD_Property.yres;
	char tmp;
	int i, j;

	#if 0 // for debugging
	for( i=0; i<SubLCD_Property.yres; i++ ) {
		for( j=0; j<SubLCD_Property.xres; j++ ) {
			printf( "%d", *(pBase+i*SubLCD_Property.xres+j) );
		}
		printf( "\n" );
	}
	#endif

	SubLCD_SetColumn( 0 );
	SubLCD_SetPage( 0 );
	for( i=0; i<SubLCD_Property.xres; i++ ) {
		tmp = 0;
		for( j=0; j<8; j++ ) {
			tmp |= *(pBase+j*SubLCD_Property.xres+i) << j;
		}
		LCDBUF = tmp;
		//printf( "tmp: 0x%lx\n", tmp );
	}

	pBase += SubLCD_Property.xres*SubLCD_Property.yres/2;
	
	SubLCD_SetColumn( 0 );
	SubLCD_SetPage( 1 );
	
	for( i=0; i<SubLCD_Property.xres; i++ ) {
		tmp = 0;
		for( j=0; j<8; j++ ) {
			tmp |= *(pBase+j*SubLCD_Property.xres+i) << j;
		}
		LCDBUF = tmp;
	}
}

char* SubLCD_GetPlaneBuffer( int plane )
{
	// last plane reserved
	if( plane < 0 || plane > SubLCD_Property.planes-1 ) return NULL;
	char* pBase = SubLCD_Property.fb + plane*SubLCD_Property.xres*SubLCD_Property.yres;

	return pBase;
}

void SubLCD_Reverse( bool onoff )
{
	SubLCD_Cmd( REVERSE_DISPLAY | (int)onoff );
}

void SubLCD_BitBlt( char* pDst, int dx, int dy, char* pSrc, int sx, int sy, int w, int h )
{
	int dw;
	int dh;
	int dstride, sstride;
	int i;
	int cw;
	char* pd, *ps;

	if( pDst == NULL || pSrc == NULL ) return;
	
	pd = pDst;
	ps = pSrc;
	cw = 1;
	dw = SubLCD_Property.xres - dx;
	dh = 16 - dy;
	// clip area
	if( dw < w ) w = dw;
	if( dh < h ) h = dh;

	dstride = SubLCD_Property.xres;
	sstride = w;
	pd += dx*cw + dy*dstride;
	ps += sx*cw + sy*sstride;
	for( i=0; i<h; i++ ) {
		memcpy( pd, ps, w*cw );
		pd += dstride;
		ps += sstride ;
	}

}

void SubLCD_Init( void )
{
	RS_HIGH;
	// output direction
	*pPIO0_DIR = 0x00100010;

	// RESET
//	SubLCD_Cmd( RESET );

	// Horizontal reverse
	SubLCD_Cmd( ADC_SELECT | 0x01 );
	// Vertical reverse
	SubLCD_Cmd( SHL_SELECT | 0x08 );
	// LCD Bias: Duty ratio 1/33
	SubLCD_Cmd( LCD_BIAS | 0x00 );

	// Voltage converter ON
	SubLCD_Cmd( POWER_CONTROL | 0x04 );
	usleep( 10000 ); // waiting for 10ms
	// Voltage Regulator ON
	SubLCD_Cmd( POWER_CONTROL | 0x06 );
	usleep( 10000 );
	// Regulator Follower ON
	SubLCD_Cmd( POWER_CONTROL | 0x07 );
	usleep( 10000 );

	// Regulator resistor select
	SubLCD_Cmd( REGULATOR_RES_SEL | 0x04 );
	// Set Reference voltage mode
	SubLCD_Cmd( REF_VOLTAGE_SELECT );
	SubLCD_Contrast( 63 );
	// waiting for stablizing the LCD power level
	usleep( 10000 );
	// line address
	SubLCD_Cmd( INITIAL_DISPLAY_LINE | 0x30 );


	int i;	
	SubLCD_Property.xres = 129;
	SubLCD_Property.yres = 16;
	SubLCD_Property.xvirt = 129;
	SubLCD_Property.yvirt = 64;
	SubLCD_Property.planes = SubLCD_Property.yvirt / SubLCD_Property.yres;
	SubLCD_Property.fb = ( char* )malloc( SubLCD_Property.xvirt*SubLCD_Property.yvirt );
	for( i=0; i<SubLCD_Property.xvirt*SubLCD_Property.yvirt; i++ ) 
		SubLCD_Property.fb[i] = 0x00;

	// Clear screen
	SubLCD_SetPage( 0 );
	SubLCD_SetColumn( 0 );
	for( i=0; i<SubLCD_Property.xres; i++ ) LCDBUF = 0x00;
	SubLCD_SetPage( 1 );
	SubLCD_SetColumn( 0 );
	for( i=0; i<SubLCD_Property.xres; i++ ) LCDBUF = 0x00;
	SubLCD_ON( true );
}

void SubLCD_Exit( void )
{
	SubLCD_Clear();
	SubLCD_ON( false );
	free( SubLCD_Property.fb );
}

void SubLCD_ShowLOGO( void )
{
	char* pPlane = SubLCD_GetPlaneBuffer( 0 );
	int x = (SubLCD_Property.xres - LOGO_ICON_WIDTH)/2;
	int y = (SubLCD_Property.yres - LOGO_ICON_HEIGHT)/2;
	SubLCD_BitBlt( pPlane, x, y, TAVI_Logo, 0, 0, LOGO_ICON_WIDTH, LOGO_ICON_HEIGHT );
}

void SubLCD_ShowShutDown( void )
{
	char* pPlane = SubLCD_GetPlaneBuffer( 0 );
	SubLCD_ClearArea( 0, 0, SubLCD_Property.xres, SubLCD_Property.yres, 0 );
	int x = (SubLCD_Property.xres-SHUTDOWN_ICON_WIDTH)/2;
	int y = (SubLCD_Property.yres-SHUTDOWN_ICON_HEIGHT)/2;
	SubLCD_BitBlt( pPlane, x, y, ShutdownIcon, 0, 0, 
		SHUTDOWN_ICON_WIDTH, SHUTDOWN_ICON_HEIGHT );
}

void SubLCD_Clear( void )
{
	int i;
	
	// Clear screen
	SubLCD_SetPage( 0 );
	SubLCD_SetColumn( 0 );
	for( i=0; i<SubLCD_Property.xres; i++ ) LCDBUF = 0x00;
	SubLCD_SetPage( 1 );
	SubLCD_SetColumn( 0 );
	for( i=0; i<SubLCD_Property.xres; i++ ) LCDBUF = 0x00;
}

void SubLCD_ClearArea( int x, int y, int w, int h, int planeno )
{
	if( planeno < 0 || planeno > 2 ) return;
	
	char* pDummyPlane = SubLCD_GetPlaneBuffer( 3 );
	char* pPlane = SubLCD_GetPlaneBuffer( planeno );
	if( pDummyPlane == NULL ) return;

	if( x > SubLCD_Property.xres ) return;
	if( y > SubLCD_Property.yres ) return;

	if( x+w > SubLCD_Property.xres ) w = x+w-SubLCD_Property.xres;
	if( y+h > SubLCD_Property.yres ) h = y+h-SubLCD_Property.yres;
	SubLCD_BitBlt( pPlane, x, y, pDummyPlane, 0, 0, w, h );
}

int SubLCD_XResolution( void )
{
	return SubLCD_Property.xres;
}

int SubLCD_YResolution( void )
{
	return SubLCD_Property.yres;
}

#if SUB_LCD_TEST
int main( int argc, char** argv )
{
	SubLCD_Init();

	char* pPlane = SubLCD_GetPlaneBuffer( 0 );
	SubLCD_BitBlt( pPlane, 111, 3, BatteryIcon+6*17*10, 0, 0, BATTERY_ICON_WIDTH, BATTERY_ICON_HEIGHT );
	SubLCD_Flush( 0 );

	return 0;
}
#endif /* SUB_LCD_TEST */
