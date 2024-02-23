#include "dbg.h"

#include <fstream>

namespace dbg {

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

using Stream_handle = std::shared_ptr<std::ostream>;
Stream_handle g_stream;
static std::string g_file_name;

std::ostream * get_stream() {
	if( !g_stream ) {
		if( !g_file_name.empty() ) {
			g_stream = Stream_handle( new std::ofstream( g_file_name.c_str() ) );
		}
	}
	return g_stream.get();
}

void init_stream( const std::string & name ) {
	g_file_name = name;
	if( g_file_name.empty() ) {
		g_stream.reset();
	}
}

} // namespace dbg
