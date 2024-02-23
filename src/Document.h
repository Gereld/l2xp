#pragma once

#include "Connection.h"
#include "Character.h"
#include "l2/PacketExtractor.h"
#include "net/IPacket_processor.h"

#include <map>
#include <pcap/pcap.h>
#include <Winsock2.h>
#include <thread>
#include <Gx/io/OutputStream.h>
#include <mutex>

class MainFrame;

class Document {
public:
	MainFrame * m_pFrame;

public:
	Document();
	~Document();

	void set_name( const Connection & connection, const std::string & name );
	void add_xp( const Connection & connection, __int64 xp );
	void add_sp( const Connection & connection, int sp );
	void add_adena( const Connection & connection, __int64 adena );
	void set_last_target_id( const Connection & connection, unsigned int id );
	void add_damage( const Connection & connection, __int64 damage );
	void set_damage_type( const Connection & connection, DamageType type );

	void reset_xp( Character * pCharacter );
	void reset_sp( Character * pCharacter );
	void reset_adena( Character * pCharacter );
	void reset_damage( Character * pCharacter );
	void reset_all( Character * pCharacter );

	void stop_combat_mode( const Connection & connection ); 
	void set_combat_mode( Character * pCharacter, bool combat_mode );

	void log_out( const Connection & connection );

	void start_capture();
	void start_replay();
	void change_mode( bool bSniffer_mode );

	void set_dump( bool bDump );
	void set_log( bool bLog );
	void calc_session_date_time();
	void calc_processor();

	std::string m_dump_source;

protected:
	typedef std::map<Connection, std::shared_ptr<Character> > Character_map;
	typedef std::map<Connection, std::shared_ptr<net::IPacket_processor> > Stream_map;

	Character_map m_characters;
	Stream_map m_streams;
	std::recursive_mutex m_mutex;
	std::shared_ptr<std::vector<std::thread>> m_pGroup;
	std::string m_session_date_time;
	std::shared_ptr<Gx::io::OutputStream> m_dump;
	std::shared_ptr<net::IPacket_processor> m_processor;
	bool m_bSniffer_mode;
	bool m_bLog;
	bool m_bDump;
	bool m_stop_threads = false;

	std::shared_ptr<Character> get_character( const Connection & connection );

	void capture_thread( pcap_if_t * d, std::condition_variable & init_condition );
	void replay_thread();

	void pcap_packet_handler( const struct pcap_pkthdr * header, const u_char * pkt_data );
	void packet_handler( const unsigned char * pkt_data, unsigned int pkt_len );
	void dispatch( const Connection & connection, const std::shared_ptr<net::tcp_ip_packet> & packet );
	void stop_threads();
};