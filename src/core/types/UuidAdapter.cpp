#include "Uuid_Adapter.h"
#include "../utils/CryptoHash.h"
#include <co/reserved/Uuid.h>

namespace co {

bool Uuid_Adapter::getIsNull( co::Uuid& instance )
{
	return instance.isNull();
}

void Uuid_Adapter::clear( co::Uuid& instance )
{
	instance.clear();
}

void Uuid_Adapter::createRandom( co::Uuid& instance )
{
	instance = co::Uuid::createRandom();
}

void Uuid_Adapter::createSha1( co::Uuid& instance, const std::string& data )
{
	co::CryptoHash hash( co::CryptoHash::Sha1 );
	hash.addData( data );

	co::CryptoHash::Result result;
	hash.getResult( result );

	result.toUuid( instance );
}

void Uuid_Adapter::getString( co::Uuid& instance, std::string& str )
{
	instance.getString( str );
}

void Uuid_Adapter::setString( co::Uuid& instance, const std::string& str )
{
	instance.setString( str );
}

} // namespace co
