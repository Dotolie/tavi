#ifndef _BITSTRM_H
#define _BITSTRM_H

// this class will increment Err if you proceed past the end of the buffer
// the buffer is padded with zeros if the input buffer length is not modulo 
// 4 bytes
class BitStream
{
private:
	int				m_incnt;
	unsigned char *	m_rdbfr;
	unsigned long	m_rdbfr_length;
	unsigned char *	m_rdptr;
	unsigned char * m_rdmax;
	int				m_bitcnt;
	unsigned int	m_bfr;
	
	void initbits (int *Err);

public:
	BitStream (unsigned char *pBuffer, unsigned long Length, int *Err);
	~BitStream ();

	long			m_nbits;

	// get n bits without advancing
	unsigned int showbits (int n);
	// advance n bits
	void flushbits (int n, int *Err);
	// advance 32 bits
	void flushbits32 (int *Err);
	// get n bits + advance
	unsigned int getbits (int n, int *Err);
	// get 32bits + advance
	unsigned int getbits32 (int *Err);
	// align to the next byte
	void align (int *Err);
	// see how many bits have been processed
	long getnbits ();
};

class BitBuffer
{
	unsigned char *m_data;	// data bits	
	long m_size;			// buffer size in bits
	long m_currentBit;		// current bit position in bit stream
	long m_numByte;			// number of bytes read/written

	int  writebyte (unsigned long data, int numBit);

public:
	BitBuffer (unsigned char *pBuffer, unsigned long Length);
	~BitBuffer ();

	long m_numBit;			// number of bits in buffer

	long numbits ();
	long putbits (unsigned long data, int numbits);
	long align ();
};

#endif
