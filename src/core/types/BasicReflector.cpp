#include "BasicReflector.h"
#include <co/Any.h>
#include <co/IType.h>
#include <co/NotSupportedException.h>

namespace co {

//------ BasicReflector -------------------------------------------------------

IType* BasicReflector::getType()
{
	return _type;
}

//------ NullReflector --------------------------------------------------------

class NullReflector : public BasicReflector
{
public:
	NullReflector( IType* t ) : BasicReflector( t ) {;}

	int32 getSize() { return 0; }
};

//------ PODReflector ---------------------------------------------------------

template<typename T>
class PODReflector : public BasicReflector
{
public:
	PODReflector( IType* t ) : BasicReflector( t ) {;}

	int32 getSize() { return sizeof(T); }

	void createValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			reinterpret_cast<T*>( ptr )[i] = 0;
	}

	void copyValues( const void* fromPtr, void* toPtr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			reinterpret_cast<T*>( toPtr )[i] = reinterpret_cast<const T*>( fromPtr )[i];
	}

	void destroyValues( void*, size_t )
	{
		// NOP
	}

	bool compareValues( const void* a, const void* b, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			if( reinterpret_cast<const T*>( a )[i] != reinterpret_cast<const T*>( b )[i] )
				return false;
		return true;
	}
};

//------ ClassReflector -------------------------------------------------------

template<typename T>
class ClassReflector : public BasicReflector
{
public:
	ClassReflector( IType* t ) : BasicReflector( t ) {;}

	int32 getSize() { return sizeof(T); }

	void createValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			new( reinterpret_cast<T*>( ptr ) + i ) T;
	}

	void copyValues( const void* fromPtr, void* toPtr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			reinterpret_cast<T*>( toPtr )[i] = reinterpret_cast<const T*>( fromPtr )[i];
	}

	void destroyValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			callDestructor( reinterpret_cast<T*>( ptr ) + i );
	}

	bool compareValues( const void* a, const void* b, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			if( reinterpret_cast<const T*>( a )[i] != reinterpret_cast<const T*>( b )[i] )
				return false;
		return true;
	}
};

//------ ArrayReflector -------------------------------------------------------

class ArrayReflector : public BasicReflector
{
public:
	typedef std::vector<int8> PseudoVector;

	ArrayReflector( IType* t ) : BasicReflector( t )
	{
		IType* elemType = static_cast<IArray*>( t )->getElementType();
		_elemReflector = elemType->getReflector();
		_elemSize = _elemReflector->getSize();
	}

	int32 getSize() { return sizeof(PseudoVector); }

	void createValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			new( reinterpret_cast<PseudoVector*>( ptr ) + i ) PseudoVector();
	}

	void copyValues( const void* fromPtr, void* toPtr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
		{
			Any toArray( false, _type, toPtr );
			const PseudoVector* fromV = reinterpret_cast<const PseudoVector*>( fromPtr );
			size_t numElements = sizeToElements( fromV->size() );
			toArray.resize( numElements );
			if( numElements > 0 )
				_elemReflector->copyValues( &fromV->front(), toArray[0].state.data.ptr, numElements );
		}
	}

	void destroyValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			destroyVector( reinterpret_cast<PseudoVector*>( ptr ) + i );
	}

	bool compareValues( const void* a, const void* b, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
		{
			const PseudoVector* pvA = reinterpret_cast<const PseudoVector*>( a ) + i;
			const PseudoVector* pvB = reinterpret_cast<const PseudoVector*>( b ) + i;
			size_t size = pvA->size();
			if( size != pvB->size() || !_elemReflector->compareValues( &pvA[0], &pvB[0], size ) )
				return false;
		}
		return true;
	}

private:
	inline size_t sizeToElements( size_t totalSize )
	{
		assert( totalSize % _elemSize == 0 );
		return totalSize / _elemSize;
	}

	PseudoVector* createVector( void* ptr, size_t numElements )
	{
		PseudoVector* vec = new( ptr ) PseudoVector( numElements * _elemSize );
		if( numElements > 0 )
			_elemReflector->createValues( &vec->front(), numElements );
		return vec;
	}

	void destroyVector( void* ptr )
	{
		PseudoVector* vec = reinterpret_cast<PseudoVector*>( ptr );
		size_t totalSize = vec->size();
		if( totalSize > 0 )
		{
			size_t numElements = sizeToElements( totalSize );
			_elemReflector->destroyValues( &vec->front(), numElements );
		}
		vec->~vector();
	}

private:
	size_t _elemSize;
	RefPtr<IReflector> _elemReflector;
};

//------ InternalComponentReflector -------------------------------------------

class InternalComponentReflector : public BasicReflector
{
public:
	InternalComponentReflector( IType* t ) : BasicReflector( t ) {;}
	
	int32 getSize() { return 0; }

	IObject* newInstance()
	{
		throw NotSupportedException( "cannot instantiate an internal component" );
	}
};

//------ BasicReflector Factory Method ----------------------------------------

IReflector* BasicReflector::create( IType* t )
{
	assert( t );
	switch( t->getKind() )
	{
	case TK_NULL:		return new NullReflector( t );
	case TK_BOOL:		return new PODReflector<bool>( t );
	case TK_INT8:		return new PODReflector<int8>( t );
	case TK_INT16:		return new PODReflector<int16>( t );
    case TK_INT32:		return new PODReflector<int32>( t );
    case TK_INT64:		return new PODReflector<int64>( t );
	case TK_DOUBLE:		return new PODReflector<double>( t );
	case TK_ENUM:		return new PODReflector<int16>( t );
	case TK_STRING:		return new ClassReflector<std::string>( t );
	case TK_ANY:		return new ClassReflector<AnyValue>( t );
	case TK_ARRAY:		return new ArrayReflector( t );
	case TK_COMPONENT:	return new InternalComponentReflector( t );
	default:
		assert( false );
	}
	throw NotSupportedException( "unexpected type kind" );
}

} // namespace co
