#include "Cipher.h"

namespace l2 {

Cipher::Cipher( const boost::array<unsigned char, 16> & decode_key ) 
	: m_decode_key( decode_key )
{
}

void Cipher::decrypt( unsigned char * buf, unsigned int len ) {
	unsigned char tmp = 0;

	for( unsigned int k = 0; k < len; k++ ) {
		unsigned char tmp2 = buf[k];
		buf[k] = tmp2 ^ m_decode_key[k & 0x0F] ^ tmp;
		tmp = tmp2;
	}

	unsigned int * p_Key = (unsigned int *) &m_decode_key[8];
	*p_Key += len;

	/**/
	//unsigned int old = ((unsigned int)m_decode_key[11] & 0x000000FF)
	//	| (((unsigned int)m_decode_key[10] << 8) & 0x0000FF00)
	//	| (((unsigned int)m_decode_key[9] << 16) & 0x00FF0000)
	//	| (((unsigned int)m_decode_key[8] << 24) & 0xFF000000);
	//old += len;

	//m_decode_key[11] = (unsigned char)(old & 0xFF);
	//m_decode_key[10] = (unsigned char)(old >> 8 & 0xFF);
	//m_decode_key[9] = (unsigned char)(old >> 16 & 0xFF);
	//m_decode_key[8] = (unsigned char)(old >> 24 & 0xFF);
	/*/
	unsigned int old = ((unsigned int)m_decode_key[8] & 0x000000FF)
		| (((unsigned int)m_decode_key[9] << 8) & 0x0000FF00)
		| (((unsigned int)m_decode_key[10] << 16) & 0x00FF0000)
		| (((unsigned int)m_decode_key[11] << 24) & 0xFF000000);
	old += len;

	m_decode_key[8] = (unsigned char)(old & 0xFF);
	m_decode_key[9] = (unsigned char)(old >> 8 & 0xFF);
	m_decode_key[10] = (unsigned char)(old >> 16 & 0xFF);
	m_decode_key[11] = (unsigned char)(old >> 24 & 0xFF);
	/**/
}

} // namespace l2
