#include "bitstrm.h"

// this class will increment Err if you proceed past the end of the buffer
BitStream::BitStream (unsigned char *pBuffer, unsigned long Length, int *Err)
{
	m_rdbfr = pBuffer;
	m_rdbfr_length = Length;
	m_nbits = 0;
	initbits (Err);
}

BitStream::~BitStream ()
{
}
			   
void BitStream::initbits (int *Err)
{
	m_incnt = 0;
	m_rdptr = m_rdbfr + m_rdbfr_length;
	m_rdmax = m_rdptr;
	m_bitcnt = 0;
	m_bfr = 0;
	m_rdptr = m_rdbfr;
	flushbits (0, Err);
}

// return next n bits (right adjusted) without advancing
unsigned int BitStream::showbits (int n)
{
	return m_bfr >> (32-n);
}

// advance by n bits
void BitStream::flushbits (int n, int *Err)
{
	int incnt;

	m_nbits += n;

	if (n == 32)
		m_bfr = 0;
	else
		m_bfr <<= n;	
	incnt = m_incnt -= n;
	if (incnt <= 24)
	{
		if (m_rdptr < m_rdbfr+(m_rdbfr_length-4))
		{
			do
			{
				m_bfr |= *m_rdptr++ << (24 - incnt);
				incnt += 8;
			}
			while (incnt <= 24);
		}
		else
		{
			do
			{
				if (m_rdptr >= m_rdbfr+m_rdbfr_length)
				{
					*Err++;
					m_bfr |= 0 << (24 - incnt);
				}
				else
					m_bfr |= *m_rdptr++ << (24 - incnt);
				incnt += 8;
			}
			while (incnt <= 24);
		}
		m_incnt = incnt;
	}
}

void BitStream::flushbits32 (int *Err)
{
	int incnt;

	m_nbits += 32;

	m_bfr = 0;
	incnt = m_incnt;
	incnt -= 32;
    while (incnt <= 24)
    {
		if (m_rdptr >= m_rdbfr+m_rdbfr_length)
		{
			*Err++;
			m_bfr |= 0 << (24 - incnt);
		}
		else
			m_bfr |= *m_rdptr++ << (24 - incnt);
		incnt += 8;
    }
	m_incnt = incnt;
}


// return next n bits (right adjusted)
unsigned int BitStream::getbits (int n, int *Err)
{
	unsigned int l;

	l = showbits (n);
	flushbits (n, Err);

	return l;
}

unsigned int BitStream::getbits32 (int *Err)
{
	unsigned int l;

	l = showbits (32);
	flushbits32 (Err);

	return l;
}

void BitStream::align (int *Err)
{
	int incnt;

	if (m_incnt != 32)
	{
		incnt = m_incnt % 8;
		flushbits (incnt, Err);
	}
}

long BitStream::getnbits ()
{
	return m_nbits;
}

///////////////////////////////////////////////////////////////////////////////////////
#define BYTE_NUMBIT 8		// bits in byte (char)
#define LONG_NUMBIT 32		// bits in unsigned long
#define bit2byte(a) (((a)+BYTE_NUMBIT-1)/BYTE_NUMBIT)
#define byte2bit(a) ((a)*BYTE_NUMBIT)
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

BitBuffer::BitBuffer (unsigned char *pBuffer, unsigned long Length)
{
	m_size = Length * BYTE_NUMBIT;
	m_numBit = 0;
	m_currentBit = 0;
	m_data = pBuffer;
}

BitBuffer::~BitBuffer ()
{
}

long BitBuffer::numbits ()
{
	return m_numBit;
}

int BitBuffer::writebyte (unsigned long data, int numBit)
{
	long numUsed,idx;

	idx = (m_currentBit / BYTE_NUMBIT) % bit2byte(m_size);
	if (idx == m_size)
		return -1;
	numUsed = m_currentBit % BYTE_NUMBIT;
	if (numUsed == 0)
		m_data[idx] = 0;
	m_data[idx] |= ((data & (1<<numBit)-1)) << (BYTE_NUMBIT-numUsed-numBit);
	m_currentBit += numBit;
	m_numBit = m_currentBit;

	return 0;
}

long BitBuffer::putbits (unsigned long data, int numBit)
{
	int num,maxNum,curNum;
	unsigned long bits;

	if (numBit == 0)
		return 0;

	num = 0;
	maxNum = BYTE_NUMBIT - m_currentBit % BYTE_NUMBIT;
	while (num < numBit) {
		curNum = min(numBit-num,maxNum);
		bits = data >> (numBit-num-curNum);
		if (writebyte (bits, curNum))
			return -1;		
		num += curNum;
		maxNum = BYTE_NUMBIT;
	}
	return 0;
}

long BitBuffer::align ()
{
	int len, i,j;
	len = numbits ();
	   
	j = (BYTE_NUMBIT - (len % BYTE_NUMBIT)) % BYTE_NUMBIT;

	if ((len % BYTE_NUMBIT) == 0) 
		j = 0;
	for (i=0; i<j; i++)
		putbits (0, 1);	
	return j;
}
