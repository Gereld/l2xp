#include "PacketHandler.h"

#include "Cipher.h"
#include "GameServerPacketsNames.h"
#include "SystemMessagesIds.h"
#include "../Document.h"

#include <Gx/io/ByteArrayInputStream.h>
#include <Gx/io/DataInputStream.h>
#include <cstdlib>
#include <WinNls.h>
#include <boost/variant.hpp>

namespace l2 {

// ************************************************************************************************

std::wstring read_unicode_string( const std::shared_ptr<Gx::io::DataInputStream> & dis ) {
	std::wstring wstr;

	short word;
	while( (word = dis->ReadShort()) != 0 ) {
		wstr.push_back( word );
	}

	return wstr;
}

std::string from_unicode( const std::wstring & wstr ) {
	size_t nByteNeeded = wcstombs( nullptr, wstr.c_str(), 0 );
	std::vector<char> buffer( nByteNeeded + 1 );
	size_t nResult = wcstombs( buffer.data(), wstr.c_str(), buffer.size() );
	if( nResult == size_t(-1) ) {
		return std::string();
	}
	else {
		return std::string( buffer.data() );
	}
}

typedef boost::variant<unsigned int, __int64, std::wstring> L2_param;

std::string to_string( const L2_param & param ) {
	if( boost::get<unsigned int>( &param ) != nullptr ) {
		return boost::lexical_cast<std::string>( boost::get<unsigned int>( param ) );
	}
	else if( boost::get<__int64>( &param ) != nullptr ) {
		return boost::lexical_cast<std::string>( boost::get<__int64>( param ) );
	}
	else if( boost::get<std::wstring>( &param ) != nullptr ) {
		return from_unicode( boost::get<std::wstring>( param ) );
	}
	return std::string();
}

// ************************************************************************************************

PacketHandler::PacketHandler( const Connection & connection ) 
	: m_connection( connection )
{
	m_user_object_id = 0;
}

PacketHandler::~PacketHandler() {
}

void PacketHandler::process( unsigned char * buffer, unsigned int len ) {
	if( m_pCipher ) {
		m_pCipher->decrypt( buffer, len );
	}

	unsigned char type = buffer[0];

	DBG_LOG( get_packet_name( type ) << " (" << len << ")" << std::endl );

	if( type == l2::L2_Init ) {
		/*
		printf( "L2_Init %d\n", len );

		for( unsigned int i = 0; i < len; ++i ) {
			printf( "%x ", buffer[i] );
		}

		printf( "\n" );
		*/

		boost::array<unsigned char, 16> decode_key;
		for( int i = 0; i < 8; i++ ) {
			decode_key[i] = buffer[i + 2];
		}

		decode_key[8] = (unsigned char)0xc8;
		decode_key[9] = (unsigned char)0x27;
		decode_key[10] = (unsigned char)0x93;
		decode_key[11] = (unsigned char)0x01;
		decode_key[12] = (unsigned char)0xa1;
		decode_key[13] = (unsigned char)0x6c;
		decode_key[14] = (unsigned char)0x31;
		decode_key[15] = (unsigned char)0x97;

		m_pCipher = std::make_shared<Cipher>( decode_key );
	}
	else {
		/*
		if( (type != 0x27) && (type != 0x2f) && (type != 0x0c) ) {
			printf( "l2_packet_type=%x (%s)\n", type, get_packet_name( type ) );
		}
		*/

		std::shared_ptr<Gx::io::ByteArrayInputStream> bais = std::make_shared<Gx::io::ByteArrayInputStream>( reinterpret_cast<const char *>( buffer ), len );
		std::shared_ptr<Gx::io::DataInputStream> dis = std::make_shared<Gx::io::DataInputStream>( bais );

		// skip type
		dis->ReadByte();

		switch( type ) {
		case L2_SystemMessage: 
			{
				unsigned int msg = ( dis->ReadInt() );
				unsigned int num_args = ( dis->ReadInt() );

				DBG_LOG( "msg = " << msg << std::endl );
				DBG_LOG( "num_args = " << num_args << std::endl );

				std::vector<L2_param> params;

				for( unsigned int i = 0; i < num_args; ++i ) {
					unsigned int param_type = dis->ReadInt();

					L2_param param;

					switch( param_type ) {
					case L2_TYPE_TEXT:
						{
							DBG_LOG( "L2_TYPE_TEXT" << std::endl );
							std::wstring str = read_unicode_string( dis );

							param = str;
						}
						break;
					case L2_TYPE_SKILL_NAME:
						{
							DBG_LOG( "L2_TYPE_SKILL_NAME" << std::endl );
							unsigned int skill_name = dis->ReadInt();
							unsigned int skill_level = dis->ReadInt();

							param = skill_name;
						}
						break;
					case L2_TYPE_ZONE_NAME:
						{
							DBG_LOG( "L2_TYPE_ZONE_NAME" << std::endl );
							unsigned int zonex = dis->ReadInt();
							unsigned int zoney = dis->ReadInt();
							unsigned int zonez = dis->ReadInt();

							param = zonex;
						}
						break;
					case L2_TYPE_ITEM_NAME:
					case L2_TYPE_NPC_NAME:
					case L2_TYPE_NUMBER:
						{
							DBG_LOG( "L2_TYPE_NUMBER" << std::endl );
							unsigned int num = dis->ReadInt();

							param = num;
						}
						break;
					case L2_TYPE_ITEM_NUMBER:
						{
							DBG_LOG( "L2_TYPE_ITEM_NUMBER" << std::endl );
							__int64 exp1 = dis->ReadInt();
							__int64 exp2 = dis->ReadInt();

							param = (exp2 << 32) | exp1;
						}
						break;
					default:
						break;
					}

					params.push_back( param );
				}

				switch( msg ) {
				case L2_YOU_PICKED_UP_S1_ADENA:
				case L2_YOU_HAVE_EARNED_S1_ADENA:
					{
						__int64 adena = boost::get<__int64>( params[0] );

						get_document()->add_adena( m_connection, adena );
						break;
					}
				case L2_YOU_HAVE_EARNED_S1_EXPERIENCE:
					{
						__int64 exp = boost::get<__int64>( params[0] );

						get_document()->add_xp( m_connection, exp );
						break;
					}
				case L2_YOU_HAVE_EARNED_S1_EXPERIENCE_AND_S2_SP:
					{
						__int64 exp = boost::get<__int64>( params[0] );
						unsigned int sp = boost::get<unsigned int>( params[1] );

						get_document()->add_xp( m_connection, exp );
						get_document()->add_sp( m_connection, sp );
						break;
					}
				case L2_YOU_DID_S1_DMG:
					{
						unsigned int damage = boost::get<unsigned int>( params[0] );

						get_document()->add_damage( m_connection, damage );
					}
					break;
				case 2265:
					get_document()->add_damage( m_connection, 0 );
					break;
				case 2266:
					get_document()->set_damage_type( m_connection, DMG_PHYSICAL_CRIT );
					break;
				case 2261: // dommages donnés
					{
						unsigned int damage = boost::get<unsigned int>( params[2] );
						/*
						DBG_LOG( "arg0 = " << to_string( params[0] ) << std::endl );
						DBG_LOG( "arg1 = " << to_string( params[1] ) << std::endl );
						DBG_LOG( "arg2 = " << to_string( params[2] ) << std::endl );
						*/

						if( damage > 100 ) {
							get_document()->add_damage( m_connection, damage );
						}
					}
					break;
				case 2262: // dommages reçus
					{
						/*
						unsigned int damage = boost::get<unsigned int>( params[2] );
						DBG_LOG( "arg0 = " << to_string( params[0] ) << std::endl );
						DBG_LOG( "arg1 = " << to_string( params[1] ) << std::endl );
						DBG_LOG( "arg2 = " << to_string( params[2] ) << std::endl );

						get_document()->add_damage( m_connection, damage );
						*/
					}
					break;
				case 2280: // atck resited ?
					{
						//DBG_LOG( "arg0 = " << to_string( params[0] ) << std::endl );
						//DBG_LOG( "arg1 = " << to_string( params[1] ) << std::endl );
					}
					break;
				case 1280: // magical critical hit ?
					get_document()->set_damage_type( m_connection, DMG_MAGICAL_CRIT );
					break;
				default:
					break;
				} // case
			}
			break;
		case L2_CharacterSelected:
			{
				std::string name = from_unicode( read_unicode_string( dis ) );

				get_document()->set_name( m_connection, name );
			}
			break;
		case L2_MyTargetSelected:
			{
				unsigned int object_id = dis->ReadInt();

				get_document()->set_last_target_id( m_connection, get_template( object_id ) );
			}
			break;
		case L2_AbstractNpcInfo:
			{
				unsigned int object_id = dis->ReadInt();
				unsigned int template_id = dis->ReadInt();

				abstract_npc_info( object_id, template_id - 1000000 );
			}
			break;
		case L2_DeleteObject:
			{
				unsigned int object_id = dis->ReadInt();

				delete_object( object_id );
			}
			break;
		case L2_LogOutOk:
			get_document()->log_out( m_connection );
			break;
			/*
		case L2_AutoAttackStart:
			{
				unsigned int object_id = dis->ReadInt();
				//DBG_LOG( "object_id = " << object_id << " " << get_template( object_id ) << std::endl );
				if( object_id == m_user_object_id ) {
					//DBG_LOG( "User AutoAttackStart" << std::endl );
				}
			}
			break;
			*/
		case L2_AutoAttackStop:
			{
				unsigned int object_id = dis->ReadInt();
				//DBG_LOG( "object_id = " << object_id << " " << get_template( object_id ) << std::endl );
				if( object_id == m_user_object_id ) {
					//DBG_LOG( "User AutoAttackStop" << std::endl );
					get_document()->stop_combat_mode( m_connection );
				}
			}
			break;
			/*
		case L2_SocialAction:
			{
				unsigned int object_id = dis->ReadInt();
				unsigned int action_id = dis->ReadInt();
				DBG_LOG( "object_id = " << object_id << " " << get_template( object_id ) << std::endl );
				DBG_LOG( "action_id = " << action_id << std::endl );
			}
			break;
			*/
			/*
		case L2_StatusUpdate:
			{
				unsigned int object_id = dis->ReadInt();
				DBG_LOG( "object_id = " << object_id << " " << get_template( object_id ) << std::endl );
			}
			break;
			*/
		case L2_UserInfo:
			{
				dis->ReadInt();
				dis->ReadInt();
				dis->ReadInt();
				dis->ReadInt();
				m_user_object_id = dis->ReadInt();
				//DBG_LOG( "object_id = " << m_user_object_id << " " << get_template( m_user_object_id ) << std::endl );
			}
			break;
		default:
			break;
		} // case
	} // if
}

void PacketHandler::display_buffer( unsigned char * buffer, unsigned int len ) {
	DBG_LOG( std::hex );
	DBG_LOG( std::setfill( '0' ) );
	for( unsigned int i = 0; i < len; ++i ) {
		DBG_LOG( std::setw( 2 ) << (int) buffer[i] << " " );
	}
	DBG_LOG( std::dec );
	DBG_LOG( std::endl );
}

void PacketHandler::abstract_npc_info( unsigned int object_id, unsigned int template_id ) {
	m_objects[object_id] = template_id;
}

void PacketHandler::delete_object( unsigned int object_id ) {
	m_objects.erase( object_id );
}

unsigned int PacketHandler::get_template( unsigned int object_id ) const {
	Template_map::const_iterator pos = m_objects.find( object_id );
	if( pos != m_objects.end() ) {
		return pos->second;
	}
	return 0;
}

} // namespace l2


// KeyPacket.java
	//public void writeImpl()
	//{
	//	writeC(0x2e);
	//	writeC(_id); //0 - wrong protocol, 1 - protocol ok
	//	for (int i = 0; i < 8; i++)
	//	{
	//		writeC(_key[i]); // key
	//	}
	//	writeD(0x01);
	//	writeD(0x01); // server id
	//	writeC(0x01);
	//	writeD(0x00); // obfuscation key
	//}

