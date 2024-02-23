#pragma once

#include "Packet_filter.h"

#include <set>

namespace net {

class TCP_reassemble : public Packet_filter {
public:
	TCP_reassemble( const std::shared_ptr<IPacket_processor> & pNext );

	void process( const std::shared_ptr<tcp_ip_packet> & packet );

protected:
	struct less_sequence {
		bool operator ()( const std::shared_ptr<tcp_ip_packet> & lhs, const std::shared_ptr<tcp_ip_packet> & rhs ) const {
			if( lhs->tcp().sequence_number() == rhs->tcp().sequence_number() ) {
				if( lhs->tcp().acknowlegdment_number() == rhs->tcp().acknowlegdment_number() ) {
					return lhs->data_length() < rhs->data_length();
				}
				else {
					return lhs->tcp().acknowlegdment_number() < rhs->tcp().acknowlegdment_number();
				}
			}
			else {
				return lhs->tcp().sequence_number() < rhs->tcp().sequence_number();
			}
		}
	};

	typedef std::set<std::shared_ptr<tcp_ip_packet>, less_sequence > Stock;

	unsigned int m_expected_sequence;
	bool m_bSynchronized;
	Stock m_stock;

	void process_( const std::shared_ptr<tcp_ip_packet> & packet );
};

} // namespace net