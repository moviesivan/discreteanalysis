#ifndef COMPRESSOR_DOT_H
#define COMPRESSOR_DOT_H

#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <string>

#include "byteio.h"
#include "bitio.h"
#include "trie.h"

#ifdef LOG
#include <iomanip>
#include <iostream>
#endif


template<typename INPUT, typename OUTPUT, typename MODEL>
class compressor
	{
	  typedef typename MODEL::CODE_VALUE CODE_VALUE;
	  typedef typename MODEL::prob prob;
	public :
	  compressor(INPUT &input, OUTPUT &output, MODEL &model ) 
	  : m_input(input),
		m_output(output),
		m_model(model)
	  {
	  }
	  int operator()()
	  {
		trie table; 
		init(&table);				
		int pre = 0;
		int maxbits = 16; 
		  
		int max_codes = 1 << maxbits;					
		char tmpname[] = "/tmp/tmp.XXXXXX";
		mkstemp(tmpname);
		std::fstream temp(tmpname, std::fstream::in | std::fstream::out | std::fstream::binary);
		output_bits<std::fstream> tempbits(temp);
		tempbits.put_bits(8,(char)maxbits);


		int c;									
		int total_codes = 259;					
		int offset = 3; 							
		int nBits = 9;								

		while((c = m_input.getByte()) != EOF){
label:			
			int kids = 0;
			int curr_code;
			if (pre > 2){
				kids = table[pre].kid_count;
				curr_code = table[pre].code;
			} else {	
				pre = c + 3;
				continue;
			}

			table[pre].appearances += 1;
			int code = search(&table, pre, c+offset, 0, kids - 1);
			if (code >= 0 && ((total_codes + 1) < max_codes) ){	
				pre = code; 
				continue;
			} else{	
				tempbits.put_bits(nBits, curr_code);
				if(total_codes < max_codes){
					nBits = reinit(&table, total_codes, maxbits);
					insert(&table, pre, c+offset, total_codes);
					total_codes++;

				}
				pre = 0; 										
				if (max_codes == total_codes){
					tempbits.put_bits(nBits, 0);
					total_codes = prune(&table, total_codes);
					nBits = reinit(&table, total_codes, maxbits);
					pre = 0;
				}	
				goto label;
			}
			if (max_codes == total_codes){
				tempbits.put_bits(nBits, 0);
				total_codes = prune(&table, total_codes);
				nBits = reinit(&table, total_codes, maxbits);
				pre = 0;
			}

		}
		if (pre > 2){
			table[pre].appearances += 1;					
			tempbits.put_bits(nBits, pre);
		}
		tempbits.put_bits(nBits, 2);
		tempbits.flush_bits();
		kill_children(&table, total_codes);	
		free(table);
		
		
		int pending_bits = 0;
		CODE_VALUE low = 0;
		CODE_VALUE high = MODEL::MAX_CODE;
		temp.flush();
		temp.seekg(0, temp.beg);
		input_bytes<std::fstream> tempbytes(temp);
		for ( ; ; ) {
		  int c = tempbytes.getByte();
		  if ( c == EOF )
			c = 256;
		  prob p = m_model.getProbability( c );
		  CODE_VALUE range = high - low + 1;
		  high = low + (range * p.high / p.count) - 1;
		  low = low + (range * p.low / p.count);
		  for ( ; ; ) {
			if ( high < MODEL::ONE_HALF )
			  put_bit_plus_pending(0, pending_bits);
			else if ( low >= MODEL::ONE_HALF )
			  put_bit_plus_pending(1, pending_bits);
			else if ( low >= MODEL::ONE_FOURTH && high < MODEL::THREE_FOURTHS ) {
			  pending_bits++;
			  low -= MODEL::ONE_FOURTH;  
			  high -= MODEL::ONE_FOURTH;  
			} else
			  break;
			high <<= 1;
			high++;
			low <<= 1;
			high &= MODEL::MAX_CODE;
			low &= MODEL::MAX_CODE;
		  }
		  if ( c == 256 )
			break;
		}
		pending_bits++;
		if ( low < MODEL::ONE_FOURTH )
		  put_bit_plus_pending(0, pending_bits);
		else
		  put_bit_plus_pending(1, pending_bits);
		temp.close();
		return 0;
	  }

	  inline void put_bit_plus_pending(bool bit, int &pending_bits)
	  {
		m_output.put_bit(bit);
		for ( int i = 0 ; i < pending_bits ; i++ )
		  m_output.put_bit(!bit);
		pending_bits = 0;
	  }
	private :
	  OUTPUT &m_output;
	  INPUT &m_input;
	  MODEL &m_model;
};

template<typename INPUT, typename OUTPUT, typename MODEL>
int compress(INPUT &source, OUTPUT &target, MODEL &model)
{
  input_bytes<INPUT> in(source);
  output_bits<OUTPUT> out(target);
  compressor<input_bytes<INPUT>, output_bits<OUTPUT>, MODEL> c(in,out, model);
  return c();
}

#endif //#ifndef COMPRESSOR_DOT_H
