#include "headers.h"

namespace net {

#define ADD_OPTION( FLAG, FLAG_VALUE, STR, STR_VALUE ) \
	if( FLAG & FLAG_VALUE ) { \
		if( !STR.empty() ) { STR += "|"; } \
		STR += STR_VALUE; \
	}

std::string ip_flags_to_str( unsigned char flags ) {
	std::string res;
	ADD_OPTION( flags, IP_MORE_FRAGMENT, res, "MF" );
	ADD_OPTION( flags, IP_DONT_FRAGMENT, res, "DF" );
	return res;
}

std::string tcp_flags_to_str( unsigned char flags ) {
	std::string res;
	ADD_OPTION( flags, TCP_FIN, res, "FIN" );
	ADD_OPTION( flags, TCP_SYN, res, "SYN" );
	ADD_OPTION( flags, TCP_RST, res, "RST" );
	ADD_OPTION( flags, TCP_PSH, res, "PSH" );
	ADD_OPTION( flags, TCP_ACK, res, "ACK" );
	ADD_OPTION( flags, TCP_URG, res, "URG" );
	return res;
}

} //namespace net
