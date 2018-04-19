#ifndef BITIO_DOT_H
#define BITIO_DOT_H

#include "byteio.h"
#include <limits.h>

template<typename OUTPUT>
class output_bits
{
	public :
	  output_bits(OUTPUT &output)
	  : m_Output(output),
		m_NextByte(0),
		m_Mask(0x80) 
	  {
	  }
	  ~output_bits()
	  {
		if ( m_Mask != 0x80 )
		  m_Output.putByte(m_NextByte);
	  }
	  void put_bit( bool val ) {
		if ( val )
		  m_NextByte |= m_Mask;
		m_Mask >>= 1;
		if ( !m_Mask) {
		  m_Output.putByte(m_NextByte);
		  m_Mask = 0x80;
		  m_NextByte = 0;
		}
	  }
	  void put_bits (int nBits, int code)
	  {

		if (nBits > (sizeof(int)-1) * CHAR_BIT)
		exit (fprintf (stderr, "putBits: nBits = %d too large\n", nBits));

		code &= (1 << nBits) - 1; 
		for (int i=1; i <= nBits; i++) {
			put_bit((code >> (nBits-i)) & 1);
		}
	  }
	  void flush_bits()
	  {
		  if ( m_Mask != 0x80 )
		  m_Output.putByte(m_NextByte);
	  }
	private :
	  output_bytes<OUTPUT> m_Output;
	  char m_NextByte;
	  unsigned char m_Mask;

};

template<typename INPUT>
class input_bits
{
	public :
	  input_bits(INPUT &input)
	  : m_Input(input),
		m_CurrentByte(0),
		m_LastMask(1)
	  {
	  }
	  bool get_bit() {
		if ( m_LastMask == 1 ) {
		  m_CurrentByte = m_Input.getByte();
		  m_LastMask = 0x80;
		} else
		  m_LastMask >>= 1;
		return (m_CurrentByte & m_LastMask) != 0;
	  }
	  int getBits (int nBits)
	  {
		int c=0;
											  
		if (nBits > (sizeof(int)-1) * CHAR_BIT)
		exit (fprintf (stderr, "getBits: nBits = %d too large\n", nBits));

		// Read enough new bytes to have at least nBits bits to extract code
		c |= get_bit();
		for (int i = 1; i < nBits; i++) {
			c <<= 1;
			c |= get_bit();
		}                     
		return c;
	  }

	private :
	  input_bytes<INPUT> m_Input;
	  int m_CurrentByte;
	  unsigned char m_LastMask;
};

#endif //#ifndef BITIO_DOT_H

