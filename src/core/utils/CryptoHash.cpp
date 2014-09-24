#include "CryptoHash.h"
#include "Sha1.h"
#include <co/IllegalStateException.h>
#include <cassert>
#include <sstream>

namespace co {

/************************************************************************/
/* Hash Algorithms													  */
/************************************************************************/

class CryptoHashAlgorithm
{
public:
	virtual ~CryptoHashAlgorithm()
	{;}

	virtual void addData( const uint8* data, size_t length ) = 0;
	virtual void getResult( CryptoHash::Result& res ) const = 0;

	virtual void reset() = 0;
	
protected:
	inline void assertNoResult()
	{
		if( _hasResult )
		{
			CORAL_THROW( IllegalStateException, "The hash result has already been calculated. "
						"You must reset the CryptoHash before re-using it." );
		}
	}

protected:
	mutable bool _hasResult;
};

class Sha1CryptoHashAlgorithm : public CryptoHashAlgorithm
{
public:
	Sha1CryptoHashAlgorithm()
	{
		reset();
	}

	virtual ~Sha1CryptoHashAlgorithm()
	{
		//empty;
	}

	virtual void addData( const uint8* data, size_t length )
	{
		assertNoResult();
		assert( length < UINT32_MAX );
		sha1::update( &_ctx, data, static_cast<co::uint32>( length ) );
	}

	virtual void getResult( CryptoHash::Result& res ) const
	{
		if( !_hasResult )
		{
			sha1::final( &_ctx, _result );
			_hasResult = true;
		}
		res.algorithm = CryptoHash::Sha1;
		res.numBytes = sha1::DIGEST_SIZE;
		std::copy( _result, _result + sha1::DIGEST_SIZE, res.data.bytes );
	}

	virtual void reset()
	{
		sha1::init( &_ctx );
		_hasResult = false;
	}

private:
	mutable sha1::Context _ctx;
	mutable uint8 _result[sha1::DIGEST_SIZE];
};

/************************************************************************/
/* CryptoHash														   */
/************************************************************************/
CryptoHash::CryptoHash( Algorithm method )
{
	switch( method )
	{
	case Sha1: 
		_algorithm = new Sha1CryptoHashAlgorithm(); 
		break;
	default:
		assert( false );
	}
}

CryptoHash::~CryptoHash()
{
	delete _algorithm;
}

void CryptoHash::Result::toString( std::string& str )
{
	static const char HEX_CHARS[16] = {
		'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};

	str.clear();
	str.reserve( 2 * numBytes );

	for( int i = 0; i < numBytes; ++i )
	{
		uint8 v = data.bytes[i];
		str += HEX_CHARS[v / 16];
		str += HEX_CHARS[v % 16];
	}
}
	
void CryptoHash::Result::toUuid( Uuid& uuid )
{
	uuid.set( data.bytes, Uuid::Sha1 );
}

void CryptoHash::addData( const uint8* data, size_t length )
{
	_algorithm->addData( data, length );
}

void CryptoHash::getResult( Result& res ) const
{
	_algorithm->getResult( res );
}

void CryptoHash::reset()
{
	_algorithm->reset();
}

CryptoHash::CryptoHash( const CryptoHash& other )
{
	*this = other;
}

CryptoHash& CryptoHash::operator=( const CryptoHash& other )
{
	this->_algorithm = other._algorithm;
	return *this;
}

} // namespace co
