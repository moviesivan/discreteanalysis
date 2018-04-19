#ifndef DECOMPESSOR_DOT_H
#define DECOMPESSOR_DOT_H

#ifdef LOG
#include <iostream>
#include <iomanip>
#endif

#include <iostream>
#include <cstdio>
#include <string>

#include "byteio.h"
#include "bitio.h"
#include "trie.h"

template<typename INPUT, typename OUTPUT, typename MODEL>
class decompressor
{
	  typedef typename MODEL::CODE_VALUE CODE_VALUE;
	  typedef typename MODEL::prob prob;
	public :
	  decompressor(INPUT &input, OUTPUT &output, MODEL &model ) 
	  : m_input(input),
		m_output(output),
		m_model(model)
	  {
	  }
	  int operator()()
	  {
		CODE_VALUE high = MODEL::MAX_CODE;
		CODE_VALUE low = 0;
		CODE_VALUE value = 0;
		char tmpname[] = "/tmp/tmp.XXXXXX";
		mkstemp(tmpname);
		std::fstream temp(tmpname, std::fstream::in | std::fstream::out | std::fstream::binary);
		output_bytes<std::fstream> tempbytes(temp);
		for ( int i = 0 ; i < MODEL::CODE_VALUE_BITS ; i++ ) {
		  value <<= 1;
		  value += m_input.get_bit() ? 1 : 0;
		}
		for (int i = 0 ; ; i++) {
		  CODE_VALUE range = high - low + 1;
		  CODE_VALUE scaled_value =  ((value - low + 1) * m_model.getCount() - 1 ) / range;
		  int c;
		  prob p = m_model.getChar( scaled_value, c );
		  if ( c == 256 )
			break;
		  tempbytes.putByte(c);
		  high = low + (range*p.high)/p.count -1;
		  low = low + (range*p.low)/p.count;
		  for( ; ; ) {
			if ( high < MODEL::ONE_HALF ) {
			} else if ( low >= MODEL::ONE_HALF ) {
			  value -= MODEL::ONE_HALF;  
			  low -= MODEL::ONE_HALF;
			  high -= MODEL::ONE_HALF;
			} else if ( low >= MODEL::ONE_FOURTH && high < MODEL::THREE_FOURTHS ) {
			  value -= MODEL::ONE_FOURTH;
			  low -= MODEL::ONE_FOURTH;
			  high -= MODEL::ONE_FOURTH;
			} else
			  break;
			low <<= 1;
			high <<= 1;
			high++;
			value <<= 1;
			value += m_input.get_bit() ? 1 : 0;
		  }
		}
		temp.flush();
		temp.seekg(0, temp.beg);
		input_bits<std::fstream> tempbits(temp);
		
		int maxbits = tempbits.getBits(8);		

		int max_codes = 1 << maxbits;			

		trie table; 
		init(&table);		
		int old_pre = 0;
		int nBits = 9;
		int offset = 3;

		int c;					
		int total_codes = 259;			

		while((c = tempbits.getBits(nBits)) != 2){
			if (c == 0){			
				total_codes = prune(&table, total_codes);
				nBits = reinit(&table, total_codes, maxbits);
				c = tempbits.getBits(nBits);
				if (c == 2)
					break;
				old_pre = 0;
			}
			int first_letter = c;
			if (c == total_codes)
				first_letter = old_pre;
			if (c > total_codes){
				fprintf(stderr,"invalid input\n");
				return 255;
			}
			while(table[first_letter].prefix != 0)
				first_letter = table[first_letter].prefix;
				
			if(total_codes < max_codes){
				if (old_pre > 2){
					insert(&table, old_pre, first_letter - offset,total_codes);
					total_codes++;
				}
				nBits = reinit(&table, total_codes, maxbits);
			}
			output(&table, c);
			old_pre = c;
		}

		kill_children(&table, total_codes);	
		free(table);
		temp.close();
		
		return 0;
	  }
	  void output(trie *table, int entry){
		(*table)[entry].appearances += 1;
		if (entry == 0)
			return;
		output(table, (*table)[entry].prefix);
		m_output.putByte((char)(*table)[entry].suffix);
		return;
	}
	private :
	  OUTPUT &m_output;
	  INPUT &m_input;
	  MODEL &m_model;
};

template<typename INPUT, typename OUTPUT, typename MODEL>
int decompress(INPUT &source, OUTPUT &target, MODEL &model)
{
  input_bits<INPUT> in(source);
  output_bytes<OUTPUT> out(target);
  decompressor<input_bits<INPUT>, output_bytes<OUTPUT>, MODEL> d(in,out, model);
  return d();
}

#endif //#ifndef DECOMPESSOR_DOT_H
