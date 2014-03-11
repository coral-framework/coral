#ifndef _CO_TYPETRAITS_H_
#define _CO_TYPETRAITS_H_

#include <co/Platform.h>
#include <co/TypeKind.h>
#include <type_traits>
#include <string>
#include <vector>

namespace co {

// Forward Decls:
class IEnum;
class IType;
class IArray;
class IObject;
class IStruct;
class IService;
class IException;
class IInterface;
class IComponent;
class INativeClass;
template<typename T> class Slice;
template<typename T> class RefPtr;

/*!
	Maps TypeKinds to either their names in CSL.
	\warning make sure ( index >= 0 && index < co::TK_COUNT ).
 */
extern CORAL_EXPORT const std::string TK_STRINGS[];

/*!
	Maps the basic type kinds to their IType (or NULL if not a basic type).
	\warning make sure ( index >= 0 && index < co::TK_COUNT ).
 */
extern CORAL_EXPORT RefPtr<IType> BASIC_TYPES[];

/****************************************************************************/
/* Internal Helper Functions (to avoid type dependencies in template code)  */
/****************************************************************************/

#ifndef DOXYGEN

// Just like co::getType() but takes a C-string instead of a std::string.
CORAL_EXPORT IType* getTypeByLiteralName( const char* fullName );

// Returns the kind of a 'type'.
CORAL_EXPORT TypeKind getKind( IType* type );

// Attempts to retrieve a service of a certain 'type' from 'object'.
CORAL_EXPORT IService* findServiceByType( IObject* object, IInterface* type );

// Retrieves a service of a certain 'type' from 'object'. If impossible raises an exception.
CORAL_EXPORT IService* getServiceByType( IObject* object, IInterface* type );

// Attempts to retrieve a service from 'object' by its port name.
CORAL_EXPORT IService* findServiceByName( IObject* object, const std::string& portName );

// Retrieves a service from 'object' by its port name. If impossible raises an exception.
CORAL_EXPORT IService* getServiceByName( IObject* object, const std::string& portName );

// Binds a 'service' to the receptacle identified by 'receptacleName' in 'object'.
CORAL_EXPORT void setServiceByName( IObject* object, const std::string& receptacleName, IService* service );

#endif


/****************************************************************************/
/* Kind Trait Operators (to distinguish all kinds of types)                 */
/****************************************************************************/

//! Returns true for integer types.
inline bool isInteger( TypeKind k ) { return k >= TK_BOOL && k <= TK_UINT32; }

//! Returns true for signed integer types.
inline bool isSignedInteger( TypeKind k ) { return k >= TK_BOOL && k <= TK_INT32; }

//! Returns true for floating-point types.
inline bool isFloat( TypeKind k ) { return k == TK_FLOAT || k == TK_DOUBLE; }

//! Returns true for integer and floating-point types.
inline bool isNumeric( TypeKind k ) { return k >= TK_BOOL && k <= TK_DOUBLE; }

//! Returns true for numeric and enumeration types.
inline bool isScalar( TypeKind k ) { return k >= TK_BOOL && k <= TK_ENUM; }

//! Returns true for value types (with deep copy semantics).
inline bool isValue( TypeKind k ) { return k > TK_NULL && k <= TK_NATIVECLASS; }

//! Returns true for reference types (with shallow copy semantics).
inline bool isReference( TypeKind k ) { return k == TK_INTERFACE; }

//! Returns whether it is possible to declare variables and fields of this type.
inline bool isData( TypeKind k ) { return k > TK_NULL && k <= TK_INTERFACE; }

//! Returns true for types whose variable is a scalar.
inline bool isScalarOrRef( TypeKind k ) { return isScalar( k ) || isReference( k ); }

//! Returns true if new types of this kind can be defined by users.
inline bool isCustom( TypeKind k ) { return k >= TK_STRUCT || k == TK_ENUM; }

//! Returns true for value types that require custom reflectors.
inline bool isComplexValue( TypeKind k ) { return k == TK_STRUCT || k == TK_NATIVECLASS; }

//! Returns true for types that may contain members (ICompositeType).
inline bool isComposite( TypeKind k ) { return k >= TK_STRUCT && k <= TK_COMPONENT; }

//! Returns true for types that support inheritance.
inline bool isInheritable( TypeKind k ) { return k == TK_INTERFACE; }

//! Returns true if a built-in IReflector is available for the type.
inline bool hasBuiltInReflector( TypeKind k ) { return k < TK_STRUCT; }


/****************************************************************************/
/* Type Traits: kindOf<T>::kind                                             */
/****************************************************************************/

template<typename>
struct kindOf
{
	static const TypeKind kind = TK_NULL;
};

//! Common implementation of co::kindOf<T>:
template<TypeKind k> struct kindOfBase { static const TypeKind kind = k; };

// specializations for basic types:
template<> struct kindOf<bool> : public kindOfBase<TK_BOOL> {};
template<> struct kindOf<int8> : public kindOfBase<TK_INT8> {};
template<> struct kindOf<int16> : public kindOfBase<TK_INT16> {};
template<> struct kindOf<int32> : public kindOfBase<TK_INT32> {};
template<> struct kindOf<uint8> : public kindOfBase<TK_UINT8> {};
template<> struct kindOf<uint16> : public kindOfBase<TK_UINT16> {};
template<> struct kindOf<uint32> : public kindOfBase<TK_UINT32> {};
template<> struct kindOf<float> : public kindOfBase<TK_FLOAT> {};
template<> struct kindOf<double> : public kindOfBase<TK_DOUBLE> {};
template<> struct kindOf<std::string> : public kindOfBase<TK_STRING> {};

// specialization for container types:
template<typename T> struct kindOf<Slice<T> > : public kindOfBase<TK_ARRAY> {};
template<typename T> struct kindOf<RefPtr<T> > : public kindOf<T> {};
template<typename T> struct kindOf<std::vector<T> > : public kindOfBase<TK_ARRAY> {};

// for unknown pointers, use the dereferenced type
template<typename T> struct kindOf<T*> : public kindOf<T> {};
template<typename T> struct kindOf<T* const> : public kindOf<T*> {};

// specialization for co::TypeKind (must go here to avoid cyclic dependencies)
template<> struct kindOf<TypeKind> : public kindOfBase<TK_ENUM> {};


/****************************************************************************/
/* co::typeDescriptorFor<TypeKind>::Type maps a TK to its descriptor type   */
/****************************************************************************/

template<TypeKind> struct typeDescriptorFor { typedef IType Type; };
template<> struct typeDescriptorFor<TK_ARRAY> { typedef IArray Type; };
template<> struct typeDescriptorFor<TK_ENUM> { typedef IEnum Type; };
template<> struct typeDescriptorFor<TK_EXCEPTION> { typedef IException Type; };
template<> struct typeDescriptorFor<TK_STRUCT> { typedef IStruct Type; };
template<> struct typeDescriptorFor<TK_NATIVECLASS> { typedef INativeClass Type; };
template<> struct typeDescriptorFor<TK_INTERFACE> { typedef IInterface Type; };
template<> struct typeDescriptorFor<TK_COMPONENT> { typedef IComponent Type; };


/****************************************************************************/
/* co::nameOf<T>::get() returns the full name of a Coral type.              */
/****************************************************************************/

template<typename T>
struct nameOf
{
	static const char* get()
	{
		return TK_STRINGS[kindOf<T>::kind].c_str();
	}
};

//! Common implementation of co::nameOf<T> for arrays:
template<typename ET>
struct nameOfArrayBase
{
	static const char* get()
	{
		static const std::string s_name( std::string( nameOf<ET>::get() ) + "[]" );
		return s_name.c_str();
	}
};

// specialization for array types:
template<typename T> struct nameOf<Slice<T> > : public nameOfArrayBase<T> {};
template<typename T> struct nameOf<std::vector<T> > : public nameOfArrayBase<T> {};

// if necessary dereference pointer types
template<typename T> struct nameOf<T*> : public nameOf<T> {};
template<typename T> struct nameOf<T* const> : public nameOf<T*> {};
template<typename T> struct nameOf<RefPtr<T> > : public nameOf<T> {};

// specialization for co::TypeKind (must go here to avoid cyclic dependencies)
template<> struct nameOf<TypeKind> { static const char* get() { return "co.TypeKind"; } };


/****************************************************************************/
/* co::typeOf<T>::get() returns the co::IType instance of a Coral type      */
/****************************************************************************/

template<typename T>
struct typeOf
{
	static const TypeKind kind = kindOf<T>::kind;
	typedef typename typeDescriptorFor<kind>::Type Descriptor;

	static Descriptor* get()
	{
		IType* type = getTypeByLiteralName( nameOf<T>::get() );
		assert( getKind( type ) == kind );
		return static_cast<Descriptor*>( type );
	}
};

template<typename T>
struct typeOfBasic
{
	static const TypeKind kind = kindOf<T>::kind;
	typedef IType Descriptor;
	static Descriptor* get() { return reinterpret_cast<Descriptor**>( BASIC_TYPES )[kind]; }
};

// specializations for basic types:
template<> struct typeOf<bool> : public typeOfBasic<bool> {};
template<> struct typeOf<int8> : public typeOfBasic<int8> {};
template<> struct typeOf<uint8> : public typeOfBasic<uint8> {};
template<> struct typeOf<int16> : public typeOfBasic<int16> {};
template<> struct typeOf<uint16> : public typeOfBasic<uint16> {};
template<> struct typeOf<int32> : public typeOfBasic<int32> {};
template<> struct typeOf<uint32> : public typeOfBasic<uint32> {};
template<> struct typeOf<float> : public typeOfBasic<float> {};
template<> struct typeOf<double> : public typeOfBasic<double> {};
template<> struct typeOf<std::string> : public typeOfBasic<std::string> {};


/****************************************************************************/
/* Basic RTTI functions                                                     */
/****************************************************************************/

/*!
	Returns whether a \a service is of a given \a type.
	This method always returns \c true if \a service is NULL.
 */
CORAL_EXPORT bool isA( IService* service, IInterface* type );

template<typename SubType>
inline bool isA( IService* service )
{
	return isA( service, typeOf<SubType>::get() );
}

/*!
	Raises an IllegalCastException if \a service if not of a given \a type.
	No exception is raised if \a service is NULL.
 */
CORAL_EXPORT void ensureIsA( IService* service, IInterface* type );

/*!
	Downcasts a service, raising an IllegalCastException on failure.
	The cast always succeeds for NULL pointers (result is a NULL pointer).
	\throw IllegalCastException if the service is not of the requested type.
 */
template<typename SubType>
inline SubType* cast( IService* service )
{
	ensureIsA( service, typeOf<SubType>::get() );
	return static_cast<SubType*>( service );
}

} // namespace co

/*!
	Outputs the string associated with a co::TypeKind. \relates co::TypeKind
 */
inline std::ostream& operator<<( std::ostream& out, co::TypeKind kind )
{
	return out << co::TK_STRINGS[kind];
}

#endif // _CO_TYPETRAITS_H_
