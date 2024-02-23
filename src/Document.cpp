#include "Document.h"

#include "l2/PacketHandler.h"
#include "MainFrame.h"
#include "net/headers.h"
#include "net/ip_header_helper.h"
#include "net/tcp_header_helper.h"
#include "net/Packet_dump.h"
#include "net/TCP_reassemble.h"
#include "Packet_log.h"
#include "Packet_bridge.h"

#include <pcap/pcap.h>
#include <Winsock2.h>
#include <thread>
#include <Gx/io/FileOutputStream.h>
#include <Gx/io/FileInputStream.h>
#include <Gx/io/DataInputStream.h>
#include <Gx/io/EOFException.h>
#include <mutex>
#include <boost/bind.hpp>

// ************************************************************************************************

Document::Document() {
	m_pFrame = nullptr;
	m_bSniffer_mode = false;
	m_bLog = false;
	m_bDump = false;
}

Document::~Document() {
	stop_threads();
}

void Document::set_name( const Connection & connection, const std::string & name ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->set_name( name );
	m_pFrame->update_character( pCharacter.get() );
}

void Document::add_xp( const Connection & connection, __int64 xp ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->add_xp( xp );
	m_pFrame->update_character( pCharacter.get() );
}

void Document::add_sp( const Connection & connection, int sp ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->add_sp( sp );
	m_pFrame->update_character( pCharacter.get() );
}

void Document::add_adena( const Connection & connection, __int64 adena ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->add_adena( adena );
	m_pFrame->update_character( pCharacter.get() );
}

void Document::set_last_target_id( const Connection & connection, unsigned int id ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->set_last_target_id( id );
	m_pFrame->update_character( pCharacter.get() );
}

void Document::add_damage( const Connection & connection, __int64 damage ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->add_damage( damage );
	m_pFrame->update_character( pCharacter.get() );
}

void Document::set_damage_type( const Connection & connection, DamageType type ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->set_damage_type( type );
	m_pFrame->update_character( pCharacter.get() );
}

void Document::reset_xp( Character * pCharacter ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	pCharacter->reset_xp();
	m_pFrame->update_character( pCharacter );
}

void Document::reset_sp( Character * pCharacter ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	pCharacter->reset_sp();
	m_pFrame->update_character( pCharacter );
}

void Document::reset_adena( Character * pCharacter ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	pCharacter->reset_adena();
	m_pFrame->update_character( pCharacter );
}

void Document::reset_damage( Character * pCharacter ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	pCharacter->reset_damage();
	m_pFrame->update_character( pCharacter );
}

void Document::reset_all( Character * pCharacter ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	pCharacter->reset_all();
	m_pFrame->update_character( pCharacter );
}

void Document::stop_combat_mode( const Connection & connection ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	std::shared_ptr<Character> pCharacter = get_character( connection );
	pCharacter->stop_combat_mode();
	m_pFrame->update_character( pCharacter.get() );
}

void Document::set_combat_mode( Character * pCharacter, bool combat_mode ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	pCharacter->set_combat_mode( combat_mode );
	m_pFrame->update_character( pCharacter );
}

void Document::log_out( const Connection & connection ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );
	Character_map::iterator pos = m_characters.find( connection );
	if( pos != m_characters.end() ) {
		m_pFrame->remove_character( pos->second.get() );
		m_characters.erase( pos );
	}
}

std::shared_ptr<Character> Document::get_character( const Connection & connection ) {
	std::unique_lock<std::recursive_mutex> lock( m_mutex );

	Character_map::iterator pos = m_characters.find( connection );
	if( pos == m_characters.end() ) {
		DBG_LOG( "NEW CHARACTER" << std::endl );
		std::shared_ptr<Character> pCharacter = std::make_shared<Character>();
		m_characters[connection] = pCharacter;
		return pCharacter;
	}
	else {
		return pos->second;
	}
}

// ************************************************************************************************

void Document::start_capture() {
	stop_threads();
	m_bSniffer_mode = true;

	char errbuf[PCAP_ERRBUF_SIZE];
	
	/* Retrieve the device list */
	pcap_if_t * alldevs = nullptr;
	if( pcap_findalldevs( &alldevs, errbuf ) == -1 ) {
		::MessageBox( NULL, "Error in pcap_findalldevs.", "Error", MB_OK );
		//fprintf( stderr, "Error in pcap_findalldevs: %s\n", errbuf );
		return;
	}
	
	if( alldevs == NULL ) {
		::MessageBox( NULL, "No interfaces found! Make sure WinPcap is installed.", "Error", MB_OK );
		return;
	}

	std::condition_variable init_condition;

	m_pGroup = std::make_shared<std::vector<std::thread>>();

	size_t nThread = 0;
	for( pcap_if_t * d = alldevs; d ; d = d->next ) {
		m_pGroup->push_back( std::thread( boost::bind( &Document::capture_thread, this, d, std::ref( init_condition ) ) ) );
		nThread++;
	}
	
	/*
	// Wait until all devices are opened.
	{
		boost::mutex mtx;
		std::unique_lock<boost::mutex> lock( mtx );
		for( size_t i = 0; i < nThread; ++i ) {
			init_condition.wait( lock );
		}

		//std::cout << "Initialisation done." << std::endl;
		pcap_freealldevs( alldevs );
	}
	*/

	//m_group.join_all();
}

void Document::start_replay() {
	stop_threads();
	m_bSniffer_mode = false;

	if( !m_dump_source.empty() )  {
		m_pGroup = std::make_shared<std::vector<std::thread>>();
		m_pGroup->push_back( std::thread( boost::bind( &Document::replay_thread, this ) ) );
	}
}

void Document::stop_threads() {
	if( m_pGroup ) {
		m_stop_threads = true;
		//m_pGroup->interrupt_all();
		//m_pGroup->join_all();
		for( auto & thread : *m_pGroup ) {
			thread.join(); 
		}
		m_pGroup.reset();
		m_stop_threads = false;
	}
}

void Document::capture_thread( pcap_if_t * d, std::condition_variable & init_condition ) {
	//std::cout << "Waiting for connection on : " << d->description << std::endl;

	char errbuf[PCAP_ERRBUF_SIZE];

	/* Open the device */
	/* Open the adapter */
	pcap_t * adhandle = pcap_open_live( 
		d->name,	// name of the device
		65536,			// portion of the packet to capture. 
		// 65536 grants that the whole packet will be captured on all the MACs.
		1,				// promiscuous mode ( nonzero means promiscuous )
		1000,			// read timeout
		errbuf			// error buffer
	);

	if( adhandle == nullptr ) {
		//::MessageBox( NULL "Unable to open the adapter. %s is not supported by WinPcap\n", d->name );
	}
	
	for( ; adhandle != nullptr; ) {
		const char * filter = "ip proto \\tcp && (src port 7777)";

		// We should loop through the adapters returned by the pcap_findalldevs_ex()
		// in order to locate the correct one.
		//
		// Let's do things simpler: we suppose to be in a C class network ;- )
		bpf_u_int32 net_mask = 0xffffff;

		struct bpf_program fcode;

		//compile the filter
		if( pcap_compile( adhandle, &fcode, filter, 1, net_mask ) < 0 ) {
			//fprintf( stderr, "Error compiling filter: wrong syntax.\n" );

			pcap_close( adhandle );
			break;
		}

		//set the filter
		if( pcap_setfilter( adhandle, &fcode ) < 0 ) {
			//fprintf( stderr, "Error setting the filter\n" );

			pcap_close( adhandle );
			break;
		}

		break;
	}
	
	// Initialisation done.
	//init_condition.notify_one();

	// start the capture
	struct pcap_pkthdr * header;
	const u_char * pkt_data;
	int res;

	//try {
		while( !m_stop_threads && (res = pcap_next_ex( adhandle, &header, &pkt_data )) >= 0 ) {
			//std::this_thread::interruption_point();

			if( res == 0 ) {
				// Timeout elapsed
				continue;
			}

			pcap_packet_handler( header, pkt_data );
		}
	//}
	//catch( std::thread_interrupted & ) {
	//}
 
    if( res == -1  ){
        //printf( "Error reading the packets: %s\n", pcap_geterr( adhandle ) );
    }

	pcap_close( adhandle );
}

void Document::pcap_packet_handler( const struct pcap_pkthdr * header, const u_char * pkt_data ) {
    struct tm ltime;
    char timestr[16];
    time_t local_tv_sec;

    // convert the timestamp to readable format
    local_tv_sec = header->ts.tv_sec;
    localtime_s( &ltime, &local_tv_sec );
    strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime );

    // print timestamp of the packet
    //printf( "%s.%.6d ", timestr, header->ts.tv_usec );

    // print length of the packet
    //printf( "len:%d ", header->len );

    // retrieve the position of the ip header
	packet_handler( pkt_data + 14, header->len - 14 ); // skip the mac header
}

void Document::dispatch( const Connection & connection, const std::shared_ptr<net::tcp_ip_packet> & packet ) {
	std::shared_ptr<net::IPacket_processor> proc;

	Stream_map::iterator pos = m_streams.find( connection );
	if( pos == m_streams.end() ) {
		DBG_LOG( "NEW CONNECTION" << std::endl );
		std::shared_ptr<l2::PacketHandler> pHandler = std::make_shared<l2::PacketHandler>( connection );
		std::shared_ptr<l2::PacketExtractor> pExtractor = std::make_shared<l2::PacketExtractor>( pHandler );
		proc = std::make_shared<Packet_bridge>( pExtractor );
		proc = std::make_shared<net::TCP_reassemble>( proc );
		m_streams[connection] = proc;
	}
	else {
		proc = pos->second;
	}

	proc->process( packet );
}

void Document::replay_thread() {
	std::shared_ptr<Gx::io::FileInputStream> fis = std::make_shared<Gx::io::FileInputStream>( m_dump_source.c_str() );
	std::shared_ptr<Gx::io::DataInputStream> dis = std::make_shared<Gx::io::DataInputStream>( fis );

	try {
		while( !m_stop_threads ) {
			//boost::this_thread::interruption_point();

			int len = dis->ReadInt();

			std::vector<unsigned char> buffer( len );
			dis->Read( (char *) buffer.data(), len );

			packet_handler( buffer.data(), len );
		}
	}
	catch( Gx::io::EOFException & ) {
	}
	/*
	catch( boost::thread_interrupted & ) {
	}
	*/

	::MessageBox( NULL, "Done", "", MB_OK );
}

void Document::packet_handler( const unsigned char * pkt_data, unsigned int pkt_len ) {
	net::ip_header_helper ih( pkt_data ); // length of ethernet header

	// version should be 4
	if( ih.version() != 4 ) {
		//printf( "Received non IP4 packet : %d\n", ip_ver );
		return;
	}

	// protocol should be TCP
	if( ih.protocol() != 6 ) {
		//printf( "Received non TCP packet\n" );
		return;
	}

	unsigned short total_len = ih.total_length();
	//printf( "tlen:%d ", total_len );

    // retrieve the position of the tcp header
	unsigned int ip_len = ih.header_length();

	net::tcp_header_helper th( ih.data() );
	unsigned int tcp_len = th.header_length();

    // convert from network byte order to host byte order
    unsigned short sport = th.source_port();
    unsigned short dport = th.destination_port();

	const unsigned char * data = th.data();
	unsigned short data_len = total_len - tcp_len - ip_len;

	std::shared_ptr<net::tcp_ip_packet> packet = std::make_shared<net::tcp_ip_packet>( ih.ptr(), total_len );

	calc_processor();
	if( m_processor ) {
		m_processor->process( packet );
	}

	Connection connection( 
		Endpoint( 
			ih.source_address().byte1,
			ih.source_address().byte2,
			ih.source_address().byte3,
			ih.source_address().byte4,
			sport
		),
		Endpoint( 
			ih.destination_address().byte1,
			ih.destination_address().byte2,
			ih.destination_address().byte3,
			ih.destination_address().byte4,
			dport
		)
	);

	dispatch( connection, packet );
}

void Document::change_mode( bool bSniffer_mode ) {
	if( bSniffer_mode != bSniffer_mode ) {
		if( bSniffer_mode ) {
			start_capture();
		}
		else {
			start_replay();
		}
	}
}

void Document::set_dump( bool bDump ) {
	if( bDump ) {
		calc_session_date_time();
		m_dump = std::make_shared<Gx::io::FileOutputStream>( std::string( "dump-" + m_session_date_time + ".bin" ).c_str() );
	}
	else {
		m_session_date_time.clear();
		m_dump.reset();
	}

	m_bDump = bDump;
	m_processor.reset();
}

void Document::set_log( bool bLog ) {
	if( bLog ) {
		calc_session_date_time();
		DBG_LOG_INIT( "log-" + m_session_date_time + ".txt" )
	}
	else {
		m_session_date_time.clear();
		DBG_LOG_INIT( std::string() )
	}

	m_bLog = bLog;
	m_processor.reset();
}

void Document::calc_session_date_time() {
	if( m_session_date_time.empty() ) {
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		boost::gregorian::date date = now.date();
		boost::posix_time::time_duration time = now.time_of_day();

		std::stringstream ss;
		ss << std::setfill( ss.widen( '0' ) ); 
		ss << boost::gregorian::to_iso_extended_string( date );
		ss << "-" << std::setw( 2 ) << time.hours();
		ss << "-" << std::setw( 2 ) << time.minutes();
		ss << "-" << std::setw( 2 ) << time.seconds();

		m_session_date_time = ss.str();
	}
}

void Document::calc_processor() {
	if( !m_processor ) {
		if( m_bDump ) {
			m_processor = std::make_shared<net::Packet_dump>( m_dump, m_processor );
		}
		if( m_bLog ) {
			m_processor = std::make_shared<Packet_log>( m_processor );
		}
	}
}
