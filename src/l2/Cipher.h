#pragma once

#include <memory>

namespace l2 {

class Cipher {
public:
	Cipher( const boost::array<unsigned char, 16> & decode_key );

	void decrypt( unsigned char * buf, unsigned int len );

private:
	boost::array<unsigned char, 16> m_decode_key;
};

} // namespace l2
