#include <stdio.h>
#include <fcntl.h>

#define I2C_MASTER_BASE_ADDRESS		(0x500800)
#define I2C_MASTER_CONFIG			(I2C_MASTER_BASE_ADDRESS + 0x00)
#define I2C_MASTER_CLK_DIV			(I2C_MASTER_BASE_ADDRESS + 0x04)
#define I2C_MASTER_DEV_ADDR			(I2C_MASTER_BASE_ADDRESS + 0x08)
#define I2C_MASTER_ADR				(I2C_MASTER_BASE_ADDRESS + 0x0c)
#define I2C_MASTER_DATAOUT			(I2C_MASTER_BASE_ADDRESS + 0x10)
#define I2C_MASTER_DATAIN			(I2C_MASTER_BASE_ADDRESS + 0x14)
#define I2C_MASTER_STATUS			(I2C_MASTER_BASE_ADDRESS + 0x18)
#define I2C_MASTER_STARTXFER		(I2C_MASTER_BASE_ADDRESS + 0x1c)
#define I2C_MASTER_BYTE_CNT			(I2C_MASTER_BASE_ADDRESS + 0x20)
#define I2C_MASTER_INTEN			(I2C_MASTER_BASE_ADDRESS + 0x24)
#define I2C_MASTER_INT				(I2C_MASTER_BASE_ADDRESS + 0x28)

#define ADDR_DEVICE					0xA0	//adresse of the device (0x36)

#define CLKDIV						3750

static void EM85xx_WriteReg (unsigned int Reg, unsigned int Data)
{
	*((volatile unsigned int *)Reg) = Data;
}

static unsigned int EM85xx_ReadReg (unsigned int Reg)
{
	return *((volatile unsigned int *)(Reg));
}

static int EM85xxI2C__Write (char devaddr, char adr, char* pData, int n)
{
	int i;
	EM85xx_WriteReg (I2C_MASTER_CONFIG, 0xf8);
	EM85xx_WriteReg (I2C_MASTER_CLK_DIV, CLKDIV);	// assume 150mhz clock (375=750/2)
	EM85xx_WriteReg (I2C_MASTER_DEV_ADDR, devaddr >> 1);

	// ready for a command
	// XXX bug: we really should have a timeout here
	while ((EM85xx_ReadReg (I2C_MASTER_STATUS) & 1) == 0);

	// write
	for (i=0; i<n; i++)
	{ 
		EM85xx_WriteReg (I2C_MASTER_ADR, adr++);
		EM85xx_WriteReg (I2C_MASTER_BYTE_CNT, 0);
		EM85xx_WriteReg (I2C_MASTER_DATAOUT, *pData++);
		EM85xx_WriteReg (I2C_MASTER_STARTXFER, 0);
		// XXX bug: we really should have a timeout here
		while ((EM85xx_ReadReg (I2C_MASTER_STATUS) & 2) == 0);
	}	
	return 1;
}

static int EM85xxI2C__Read (char devaddr, char adr, char* pData, int n)
{
	int i;	
	EM85xx_WriteReg (I2C_MASTER_CLK_DIV, CLKDIV);	// assume 150mhz clock (375=750/2)
	
	// ready for a command
	// XXX bug: we really should have a timeout here
	while ((EM85xx_ReadReg (I2C_MASTER_STATUS) & 1) == 0);

	for (i=0; i<n; i++)
	{
		EM85xx_WriteReg (I2C_MASTER_CONFIG, 0xfa);
		EM85xx_WriteReg (I2C_MASTER_BYTE_CNT, 0);
		EM85xx_WriteReg (I2C_MASTER_DEV_ADDR, devaddr >> 1);
		EM85xx_WriteReg	(I2C_MASTER_DATAOUT, adr++);		
		EM85xx_WriteReg	(I2C_MASTER_STARTXFER, 0 | 0x4);
		// XXX bug: we really should have a timeout here
		while ((EM85xx_ReadReg (I2C_MASTER_STATUS) & 2) == 0);
		while ((EM85xx_ReadReg (I2C_MASTER_STATUS) & 1) == 0);

		EM85xx_WriteReg (I2C_MASTER_CONFIG, 0x00FA);
		EM85xx_WriteReg (I2C_MASTER_BYTE_CNT, 0);		
		EM85xx_WriteReg (I2C_MASTER_DEV_ADDR, devaddr >> 1);	
		EM85xx_WriteReg (I2C_MASTER_STARTXFER, 1);

		// XXX bug: we really should have a timeout here
		while ((EM85xx_ReadReg (I2C_MASTER_STATUS) & 4) == 0);
		while ((EM85xx_ReadReg (I2C_MASTER_STATUS) & 1) == 0);

		*pData++ = (char)EM85xx_ReadReg (I2C_MASTER_DATAIN);
	}
	return 1;
}

void WriteI2CRom( char* data, int size )
{
	int i;
	char rdata;

	for( i=0; i<size; i++ ) {
		EM85xxI2C__Write( ADDR_DEVICE, i, (data+i), 1 );
__loop:
		EM85xxI2C__Read( ADDR_DEVICE, i, &rdata, 1 );
		if( rdata != *(data+i) ) goto __loop;
		printf( "Writing....... 0x%02x, 0x%02x, 0x%02x\r", ADDR_DEVICE, i, *(data+i) );
	}
	printf( "... Completed\n" );
}

int VerifyI2CRom( char* data, int size )
{
	char rdata;
	int i;

	for( i=0; i<size; i++ ) {
__retry:
		EM85xxI2C__Read( ADDR_DEVICE, i, &rdata, 1 );
#if 1
		if( data[i] != rdata ) {
			goto __retry;
		}
#endif
		printf( "rdata->0x%02x, data[%d]->0x%02x\n", (unsigned char)rdata, i, (unsigned)data[i] );
	}

	if( i<size ) return -1;

	return 0;
}

int main( int argc, char** argv )
{
	int fd;
	int size;
	char* data;

	if( argc < 2 ) {
		fprintf( stderr, "usage: i2crom binary.bin\n" );
		return 1;
	}

	fd = open( argv[1], O_RDONLY );
	if( fd<0 ) {
		perror( "open" );
		return 1;
	}

	data = ( char* )malloc( 2048 );
	size = read( fd, data, 2048 );
	close( fd );

	WriteI2CRom( data, size );
	if( VerifyI2CRom( data, size ) < 0 ) {
		printf( "Failed to verify...\n" );
	}
	else {
		printf( "Success to verify..\n" );
	}

	free( data );

	return 0;
}
