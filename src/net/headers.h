#pragma once

namespace net {

/* 4 bytes IP address */
struct ip_address {
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
	unsigned char byte4;
};

/* IPv4 header */
struct ip_header {
	unsigned char ver_ihl;		// Version ( 4 bits ) + Internet header length ( 4 bits )
	unsigned char tos;			// Type of service 
	unsigned short tlen;			// Total length 
	unsigned short identification; // Identification
	unsigned short flags_fo;		// Flags ( 3 bits ) + Fragment offset ( 13 bits )
	unsigned char ttl;			// Time to live
	unsigned char proto;			// Protocol
	unsigned short crc;			// Header checksum
	ip_address saddr;		// Source address
	ip_address daddr;		// Destination address
	unsigned int op_pad;			// Option + Padding
};

/* UDP header*/
struct udp_header {
	unsigned short sport;			// Source port
	unsigned short dport;			// Destination port
	unsigned short len;			// Datagram length
	unsigned short crc;			// Checksum
};

struct tcp_header {
	unsigned short sport;
	unsigned short dport;
	unsigned long seqnum;
	unsigned long acknum;
	unsigned char hlen; // hlen 4 bits + reserved 4 bits
	unsigned char flags; // reserved 2 bits + control 6 bits
	unsigned short win;
	unsigned short cksum;
	unsigned short urgptr;
};

enum IP_Flags {
	IP_MORE_FRAGMENT = (1 << 0),
	IP_DONT_FRAGMENT = (1 << 1),
};

enum TCP_Flags {
	TCP_FIN = 0x01,
	TCP_SYN = 0x02,
	TCP_RST = 0x04,
	TCP_PSH = 0x08,
	TCP_ACK = 0x10,
	TCP_URG = 0x20,
};

} // namespace net
