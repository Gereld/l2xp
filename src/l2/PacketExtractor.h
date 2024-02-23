#pragma once 

#include <memory>
#include <vector>

namespace l2 {

class PacketHandler;
typedef std::vector<unsigned char> Buffer;

class PacketExtractor {
public:
	PacketExtractor( const std::shared_ptr<PacketHandler> & pHandler );
	~PacketExtractor();

	void add( const unsigned char * buf, unsigned int len );

protected:
	std::shared_ptr<PacketHandler>	m_pHandler;
	std::shared_ptr<Buffer>	m_pBuffer;
};

} // namespace l2
