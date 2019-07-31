#pragma once
#include <string>

namespace TU
{

class CTableUtil
{
public:
	static std::string SqliteDbPath();
	static std::string StringFormat( const char * _Format, ...);
	static std::string StringTransf( const std::string & sVal );
};

} // namespace TU