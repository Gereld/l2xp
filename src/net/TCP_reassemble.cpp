#include "TCP_reassemble.h"

namespace net {

TCP_reassemble::TCP_reassemble( const std::shared_ptr<IPacket_processor> & pNext ) 
	: Packet_filter( pNext )
{
	m_expected_sequence = 0;
	m_bSynchronized = false;
}

void TCP_reassemble::process( const std::shared_ptr<tcp_ip_packet> & packet ) {
	if( !m_bSynchronized ) {
		if( packet->tcp().flags() & TCP_SYN ) {
			DBG_LOG( "TCP_SYN" << std::endl );
			m_expected_sequence = packet->tcp().sequence_number() + 1;
			DBG_LOG( "next seq: " << m_expected_sequence << std::endl );
			m_bSynchronized = true;
			__super::process( packet );
		}
		return;
	}

	if( packet->tcp().sequence_number() != m_expected_sequence ) {
		DBG_LOG( "TCP: OUT OF ORDER" << std::endl );

		if( packet->tcp().sequence_number() > m_expected_sequence ) {
			DBG_LOG( "TCP: DELAYED" << std::endl );
			// stocker
			m_stock.insert( packet );
		}
		else {
			DBG_LOG( "TCP: DISCARDED" << std::endl );
		}

		return;
	}

	process_( packet );

	if( packet->tcp().sequence_number() == 2525787825 ) {
		int i = 0;
	}

	if( !m_stock.empty() ) {
		DBG_LOG( "TCP: PROCESS DELAYED" << std::endl );
		auto i = m_stock.begin();
		for( ; i != m_stock.end(); ++i ) {
			std::shared_ptr<tcp_ip_packet> packet = *i;
			DBG_LOG( "seq: " << packet->tcp().sequence_number() << std::endl );

			if( packet->tcp().sequence_number() <= m_expected_sequence ) {
				if( packet->tcp().sequence_number() == m_expected_sequence ) {
					DBG_LOG( "TCP: GOOD" << std::endl );
					process_( packet );
				}
				else {
					DBG_LOG( "TCP: DISCARDED" << std::endl );
				}
			}
			else {
				break;
			}
		}

		if( i != m_stock.begin() ) {
			m_stock.erase( m_stock.begin(), i );
		}
	}
}

void TCP_reassemble::process_( const std::shared_ptr<tcp_ip_packet> & packet ) {
	if( packet->tcp().flags() & TCP_RST ) {
		DBG_LOG( "TCP_RST" << std::endl );
		m_expected_sequence += packet->data_length();
	}
	else if( packet->tcp().flags() & TCP_SYN ) {
		DBG_LOG( "TCP_SYN" << std::endl );
		m_expected_sequence = packet->tcp().sequence_number() + 1;
	}
	else {
		m_expected_sequence += packet->data_length();
	}
	DBG_LOG( "next seq: " << m_expected_sequence << std::endl );
	__super::process( packet );
}

} // namespace net