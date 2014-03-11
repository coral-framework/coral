#include "Uuid.h"
#include <co/Endian.h>
#include <cstdio>
#include <ostream>

#ifdef CORAL_OS_WIN
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <wincrypt.h>
#endif

namespace co {

inline void generateRandomBytes( uint8* data )
{
#if defined(CORAL_OS_WIN)
	HCRYPTPROV cryptProvider;
	if( CryptAcquireContext( &cryptProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
		if( CryptGenRandom( cryptProvider, 16, data ) )
			if( CryptReleaseContext( cryptProvider, 0 ) )
				return;
	assert( false );
	for( unsigned int i = 0; i < 16; ++i )
		data[i] = 0;
#elif defined(CORAL_OS_UNIX)
	FILE *fd = fopen( "/dev/urandom", "r" );
	for( unsigned int i = 0; i < 16; ++i )
		data[i] = static_cast<uint8>( fgetc( fd ) );
	fclose( fd );
#else
#error "No PRNG defined for this platform!"
#endif
}

Uuid Uuid::createRandom()
{
	Uuid uuid;
	generateRandomBytes( uuid._data.bytes );
	uuid.setVersion( Random );
	return uuid;
}

const Uuid& Uuid::null()
{
	static const Uuid s_null;
	assert( s_null.isNull() );
	return s_null;
}

Uuid::Uuid()
{
	clear();
}

Uuid::Uuid( const Uuid &other )
{
	std::copy( other._data.dwords, other._data.dwords + 4, _data.dwords );
}

Uuid::Uuid( uint32 timeLow, uint16 timeMid, uint16 timeHiAndVersion, uint8 clockSeqHiAndReserved, uint8 clockSeqLow, uint8 node[6] )
{
	_data.dce.timeLow = toBigEndian( timeLow );
	_data.dce.timeMid = toBigEndian( timeMid );
	_data.dce.timeHiAndVersion = toBigEndian( timeHiAndVersion );
	_data.dce.clockSeqHiAndReserved = clockSeqHiAndReserved;
	_data.dce.clockSeqLow = clockSeqLow;
	_data.dce.node[0] = node[5];
	_data.dce.node[1] = node[4];
	_data.dce.node[2] = node[3];
	_data.dce.node[3] = node[2];
	_data.dce.node[4] = node[1];
	_data.dce.node[5] = node[0];
}

Uuid::Uuid( const std::string& text )
{
	setString( text );
}

bool Uuid::isNull() const
{
	for( int i = 0; i < 4; i++ )
		if( _data.dwords[i] != 0 )
			return false;
	return true;
}

int Uuid::getVersion() const
{
	if( isNull() )
		return Uuid::VerUnknown;

	return ( _data.dce.timeHiAndVersion & 0x000F );
}

void Uuid::set( uint8 bytes[], Version v )
{
	std::copy( bytes, bytes + 16, _data.bytes );
	setVersion( v );
}

void Uuid::getString( std::string& str ) const
{
	str.clear();
	str.reserve( 37 );

	static const char HEX_CHARS[16] = {
		'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};

	// using the defacto standard format, which is 8-4-4-16
	for( int i = 0; i < 16; ++i )
	{
		uint8 v = _data.bytes[i];
		str.push_back( HEX_CHARS[v / 16] );
		str.push_back( HEX_CHARS[v % 16] );
		if( i == 3 || i == 5 || i == 7 )
			str.push_back( '-' );
	}
}

inline int isHex( const char c )
{
	return ( c >= '0' && c <= '9' ) || ( c >= 'A' && c <= 'F' ) || ( c >= 'a' && c <= 'f' );
}

inline uint8 hex2nibble( uint8 c )
{
	if( c >= '0' && c <= '9' )
		return c - '0';
	if( c >= 'a' && c <= 'f' )
		return ( c - 'a' + 10 );
	if( c >= 'A' && c <= 'F' )
		return ( c - 'A' + 10 );
	assert( false );
	return 0;
}

inline uint8 hex2byte( uint8 c1, uint8 c2 )
{
	return ( hex2nibble( c1 ) << 4 ) | hex2nibble( c2 ) ;
}

void Uuid::setString( const std::string& str )
{
	const char* first = str.c_str();
	const char* last = str.c_str() + str.length() - 1;

	// ignore curly braces at the beginning/end of the string
	if( *first == '{' ) ++first;
	if( last > first && *last == '}' ) --last;

	// number of chars in a UUID string
	static const uint8 NUM_CHARS = 35;

	if( ( last - first + 1 ) == NUM_CHARS )
	{
		uint8 i = 0, bytesRead = 0;
		while( true )
		{
			// positions where we expect a '-'
			if( i == 8 || i == 13 || i == 18 )
			{
				if( first[i] != '-' )
					break; // malformed string
				++i;
				continue;
			}

			uint8 hexChar1 = first[i++];
			uint8 hexChar2 = first[i++];

			if( !isHex( hexChar1 ) || !isHex( hexChar2 ) )
				break;

			_data.bytes[bytesRead++] = hex2byte( hexChar1, hexChar2 );

			if( bytesRead == 16 )
				return;

			assert( i < NUM_CHARS );
		}
	}

	// malformed
	clear();
}

void Uuid::clear()
{
	std::fill( _data.dwords, _data.dwords + 4, 0 );
}

Uuid& Uuid::operator=( const Uuid& other )
{
	std::copy( other._data.dwords, other._data.dwords + 4, _data.dwords );
	return *this;
}

void Uuid::setVersion( Version version )
{
	// set version
	_data.dce.timeHiAndVersion &= 0xFFF0;
	_data.dce.timeHiAndVersion |= version;
}

} // namespace co

std::ostream& operator<<( std::ostream& out, const co::Uuid& uuid )
{
	std::string str;
	uuid.getString( str );
	return out << str;
}
