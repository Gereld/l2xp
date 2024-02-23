#pragma once

#include <string>
#include <iostream>

namespace dbg {

std::ostream * get_stream();
void init_stream( const std::string & name );

#define DBG_LOG_INIT(X) { dbg::init_stream( X ); }
#define DBG_LOG(Y) { std::ostream * pStream = dbg::get_stream(); if( pStream ) { (*pStream) << Y; pStream->flush(); } }

} // namespace
