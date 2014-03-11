#include "RefCounted.h"
#include <co/Log.h>

namespace co {

RefCounted::~RefCounted()
{
	if( _refCount != 0 )
	{
		CORAL_LOG(FATAL) << "Deleting object " << this <<
			" with a reference count of " << _refCount << ".";
	}
}

} // namespace co
