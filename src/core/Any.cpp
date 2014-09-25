#include "Any.h"
#include <co/Slice.h>
#include <co/IEnum.h>
#include <co/IType.h>
#include <co/IField.h>
#include <co/INamespace.h>
#include <co/IReflector.h>
#include <co/IInterface.h>
#include <co/IllegalCastException.h>
#include <co/IllegalStateException.h>
#include <cstdlib>
#include <sstream>

namespace co {

/****************************************************************************/
/* Casting Between Scalar Types                                             */
/****************************************************************************/

template<typename From, typename To>
inline void cast( const void* from, void* to )
{
	*reinterpret_cast<To*>( to ) = static_cast<To>( *reinterpret_cast<const From*>( from ) );
}

template<typename To>
inline void castTo( TypeKind fromKind, const void* from, void* to )
{
	switch( fromKind )
	{
	case TK_BOOL:	cast<bool, To>( from, to );		break;
	case TK_INT8:	cast<int8, To>( from, to );		break;
	case TK_INT16:	cast<int16, To>( from, to );	break;
	case TK_INT32:	cast<int32, To>( from, to );	break;
	case TK_UINT8:	cast<uint8, To>( from, to );	break;
	case TK_UINT16:	cast<uint16, To>( from, to );	break;
	case TK_UINT32:	cast<uint32, To>( from, to );	break;
	case TK_FLOAT:	cast<float, To>( from, to );	break;
	case TK_DOUBLE:	cast<double, To>( from, to );	break;
	case TK_ENUM:	cast<uint32, To>( from, to );	break;
	default:		assert( false );
	}
}

template<typename From>
inline void castFrom( const void* from, TypeKind toKind, void* to )
{
	switch( toKind )
	{
	case TK_BOOL:	cast<From, bool>( from, to );	break;
	case TK_INT8:	cast<From, int8>( from, to );	break;
	case TK_INT16:	cast<From, int16>( from, to );	break;
	case TK_INT32:	cast<From, int32>( from, to );	break;
	case TK_UINT8:	cast<From, uint8>( from, to );	break;
	case TK_UINT16:	cast<From, uint16>( from, to );	break;
	case TK_UINT32:	cast<From, uint32>( from, to );	break;
	case TK_FLOAT:	cast<From, float>( from, to );	break;
	case TK_DOUBLE:	cast<From, double>( from, to );	break;
	case TK_ENUM:	cast<From, uint32>( from, to );	break;
	default:		assert( false );
	}
}

void castScalar( TypeKind fromKind, const void* from, TypeKind toKind, void* to )
{
	assert( isScalar( toKind ) && isScalar( fromKind ) );
	switch( fromKind )
	{
	case TK_BOOL:	castFrom<bool>( from, toKind, to );		break;
	case TK_INT8:	castFrom<int8>( from, toKind, to );		break;
	case TK_INT16:	castFrom<int16>( from, toKind, to );	break;
	case TK_INT32:	castFrom<int32>( from, toKind, to );	break;
	case TK_UINT8:	castFrom<uint8>( from, toKind, to );	break;
	case TK_UINT16:	castFrom<uint16>( from, toKind, to );	break;
	case TK_UINT32:	castFrom<uint32>( from, toKind, to );	break;
	case TK_FLOAT:	castFrom<float>( from, toKind, to );	break;
	case TK_DOUBLE:	castFrom<double>( from, toKind, to );	break;
	case TK_ENUM:	castFrom<uint32>( from, toKind, to );	break;
	default:		assert( false );
	}
}

/****************************************************************************/
/* General Utility Functions                                                */
/****************************************************************************/

template<typename T>
inline bool isEqual( const void* p1, const void* p2 )
{
	return *reinterpret_cast<const T*>( p1 ) == *reinterpret_cast<const T*>( p2 );
}

template<typename T>
inline void copy( const void* from, void* to )
{
	*reinterpret_cast<T*>( to ) = *reinterpret_cast<T const*>( from );
}

bool isTrue( const std::string& str )
{
	return str == "true";
}

void streamOut( std::ostream& os, const co::Any& var )
{
	TypeKind kind = var.getKind();
	switch( kind )
	{
	case TK_NULL:	os << "null"; break;
	case TK_BOOL:
		{
			bool v = var.isIn() ? var.state.data.b : var.state.data.deref->b;
			os << ( v ? "true" : "false" );
		}
		break;

	case TK_INT8:
	case TK_INT16:
	case TK_INT32:
	case TK_UINT8:
	case TK_UINT16:
	case TK_UINT32:
	case TK_FLOAT:
	case TK_DOUBLE:
		{
			double scalar;
			castTo<double>( kind, var.isIn() ? &var.state.data : var.state.data.ptr, &scalar );

			#ifdef CORAL_OS_WINDOWS
				// makes windows compatible with the rest of the world
				_set_output_format( _TWO_DIGIT_EXPONENT );
			#endif

			// 16 digits, sign, point, and \0
			char buffer[32];
			sprintf( buffer, "%.15g", scalar );
			os << buffer;
		}
		break;

	case TK_ENUM:
		{
			uint32 id = var.state.data.u32;
			TSlice<std::string> ids = static_cast<IEnum*>( var.getType() )->getIdentifiers();
			if( id < ids.getSize() )
				os << ids[id];
			else
				os << "<INVALID ID #" << id << ">";
		}
		break;

	case TK_STRING:
		os << '"' << *var.state.data.str << '"';
		break;

	case TK_ANY: streamOut( os, var.asIn() ); break;

	case TK_ARRAY:
		{
			Any in = var.asIn();
			os << "{";
			for( size_t i = 0; i < in.state.size; ++i )
			{
				if( i > 0 ) os << ", ";
				streamOut( os, in[i] );
			}
			os << "}";
		}
		break;

	case TK_STRUCT:
	case TK_NATIVECLASS:
		os << "{";
		{
			IRecordType* recordType = static_cast<IRecordType*>( var.getType() );
			IReflector* reflector = recordType->getReflector();
			TSlice<IField*> fields = recordType->getFields();
			AnyValue v;
			for( size_t i = 0; i < fields.getSize(); ++i )
			{
				if( i > 0 ) os << ", ";
				IField* f = fields[i];
				os << f->getName() << " = ";
				reflector->getField( var, f, v );
				streamOut( os, v.getAny() );
			}
		}
		os << "}";
		break;

	case TK_INTERFACE:
		{
			void* ptr = var.isIn() ? var.state.data.ptr : var.state.data.deref->ptr;
			if( ptr )
				os << '@' << ptr;
			else
				os << "null";
		}
		break;

	default:
		assert( false );
	}
}

typedef std::vector<uint8> StdVector;

inline co::uint8* getVectorData( void* ptr  )
{
	StdVector& pv = *reinterpret_cast<StdVector*>( ptr );
#if defined(CORAL_CC_MSVC)
	return pv.empty() ? NULL : &pv[0]; // MSVC forbids &v[0] for empty vectors
#else
	return &pv[0];
#endif
}

inline co::uint8* getVectorData( const __any::State& s )
{
	return getVectorData( s.data.ptr );
}

inline size_t getVectorSize( const __any::State& s )
{
	StdVector& v = *reinterpret_cast<StdVector*>( s.data.ptr );
	if( v.empty() )
		return 0;
	IType* elemType = static_cast<IArray*>( s.type )->getElementType();
	return v.size() / elemType->getReflector()->getSize();
}

#define THROW_NO_CONVERSION( from, to, MORE_MESSAGES ) \
	CORAL_THROW(IllegalCastException, "no conversion from '" << from << "' to '" << to << "'" MORE_MESSAGES )

/****************************************************************************/
/* co::Any                                                                  */
/****************************************************************************/

Any Any::asIn() const
{
	if( isIn() )
		return *this;

	switch( getKind() )
	{
	case TK_NULL:
		return Any();

	case TK_ANY:
		return reinterpret_cast<AnyValue*>( state.data.ptr )->getAny().asIn();

	case TK_ARRAY:
		return Any( true, state.type, getVectorData( state ), getVectorSize( state ) );

	case TK_INTERFACE:
		return Any( state.data.deref->service );

	default:
		return Any( true, state.type, state.data.ptr );
	}
}

bool Any::equals( const Any& other ) const
{
	Any a( asIn() ), b( other.asIn() );
	if( a.getType() != b.getType() )
		return false;

	TypeKind kind = a.getKind();
	if( kind == TK_ARRAY )
	{
		size_t count = a.state.size;
		if( count != b.state.size )
			return false;

		IType* elemType = static_cast<IArray*>( a.getType() )->getElementType();
		return elemType->getReflector()->compareValues( a.state.data.ptr, b.state.data.ptr, count );
	}

	__any::Data *pA( &a.state.data ), *pB( &b.state.data );
	if( !isScalarOrRef( kind ) )
	{
		pA = pA->deref;
		pB = pB->deref;
	}

	return a.getType()->getReflector()->compareValues( pA, pB, 1 );
}

size_t Any::getCount() const
{
	if( getKind() == TK_ARRAY )
		return state.isIn ? state.size : getVectorSize( state );

	return 1;
}

size_t Any::getElementSize() const
{
	IType* type = getType();
	if( type->getKind() == TK_ARRAY )
		type = static_cast<IArray*>( type )->getElementType();

	return type->getReflector()->getSize();
}

void Any::set( bool isIn, IType* type, const void* addr, size_t size )
{
	// type must be a valid data type
	assert( type && isData( type->getKind() ) );

	// we can point to a co::AnyValue, but not to another co::Any
	assert( type->getKind() != TK_ANY || isIn == false );

	state.type = type;
	state.isIn = isIn;
	state.size = size;
	state.data.cptr = addr;

	if( !isIn )
		return;

	TypeKind k = type->getKind();
	switch( k )
	{
	case TK_BOOL:		copy<bool>( addr, &state.data ); break;
	case TK_INT8:		copy<int8>( addr, &state.data ); break;
	case TK_INT16:		copy<int16>( addr, &state.data ); break;
	case TK_INT32:		copy<int32>( addr, &state.data ); break;
	case TK_UINT8:		copy<uint8>( addr, &state.data ); break;
	case TK_UINT16:		copy<uint16>( addr, &state.data ); break;
	case TK_UINT32:		copy<uint32>( addr, &state.data ); break;
	case TK_FLOAT:		copy<float>( addr, &state.data ); break;
	case TK_DOUBLE:		copy<double>( addr, &state.data ); break;
	case TK_ENUM:		copy<uint32>( addr, &state.data ); break;
	case TK_INTERFACE:	copy<IService*>( addr, &state.data ); break;
	default:
		assert( !isScalar( k ) );
	}
}

void Any::put( Any in ) const
{
	TypeKind myK = getKind();
	if( myK == TK_NULL || !isOut() )
		CORAL_THROW( IllegalStateException,
			"cannot write to a '" << state << "' variable" );

	in = in.asIn();
	TypeKind inK = in.getKind();
	assert( inK != TK_ANY );

	switch( myK )
	{
	case TK_BOOL:
		if( isScalar( inK ) )
			castTo<bool>( inK, &in.state.data, state.data.ptr );
		else if( inK == TK_STRING )
			*reinterpret_cast<bool*>( state.data.ptr ) = isTrue( *in.state.data.str );
		else
			THROW_NO_CONVERSION( in.state, state, );
		break;

	case TK_INT8:
	case TK_INT16:
	case TK_INT32:
		if( isScalar( inK ) )
			castScalar( inK, &in.state.data, myK, state.data.ptr );
		else if( inK == TK_STRING )
		{
			const std::string& str = *in.state.data.str;
			long v = strtol( str.c_str(), NULL, 0 );
			castFrom<long>( &v, myK, state.data.ptr );
		}
		else
			THROW_NO_CONVERSION( in.state, state, );
		break;

	case TK_UINT8:
	case TK_UINT16:
	case TK_UINT32:
		if( isScalar( inK ) )
			castScalar( inK, &in.state.data, myK, state.data.ptr );
		else if( inK == TK_STRING )
		{
			const std::string& str = *in.state.data.str;
			unsigned long v = strtoul( str.c_str(), NULL, 0 );
			castFrom<unsigned long>( &v, myK, state.data.ptr );
		}
		else
			THROW_NO_CONVERSION( in.state, state, );
		break;
		
	case TK_FLOAT:
	case TK_DOUBLE:
		if( isScalar( inK ) )
			castScalar( inK, &in.state.data, myK, state.data.ptr );
		else if( inK == TK_STRING )
		{
			double d = strtod( in.state.data.str->c_str(), NULL );
			if( myK == TK_DOUBLE )
				state.data.deref->d = d;
			else
				state.data.deref->f = static_cast<float>( d );
		}
		else
			THROW_NO_CONVERSION( in.state, state, );
		break;

	case TK_ENUM:
		{
			IEnum* enumType = static_cast<IEnum*>( state.type );
			if( isScalar( inK ) )
			{
				uint32 id;
				castTo<uint32>( inK, &in.state.data, &id );
				if( id >= enumType->getIdentifiers().getSize() )
					THROW_NO_CONVERSION( in.state, state, << ": " << id
									<< " is out of range for the enum" );
				*reinterpret_cast<uint32*>( state.data.ptr ) = id;
			}
			else if( inK == TK_STRING )
			{
				const std::string& str = *in.state.data.str;
				int32 id = enumType->getValueOf( str );
				if( id == -1 )
					THROW_NO_CONVERSION( in.state, state,
						<< ": no such identifier '" << str << "' in the enum" );
				*reinterpret_cast<uint32*>( state.data.ptr ) = id;
			}
			else
				THROW_NO_CONVERSION( in.state, state, );
		}
		break;

	case TK_STRING:
		if( inK == TK_STRING )
			*state.data.str = *in.state.data.str;
		else
		{
			std::stringstream ss;
			streamOut( ss, in );
			ss.str().swap( *state.data.str );
		}
		break;

	case TK_ANY:
		*reinterpret_cast<AnyValue*>( state.data.ptr ) = in;
		break;

	case TK_ARRAY:
		resize( in.state.size );
		if( in.isNull() )
			break;

		if( in.getKind() != TK_ARRAY )
			THROW_NO_CONVERSION( in.state, state, );

		// are both arrays of the same exact type?
		if( getType() == in.getType() )
		{
			// fast path - straight copy using the element reflector
			IType* elemType = static_cast<IArray*>( getType() )->getElementType();
			elemType->getReflector()->copyValues( in.state.data.ptr,
							getVectorData( state.data.ptr ), in.state.size );
		}
		else
		{
			// slow path - each element must be converted through put()
			for( size_t i = 0; i < in.state.size; ++i )
				at( i ).put( in[i] );
		}
		break;

	case TK_STRUCT:
	case TK_NATIVECLASS:
		if( in.getType() != getType() )
			THROW_NO_CONVERSION( in.state, state, );
		getType()->getReflector()->copyValues( in.state.data.ptr, state.data.ptr, 1 );
		break;

	case TK_INTERFACE:
		{
			IService* service = NULL;
			if( in.isValid() )
			{
				if( in.getType()->isA( state.type ) )
					service = in.get<IService*>();
				else
					THROW_NO_CONVERSION( in.state, state, );
			}
			*reinterpret_cast<RefPtr<IService>*>( state.data.ptr ) = service;
		}
		break;

	default:
		assert( false );
	}
}

void Any::swapArrays( const Any& other ) const
{
	assert( getKind() == TK_ARRAY );
	assert( getType() == other.getType() );
	assert( isOut() && other.isOut() );
	StdVector* myVec = reinterpret_cast<StdVector*>( state.data.ptr );
	StdVector* otherVec = reinterpret_cast<StdVector*>( other.state.data.ptr );
	myVec->swap( *otherVec );
}

void Any::resize( size_t n ) const
{
	assert( isOut() && getKind() == TK_ARRAY );

	StdVector& v = *reinterpret_cast<StdVector*>( state.data.ptr );
	IType* elemType = static_cast<IArray*>( state.type )->getElementType();
	IReflector* elemReflector = elemType->getReflector();
	size_t elemSize = elemReflector->getSize();
	size_t currentSize = v.size();
	size_t currentN = currentSize / elemSize;
	if( currentN == n )
		return; // nothing to be done

	size_t newSize = n * elemSize;

	if( n < currentN )
	{
		// reduce the vector size
		elemReflector->destroyValues( &v[newSize], currentN - n );
		v.resize( newSize );
		return;
	}

	// expand the vector size
	if( v.capacity() < newSize )
	{
		// requires reallocation
		StdVector tmp( newSize );
		uint8* newAddr = &tmp[0];
		elemReflector->createValues( newAddr, n );
		if( currentN > 0 )
		{
			uint8* addr = &v[0];
			elemReflector->copyValues( addr, newAddr, currentN );
			elemReflector->destroyValues( addr, currentN );
		}
		tmp.swap( v );
	}
	else
	{
		// does not require reallocation
		uint8* addr = &v[0];
		v.resize( newSize );
		assert( addr == &v.front() );
		elemReflector->createValues( addr + currentSize, n - currentN );
	}
}

Any Any::at( size_t index ) const
{
	assert( getKind() == TK_ARRAY );
	assert( index < getCount() );

	IType* elemType = static_cast<IArray*>( state.type )->getElementType();
	co::uint8* addr = state.data.bytes;
	if( !state.isIn )
		addr = getVectorData( addr );
	else if( elemType->getKind() == TK_ANY )
		return reinterpret_cast<Any*>( addr )[index];

	addr += index * elemType->getReflector()->getSize();
	return Any( state.isIn, elemType, addr );
}

bool Any::operator==( const Any& other ) const
{
	if( getType() != other.getType() )
		return false;

	if( isNull() )
		return true;

	if( state.isIn != other.state.isIn )
		return false;

	TypeKind k = getKind();
	if( state.isIn )
	{
		if( k == TK_ARRAY && state.size != other.state.size )
			return false;

		switch( k )
		{
		case TK_BOOL:	return isEqual<bool>( &state.data, &other.state.data );
		case TK_INT8:	return isEqual<int8>( &state.data, &other.state.data );
		case TK_INT16:	return isEqual<int16>( &state.data, &other.state.data );
		case TK_INT32:	return isEqual<int32>( &state.data, &other.state.data );
		case TK_UINT8:	return isEqual<uint8>( &state.data, &other.state.data );
		case TK_UINT16:	return isEqual<uint16>( &state.data, &other.state.data );
		case TK_UINT32:	return isEqual<uint32>( &state.data, &other.state.data );
		case TK_FLOAT:	return isEqual<float>( &state.data, &other.state.data );
		case TK_DOUBLE:	return isEqual<double>( &state.data, &other.state.data );
		case TK_ENUM:	return isEqual<uint32>( &state.data, &other.state.data );
		default:
			assert( !isScalar( k ) );
		}
	}

	return state.data.ptr == other.state.data.ptr;
}

void Any::cast( Any& to ) const
{
	TypeKind myK = getKind();
	if( myK != TK_NULL )
	{
		TypeKind toK = to.getKind();

		// if this contains a co::AnyValue we may have to dereference it
		if( myK == TK_ANY && toK != TK_ANY )
		{
			assert( isOut() );
			reinterpret_cast<AnyValue*>( state.data.ptr )->getAny().cast( to );
			return;
		}

		// are we casting to an 'in' or 'out' variable?
		if( to.isOut() )
		{
			// casting to an 'out' variable: both types must match
			if( isOut() && state.type == to.state.type )
			{
				to.state.data.ptr = state.data.ptr;
				return;
			}
		}
		else if( isOut() ) // casting from an 'out' to an 'in' variable
		{
			// we must extract an 'in' var from the 'out' var to carry on the cast
			return asIn().cast( to );
		}
		else // casting from an 'in' to an 'in' variable
		{
			if( toK == TK_INTERFACE )
			{
				if( myK == TK_INTERFACE &&
				   ( state.data.ptr == NULL || state.type->isA( to.state.type ) ) )
				{
					to.state.data.ptr = state.data.ptr;
					return;
				}
			}
			else if( myK == TK_ARRAY )
			{
				if( toK == TK_ARRAY )
				{
					// check if both array element types match
					IType* myET = static_cast<IArray*>( state.type )->getElementType();
					IType* toET = static_cast<IArray*>( to.state.type )->getElementType();
					if( myET == toET || ( myET->getKind() == TK_INTERFACE && myET->isA( toET ) ) )
					{
						to.state.size = state.size;
						to.state.data = state.data;
						return;
					}
				}
			}
			else if( state.type == to.state.type )
			{
				to.state.data = state.data;
				return;
			}
		}
	}

	CORAL_THROW( IllegalCastException, "illegal cast from '" << state
					<< "' to '" << to.state << "'" );
}

/****************************************************************************/
/* co::AnyValue                                                             */
/****************************************************************************/

void AnyValue::clear()
{
	if( isNull() )
		return;

	// sanity check
	assert( _any.isOut() );

	IReflector* reflector = getType()->getReflector();
	reflector->destroyValues( _any.state.data.ptr, 1 );
	free( _any.state.data.ptr );

	_any.clear();
}

void* AnyValue::create( IType* type )
{
	assert( type && isData( type->getKind() ) );

	clear();

	IReflector* reflector = type->getReflector();
	uint32 size = reflector->getSize();
	void* ptr = malloc( size );

	try
	{
		reflector->createValues( ptr, 1 );
	}
	catch( ... )
	{
		free( ptr );
		throw;
	}

	_any.state.type = type;
	_any.state.data.ptr = ptr;

	return ptr;
}

void AnyValue::convert( IType* type )
{
	assert( type );

	if( getType() == type )
		return;

	co::AnyValue temp;
	temp.create( type );
	if( isValid() )
		temp.getAny().put( getAny() );
	swap( temp );
}

void AnyValue::copy( Any var )
{
	var = var.asIn();
	if( var.isNull() )
	{
		clear();
	}
	else
	{
		create( var.getType() );
		_any.put( var );
	}
}

} // namespace co

/****************************************************************************/
/* Global iostream Insertion Operators                                      */
/****************************************************************************/

std::ostream& operator<<( std::ostream& os, const co::__any::State& s )
{
	assert( s.type );
	if( s.type->getKind() != co::TK_NULL )
		os << ( s.isIn ? "in " : "out " );
	return os << s.type->getFullName();
}

std::ostream& operator<<( std::ostream& os, const co::Any& any )
{
	streamOut( os, any );
	return os;
}
