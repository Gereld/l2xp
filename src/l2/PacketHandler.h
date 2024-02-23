#pragma once

#include <memory>

#include "GameServerPacketsTypes.h"
#include "../Connection.h"

namespace l2 {

class Cipher;

class PacketHandler {
public:
	PacketHandler( const Connection & connection );
	virtual ~PacketHandler();

	void process( unsigned char * buffer, unsigned int len );

protected:
	std::shared_ptr<Cipher> m_pCipher;
	Connection m_connection;
	unsigned int m_user_object_id;

	typedef std::map<unsigned int, unsigned int> Template_map;
	Template_map m_objects;

	void display_buffer( unsigned char * buffer, unsigned int len );
	void abstract_npc_info( unsigned int object_id, unsigned int template_id );
	void delete_object( unsigned int object_id );
	unsigned int get_template( unsigned int object_id ) const;
};

} // namespace l2