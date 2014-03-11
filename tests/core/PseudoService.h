#ifndef _PSEUDOSERVICE_H_
#define _PSEUDOSERVICE_H_

#include <co/IService.h>

class PseudoService : public co::IService
{
public:
	PseudoService( bool* setWhenDestroyed = nullptr ) :
		_setWhenDestroyed( setWhenDestroyed ), _refCount( 0 )
	{;}

	virtual ~PseudoService()
	{
		if( _setWhenDestroyed )
			*_setWhenDestroyed = true;
	}

	inline co::int32 getRefCount() const { return _refCount; }

	co::IInterface* getInterface() { return 0; }
	co::IObject* getProvider() { return 0; }
	co::IPort* getFacet() { return 0; }
	void serviceRetain() { ++_refCount; }
	void serviceRelease() { if( --_refCount <= 0 ) delete this; }

private:
	bool* _setWhenDestroyed;
	co::int32 _refCount;
};

#endif // _PSEUDOSERVICE_H_
