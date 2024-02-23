#include "PacketExtractor.h"

#include "PacketHandler.h"

namespace l2 {

PacketExtractor::PacketExtractor( const std::shared_ptr<PacketHandler> & pHandler ) 
	: m_pHandler( pHandler )
{
}

PacketExtractor::~PacketExtractor() {
}

void PacketExtractor::add( const unsigned char * buf, unsigned int len ) {
	std::shared_ptr<Buffer> pBuffer = m_pBuffer;

	if( !pBuffer ) {
		pBuffer = std::make_shared<Buffer>( buf, buf + len );
	}
	else {
		std::shared_ptr<Buffer> pBufferTmp = std::make_shared<Buffer>( pBuffer->size() + len );
		std::copy( pBuffer->data(), pBuffer->data() + pBuffer->size(), pBufferTmp->data() );
		std::copy( buf, buf + len, pBufferTmp->data() + pBuffer->size() );
		pBuffer = pBufferTmp;
	}

	unsigned int offset = 0;
	unsigned int remaining = pBuffer->size();

	for( ;; ) {
		if( (pBuffer->size() - offset < 2) ) {
			// not enough data to read l2 packet size
			break;
		}

		unsigned short l2_packet_size = ((*pBuffer)[offset + 1] << 8) | (*pBuffer)[offset + 0];

		if( l2_packet_size > remaining ) {
			// l2 packet not complete
			break;
		}

		m_pHandler->process( pBuffer->data() + offset + 2, l2_packet_size - 2 );

		offset += l2_packet_size;
		remaining -= l2_packet_size;

		if( remaining == 0 ) {
			// not data left
			break;
		}
	}

	if( offset == 0 ) {
		m_pBuffer = pBuffer;
	}
	else {
		if( remaining == 0 ) {
			m_pBuffer.reset();
		}
		else {
			m_pBuffer = std::make_shared<Buffer>( pBuffer->data() + offset, pBuffer->data() + offset + remaining );
		}
	}
}

} // namespace l2
