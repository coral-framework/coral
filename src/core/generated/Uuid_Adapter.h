
#ifndef _CO_UUID_ADAPTER_H_
#define _CO_UUID_ADAPTER_H_

#include <co/reserved/Uuid.h> // added by co.Include()

#include <co/Uuid.h>

namespace co {

/*!
	Implement this class to adapt 'co::Uuid' to the Coral type system.
 */
class Uuid_Adapter
{
public:
	//------ Fields ------

	static bool getIsNull( co::Uuid& instance );

	//------ Methods ------

	static void clear( co::Uuid& instance );
	static void createRandom( co::Uuid& instance );
	static void createSha1( co::Uuid& instance, const std::string& data );
	static void getString( co::Uuid& instance, std::string& str );
	static void setString( co::Uuid& instance, const std::string& str );
};

} // namespace co

#endif // _CO_UUID_ADAPTER_H_
