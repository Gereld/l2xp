#pragma once

#include "Endpoint.h"

class Connection {
public:
	Connection( const Endpoint & source, const Endpoint & destination ) 
		: m_source( source )
		, m_destination( destination )
	{
	}

	bool operator <( const Connection & rhs ) const {
		if( m_source == rhs.m_source ) {
			return m_destination < rhs.m_destination;
		}
		return m_source < rhs.m_source;
	}

protected:
	Endpoint m_source;
	Endpoint m_destination;
};