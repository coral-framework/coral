#include <gtest/gtest.h>

#include <co/Any.h>
#include <co/Coral.h>
#include <co/IType.h>
#include <co/ISystem.h>
#include <co/IModule.h>
#include <co/IObject.h>
#include <co/IMember.h>
#include <co/IStruct.h>
#include <co/CSLError.h>
#include <co/INamespace.h>
#include <co/IInterface.h>
#include <co/IReflector.h>
#include <co/IRecordType.h>
#include <co/INativeClass.h>
#include <co/ITypeManager.h>
#include <co/IllegalCastException.h>
#include <co/IllegalStateException.h>
#include <co/reserved/Uuid.h>

/*****************************************************************************/
/* Performance and Portability Tests                                         */
/*****************************************************************************/

TEST( AnyTests, sizeOf )
{
	// we assume size_t is the size of a pointer
	EXPECT_EQ( CORAL_POINTER_SIZE, sizeof(void*) );
	EXPECT_EQ( sizeof(void*), sizeof(size_t) );

	// co::__any::Data should be the size of a double
	EXPECT_EQ( 8, sizeof(double) );
	EXPECT_EQ( sizeof(double), sizeof(co::__any::Data) );

	// make sure sizeof(co::__any::State) is as expected (2 ptrs + double)
#if CORAL_POINTER_SIZE == 4
	EXPECT_EQ( 16, sizeof(co::__any::State) );
#elif CORAL_POINTER_SIZE == 8
	EXPECT_EQ( 24, sizeof(co::__any::State) );
#else
#error Pointers are neither 32 nor 64 bit long?
#endif

	EXPECT_EQ( sizeof(co::Any), sizeof(co::__any::State) );
}

TEST( AnyTests, stdVectorMemoryLayout )
{
	/*
		In co::Any we assume some truths about the memory layout of std::vectors.
		This test verifies whether the assumptions are true for the local STL distro.

		Basically, the assumption is that the std::vector computes its size() by subtracting
		two pointers (one to the start, another past the end of the array). This is a fairly
		standard implementation choice for std::vectors, and allows Coral to find out how much
		memory is allocated by a std::vector in run-time without knowing its template type.

		A derived assumption is that all std::vectors have the same size.
	 */

	size_t s1 = sizeof(std::vector<co::uint8>);
	size_t s2 = sizeof(std::vector<co::IServiceRef>);
	ASSERT_EQ( s1, s2 );

	std::vector<double> dblVector( 8 );
	ASSERT_EQ( 8, dblVector.size() );

	std::vector<co::uint8>& bytesVector = *reinterpret_cast<std::vector<co::uint8>*>( &dblVector );
	ASSERT_EQ( 64, bytesVector.size() );
}

/*****************************************************************************/
/* A 'constructor' test checks whether the co::Any is detecting the correct  */
/* type for a variable passed to its constructor. It may also include basic  */
/* retrieval tests for the values passed to the constructor.                 */
/*****************************************************************************/

void EXPECT_ANY_TYPE_STREQ( const co::Any& any, const char* str )
{
	std::stringstream ss;
	ss << any.state;
	EXPECT_STREQ( str, ss.str().c_str() );
}

void EXPECT_ANY_STREQ( const co::Any& any, const char* str )
{
	std::stringstream ss;
	ss << any.state << ": " << any;
	EXPECT_STREQ( str, ss.str().c_str() );
}

template<typename T>
void constructorValueTest( co::TypeKind kind, const T& sampleValue, const char* expectedTypeName )
{
	T v = sampleValue;
	co::Any a1( v );
	ASSERT_TRUE( a1.isValid() );
	EXPECT_FALSE( a1.isIn() );
	EXPECT_TRUE( a1.isOut() );
	EXPECT_EQ( kind, a1.getType()->getKind() );
	EXPECT_EQ( sizeof(T), a1.getElementSize() );
	EXPECT_EQ( a1.getType()->getFullName(), expectedTypeName );
	EXPECT_EQ( v, a1.get<T>() );

	const T cv = sampleValue;
	co::Any a2( cv );
	ASSERT_TRUE( a2.isValid() );
	EXPECT_TRUE( a2.isIn() );
	EXPECT_FALSE( a2.isOut() );
	EXPECT_EQ( kind, a2.getType()->getKind() );
	EXPECT_EQ( sizeof(T), a2.getElementSize() );
	EXPECT_EQ( a2.getType()->getFullName(), expectedTypeName );
	EXPECT_EQ( cv, a2.get<T>() );
}

template<typename T>
void constructorPtrTest( const co::TypeKind kind, T* sampleValue, const char* expectedTypeName )
{
	T* p = sampleValue;
	co::Any a3( p );
	ASSERT_TRUE( a3.isValid() );
	EXPECT_TRUE( a3.isIn() );
	EXPECT_FALSE( a3.isOut() );
	EXPECT_EQ( kind, a3.getType()->getKind() );
	EXPECT_EQ( sizeof(void*), a3.getElementSize() );
	EXPECT_EQ( a3.getType()->getFullName(), expectedTypeName );
	EXPECT_EQ( p, a3.get<T*>() );

	T* const pc = sampleValue;
	co::Any a5;
	a5.set<T* const>( pc );
	ASSERT_TRUE( a5.isValid() );
	EXPECT_TRUE( a5.isIn() );
	EXPECT_FALSE( a5.isOut() );
	EXPECT_EQ( kind, a5.getType()->getKind() );
	EXPECT_EQ( sizeof(void*), a5.getElementSize() );
	EXPECT_EQ( a5.getType()->getFullName(), expectedTypeName );
	EXPECT_EQ( pc, a5.get<T* const>() );

	co::RefPtr<T> refPtr( p );
	co::Any a9( refPtr );
	ASSERT_TRUE( a9.isValid() );
	EXPECT_FALSE( a9.isIn() );
	EXPECT_TRUE( a9.isOut() );
	EXPECT_EQ( kind, a9.getType()->getKind() );
	EXPECT_EQ( sizeof(void*), a9.getElementSize() );
	EXPECT_EQ( a9.getType()->getFullName(), expectedTypeName );
	EXPECT_EQ( &refPtr, &a9.get<co::RefPtr<T>&>() );
}

template<typename T>
void constructorNonConstRefTest( const co::TypeKind kind, const T& sampleValue, const char* expectedTypeName )
{
	T v = sampleValue;
	
	co::Any a7;
	a7.set<T&>( v );
	ASSERT_TRUE( a7.isValid() );
	EXPECT_FALSE( a7.isIn() );
	EXPECT_TRUE( a7.isOut() );
	EXPECT_EQ( kind, a7.getType()->getKind() );
	EXPECT_EQ( sizeof(T), a7.getElementSize() );
	EXPECT_EQ( a7.getType()->getFullName(), expectedTypeName );
	EXPECT_EQ( v, a7.get<T&>() );
}

template<typename T>
void constructorRefTest( const co::TypeKind kind, const T& sampleValue, const char* expectedTypeName )
{
	constructorNonConstRefTest<T>( kind, sampleValue, expectedTypeName );

	T v = sampleValue;

	co::Any a8;
	a8.set<const T&>( v );
	ASSERT_TRUE( a8.isValid() );
	EXPECT_TRUE( a8.isIn() );
	EXPECT_FALSE( a8.isOut() );
	EXPECT_EQ( kind, a8.getType()->getKind() );
	EXPECT_EQ( sizeof(T), a8.getElementSize() );
	EXPECT_EQ( a8.getType()->getFullName(), expectedTypeName );
	EXPECT_EQ( v, a8.get<const T&>() );

	// shouldn't be able to get a non-const ref
	EXPECT_THROW( a8.get<T&>(), co::IllegalCastException );
}

TEST( AnyTests, constructDefault )
{
	co::Any defaultAny;
	EXPECT_TRUE( defaultAny.isNull() );
	EXPECT_FALSE( defaultAny.isValid() );
	EXPECT_EQ( defaultAny.getType(), co::BASIC_TYPES[co::TK_NULL].get() );
	EXPECT_ANY_STREQ( defaultAny, "null: null" );

	co::Any anotherDefaultAny;
	EXPECT_TRUE( defaultAny == anotherDefaultAny );

	co::Any notDefaultAny( 0 );
	EXPECT_TRUE( defaultAny != notDefaultAny );
}

TEST( AnyTests, constructBool )
{
	constructorValueTest<bool>( co::TK_BOOL, false, "bool" );
}

TEST( AnyTests, constructInt8 )
{
	constructorValueTest<co::int8>( co::TK_INT8, 0, "int8" );
}

TEST( AnyTests, constructInt16 )
{
	constructorValueTest<co::int16>( co::TK_INT16, 0, "int16" );
}

TEST( AnyTests, constructInt32 )
{
	constructorValueTest<co::int32>( co::TK_INT32, 0, "int32" );
}

TEST( AnyTests, constructUInt8 )
{
	constructorValueTest<co::uint8>( co::TK_UINT8, 0, "uint8" );
}

TEST( AnyTests, constructUInt16 )
{
	constructorValueTest<co::uint16>( co::TK_UINT16, 0, "uint16" );
}

TEST( AnyTests, constructUInt32 )
{
	constructorValueTest<co::uint32>( co::TK_UINT32, 0, "uint32" );
}

TEST( AnyTests, constructFloat )
{
	constructorValueTest<float>( co::TK_FLOAT, 0.0f, "float" );
}

TEST( AnyTests, constructDouble )
{
	constructorValueTest<double>( co::TK_DOUBLE, 0.0, "double" );
}

TEST( AnyTests, constructEnum )
{
	constructorValueTest<co::TypeKind>( co::TK_ENUM, co::TK_ANY, "co.TypeKind" );
}

TEST( AnyTests, constructString )
{
	std::string str( "foo" );
	constructorValueTest<std::string>( co::TK_STRING, str, "string" );
	constructorRefTest<std::string>( co::TK_STRING, str, "string" );
	EXPECT_ANY_STREQ( co::Any( str ), "out string: \"foo\"" );
}

TEST( AnyTests, constructAnyValue )
{
	co::AnyValue dblAny( 3.14 );
	constructorNonConstRefTest<co::AnyValue>( co::TK_ANY, dblAny, "any" );
	EXPECT_ANY_STREQ( dblAny, "out any: 3.14" );

	co::AnyValue nullAny;
	EXPECT_ANY_STREQ( nullAny, "out any: null" );
}

TEST( AnyTests, constructArray )
{
	co::Slice<co::int16> r1;
	co::Any a1( r1 );
	EXPECT_EQ( 0, a1.getCount() );
	EXPECT_ANY_STREQ( a1, "in int16[]: {}" );

	co::Slice<std::string> r2;
	co::Any a2( r2 );
	EXPECT_EQ( 0, a2.getCount() );
	EXPECT_ANY_STREQ( a2, "in string[]: {}" );

	const int NUM_PATH_ENTRIES = 2;
	co::Slice<std::string> r3 = co::getPaths();

	co::Any a3( r3 );
	EXPECT_EQ( NUM_PATH_ENTRIES, a3.getCount() );
	EXPECT_ANY_TYPE_STREQ( a3, "in string[]" );

	ASSERT_EQ( NUM_PATH_ENTRIES, r3.getSize() );
	co::Slice<std::string> r3ReadBack = a3.get<co::Slice<std::string> >();
	ASSERT_EQ( r3.getSize(), r3ReadBack.getSize() );
	while( r3 )
	{
		EXPECT_EQ( r3.getFirst(), r3ReadBack.getFirst() );
		r3.popFirst();
		r3ReadBack.popFirst();
	}

	std::vector<co::IInterfaceRef> r4;
	co::Any a4;
	a4.set<std::vector<co::IInterfaceRef>&>( r4 );
	EXPECT_EQ( 0, a4.getCount() );
	EXPECT_ANY_STREQ( a4, "out co.IInterface[]: {}" );

	EXPECT_TRUE( co::TK_NATIVECLASS == co::kindOf<co::Uuid>::kind );
	EXPECT_EQ( "co.Uuid", co::typeOf<co::Uuid>::get()->getFullName() );
	EXPECT_STREQ( "co.Uuid", co::nameOf<co::Uuid>::get() );

	std::vector<co::Uuid> r5;
	co::Any a5( r5 );
	EXPECT_EQ( 0, a5.getCount() );
	EXPECT_ANY_STREQ( a5, "out co.Uuid[]: {}" );

	std::vector<double> r6( 5, 4321 );
	co::Any a6( r6 );
	EXPECT_EQ( 5, a6.getCount() );
	EXPECT_ANY_STREQ( a6, "out double[]: {4321, 4321, 4321, 4321, 4321}" );

	double d = 7.5;
	EXPECT_EQ( 5, a6.get<std::vector<double>&>().size() );
	a6.get<std::vector<double>&>().push_back( d );
	ASSERT_EQ( 6, r6.size() );
	EXPECT_EQ( 6, a6.getCount() );
	EXPECT_EQ( d, r6.back() );
}

TEST( AnyTests, constructStruct )
{
	co::CSLError cslError;
	cslError.filename = "filename";
	cslError.message = "msg";
	cslError.line = 7;
	constructorRefTest<co::CSLError>( co::TK_STRUCT, cslError, "co.CSLError" );
	EXPECT_ANY_STREQ( cslError, "out co.CSLError: {filename = \"filename\", line = 7, message = \"msg\"}" );
}

TEST( AnyTests, constructNativeClass )
{
	constructorRefTest<co::Uuid>( co::TK_NATIVECLASS, co::Uuid::createRandom(), "co.Uuid" );
}

TEST( AnyTests, constructInterface )
{
	constructorPtrTest<co::INamespace>( co::TK_INTERFACE,
		co::getSystem()->getTypes()->getRootNS(), "co.INamespace" );
}

/*****************************************************************************/
/* A 'set and get' test checks whether a value can be correctly stored and   */
/* retrieved into/from a co::Any. It may also include basic coercion tests.  */
/*****************************************************************************/

template<typename T>
void setAndGetTest( T correctValue, T wrongValue )
{
	T input( correctValue );
	co::Any a;
	a.set<T>( input );
	T output( a.get<T>() );
	EXPECT_EQ( input, output );
	EXPECT_EQ( output, correctValue );
	EXPECT_NE( output, wrongValue );
}

TEST( AnyTests, setGetAny )
{
	co::AnyValue a1( 1 ), a2( 2 );
	setAndGetTest<co::AnyValue&>( a1, a2 );
}

TEST( AnyTests, setGetBool )
{
	setAndGetTest<bool>( true, false );
	setAndGetTest<bool>( false, true );
}

TEST( AnyTests, setGetInt8 )
{
	setAndGetTest<co::int8>( 127, 0 );
	setAndGetTest<co::int8>( -128, 1 );
}

TEST( AnyTests, setGetUInt8 )
{
	setAndGetTest<co::uint8>( 0, 3 );
	setAndGetTest<co::uint8>( 255, 0 );
}

TEST( AnyTests, setGetInt16 )
{
	setAndGetTest<co::int16>( -1234, -1 );
	setAndGetTest<co::int16>( 5678, 7 );
}

TEST( AnyTests, setGetUInt16 )
{
	setAndGetTest<co::uint16>( 0, 1 );
	setAndGetTest<co::uint16>( 60000, 60001 );
}

TEST( AnyTests, setGetInt32 )
{
	setAndGetTest<co::int32>( INT32_MIN, INT32_MIN + 1 );
	setAndGetTest<co::int32>( INT32_MAX, INT32_MAX - 1 );
}

TEST( AnyTests, setGetUInt32 )
{
	setAndGetTest<co::uint32>( 0, -1 );
	setAndGetTest<co::uint32>( UINT32_MAX, UINT32_MAX - 1 );
}

TEST( AnyTests, setGetFloat )
{
	setAndGetTest<float>( -1.0f, -1.001f );
	setAndGetTest<float>( 3.14159265f, 3.14159260f );
}

TEST( AnyTests, setGetDouble )
{
	setAndGetTest<double>( -1.0, -1.00000000001 );
	setAndGetTest<double>( 3.14159265, 3.141592651 );
}

TEST( AnyTests, setGetString )
{
	std::string right( "right" );
	std::string wrong( "wrong" );
	std::string something( "something" );
	std::string somethinG( "somethinG" );

	setAndGetTest<const std::string&>( right, wrong );
	setAndGetTest<std::string&>( something, somethinG );
}

TEST( AnyTests, setGetArrays )
{
	// --- Slice ---
	co::TSlice<co::IType*> range = co::getSystem()->getTypes()->getRootNS()->getTypes();
	co::Any a1( range );
	EXPECT_ANY_TYPE_STREQ( a1, "in co.IType[]" );

	co::Any a1Copy( a1 );
	co::Slice<co::IType*> retrievedRange = a1Copy.get<co::Slice<co::IType*> >();
	EXPECT_EQ( range.getSize(), retrievedRange.getSize() );
	EXPECT_EQ( range.getFirst(), retrievedRange.getFirst() );
	EXPECT_EQ( range.getLast(), retrievedRange.getLast() );

	// --- RefVector ---
	std::vector<co::ITypeRef> refVector;
	refVector.push_back( co::typeOf<co::IObject>::get() );
	refVector.push_back( co::typeOf<co::IModule>::get() );
	refVector.push_back( co::typeOf<co::INamespace>::get() );

	co::Any a2;
	a2.set<std::vector<co::ITypeRef>&>( refVector );
	EXPECT_ANY_TYPE_STREQ( a2, "out co.IType[]" );

	std::vector<co::ITypeRef>& refVectorRef = a2.get<std::vector<co::ITypeRef>&>();
	ASSERT_EQ( &refVector, &refVectorRef );

	EXPECT_EQ( 3, refVectorRef.size() );

	refVectorRef.push_back( co::typeOf<co::ITypeManager>::get() );

	EXPECT_EQ( 4, refVector.size() );
	EXPECT_EQ( "co.ITypeManager", refVector[3]->getFullName() );

	// --- std::vector ---
	std::vector<int> intVec;
	intVec.push_back( -1 );
	intVec.push_back( 5 );
	intVec.push_back( 7 );

	co::Any a3;
	a3.set<std::vector<int>&>( intVec );
	EXPECT_ANY_STREQ( a3, "out int32[]: {-1, 5, 7}" );

	std::vector<int>& intVecRef = a3.get<std::vector<int>&>();

	ASSERT_EQ( &intVec, &intVecRef );
	EXPECT_EQ( 3, intVecRef.size() );

	intVecRef.push_back( 33 );

	EXPECT_EQ( 4, intVec.size() );
	EXPECT_EQ( 33, intVec[3] );
}

TEST( AnyTests, setGetEnum )
{
	setAndGetTest<co::TypeKind>( co::TK_ANY, co::TK_ARRAY );
}

TEST( AnyTests, setGetStruct )
{
	co::CSLError cslError1;
	cslError1.filename = "filename1";
	cslError1.message = "message1";
	cslError1.line = 5;

	co::CSLError cslError2;
	cslError2.filename = "filename2";
	cslError2.message = "message2";
	cslError2.line = 9;

	setAndGetTest<co::CSLError&>( cslError1, cslError2 );
}

TEST( AnyTests, setGetNativeClass )
{
	co::Uuid nullUuid = co::Uuid::null();
	co::Uuid randUuid = co::Uuid::createRandom();
	setAndGetTest<co::Uuid&>( randUuid, nullUuid );
}

bool areSamePtr( void* a, void* b )
{
	return a == b;
}

TEST( AnyTests, setGetInterface )
{
	co::Any any;

	// set 'any' with an instance of co::IInterface
	co::IInterface* input = co::typeOf<co::INamespace>::get();
	any.set( input );
	EXPECT_ANY_TYPE_STREQ( any, "in co.IInterface" );

	// try to get the same co::IInterface back (exact retrieval)
	{
		co::IInterface* output = any.get<co::IInterface*>();
		EXPECT_TRUE( areSamePtr( input, output ) );
		EXPECT_EQ( input->getProvider(), output->getProvider() );
		EXPECT_TRUE( output->isA( co::typeOf<co::IService>::get() ) );
	}

	// try to get a co::IService back (implicit upcasting)
	{
		co::IService* output = any.get<co::IService*>();
		EXPECT_TRUE( areSamePtr( input, output ) );
		EXPECT_EQ( input->getProvider(), output->getProvider() );
	}

	// try to get a co::ICompositeType back (implicit upcasting)
	{
		co::ICompositeType* output = any.get<co::ICompositeType*>();
		EXPECT_TRUE( areSamePtr( input, output ) );
		EXPECT_EQ( input->getProvider(), output->getProvider() );
		co::IMember* res = output->getMember( "name" );
		EXPECT_EQ( res->getName(), "name" );
	}

	// try to retrieve an invalid service
	EXPECT_THROW( any.get<co::INamespace*>(), co::IllegalCastException );

	// try to retrieve a value
	EXPECT_THROW( any.get<co::uint32>(), co::IllegalCastException );

	// set 'any' with the co::ICompositeType facet of the co::IInterface instance
	co::ICompositeType* input2 = input;
	any.set( input2 );
	EXPECT_ANY_TYPE_STREQ( any, "in co.IInterface" );

	// try to get a co::IService back (implicit upcasting)
	{
		co::IService* output = any.get<co::IService*>();
		EXPECT_TRUE( areSamePtr( input2, output ) );
		EXPECT_EQ( input2->getProvider(), output->getProvider() );
	}

	// try to retrieve a co::IInterface (implicit downcasting)
	{
		co::IInterface* output = any.get<co::IInterface*>();
		EXPECT_TRUE( areSamePtr( input2, output ) );
	}
}

/*****************************************************************************/
/* Equality Tests                                                            */
/*****************************************************************************/

TEST( AnyTests, equals )
{
	std::string str( "foo" );
	co::Any aStr( str );

	// comparison between in/out vars works
	EXPECT_TRUE( aStr.isOut() );
	EXPECT_TRUE( aStr.equals( std::string( "foo" ) ) );
	EXPECT_FALSE( aStr.equals( std::string( "bar" ) ) );

	// values must be of the exact same type
	double dbl( 3.14 );
	co::Any aDbl( dbl ), aFlt( 3.14f );
	EXPECT_FALSE( aDbl.equals( aFlt ) );
	EXPECT_TRUE( aFlt.equals( 3.14f ) );

	// array comparisons
	int intArray[] = { 1, 2, 3 };
	co::Any aIntArray( intArray );

	std::vector<int> intVec( 3 );
	intVec[0] = 1; intVec[1] = 2; intVec[2] = 3;
	co::Any aIntVec( intVec );

	EXPECT_TRUE( aIntArray.equals( aIntVec ) );

	intVec.push_back( 4 );
	EXPECT_FALSE( aIntArray.equals( aIntVec ) );
}

/*****************************************************************************/
/* Coercion Tests - verify implicit value conversions at retrieval time      */
/*****************************************************************************/

TEST( AnyTests, coercionsFromBool )
{
	co::Any a1( false );
	co::Any a2( true );

	EXPECT_ANY_STREQ( a1, "in bool: false" );
	EXPECT_ANY_STREQ( a2, "in bool: true" );

	// to string
	EXPECT_EQ( "false", a1.get<std::string>() );
	EXPECT_EQ( "true", a2.get<std::string>() );

	// to int16
	EXPECT_EQ( 0, a1.get<co::int16>() );
	EXPECT_EQ( 1, a2.get<co::int16>() );

	// to double
	EXPECT_EQ( 0.0, a1.get<double>() );
	EXPECT_EQ( 1.0, a2.get<double>() );

	// to enum (co::TypeKind)
	EXPECT_EQ( co::TK_NULL, a1.get<co::TypeKind>() );
	EXPECT_EQ( co::TK_BOOL, a2.get<co::TypeKind>() );

	// no conversion to string or 'reference-based' types
	EXPECT_THROW( a1.get<std::string&>(), co::IllegalCastException );
	EXPECT_THROW( a1.get<co::Uuid&>(), co::IllegalCastException );
}

TEST( AnyTests, coercionsFromUInt32 )
{
	co::Any a0; a0.setIn<co::uint32>( 0 );
	co::Any a1; a1.setIn<co::uint32>( INT16_MAX );
	co::Any a2; a2.setIn<co::uint32>( UINT32_MAX );

	EXPECT_ANY_STREQ( a0, "in uint32: 0" );
	EXPECT_ANY_STREQ( a1, "in uint32: 32767" );
	EXPECT_ANY_STREQ( a2, "in uint32: 4294967295" );

	// to bool
	EXPECT_FALSE( a0.get<bool>() );
	EXPECT_TRUE( a1.get<bool>() );
	EXPECT_TRUE( a2.get<bool>() );

	// to int16
	EXPECT_EQ( 0, a0.get<co::int16>() );
	EXPECT_EQ( INT16_MAX, a1.get<co::int16>() );
	EXPECT_EQ( -1, a2.get<co::int16>() ); // truncation + 2's complement = -1

	// to double
	EXPECT_EQ( 0.0, a0.get<double>() );
	EXPECT_EQ( static_cast<double>( INT16_MAX ), a1.get<double>() );
	EXPECT_EQ( static_cast<double>( UINT32_MAX ), a2.get<double>() );

	// to enum (co::TypeKind)
	EXPECT_EQ( co::TK_NULL, a0.get<co::TypeKind>() );
	EXPECT_EQ( co::TK_EXCEPTION, co::Any( 18 ).get<co::TypeKind>() );
	EXPECT_THROW( a1.get<co::TypeKind>(), co::IllegalCastException );
	EXPECT_THROW( a2.get<co::TypeKind>(), co::IllegalCastException );

	// no conversion to string or 'reference-based' types
	EXPECT_THROW( a0.get<std::string&>(), co::IllegalCastException );
	EXPECT_THROW( a0.get<co::Uuid&>(), co::IllegalCastException );
}

TEST( AnyTests, coercionsFromDouble )
{
	co::Any a0( 0.0 );
	co::Any a1( 3.141592653589 );
	co::Any a2( -9.99 );

	EXPECT_ANY_STREQ( a0, "in double: 0" );
	EXPECT_ANY_STREQ( a1, "in double: 3.141592653589" );
	EXPECT_ANY_STREQ( a2, "in double: -9.99" );

	// to bool
	EXPECT_FALSE( a0.get<bool>() );
	EXPECT_TRUE( a1.get<bool>() );
	EXPECT_TRUE( a2.get<bool>() );

	// to int16
	EXPECT_EQ( 0, a0.get<co::int16>() );
	EXPECT_EQ( 3, a1.get<co::int16>() );
	EXPECT_EQ( -9, a2.get<co::int16>() );

	// to double/float
	EXPECT_EQ( 3.141592653589, a1.get<double>() );
	EXPECT_EQ( 3.14159265f, a1.get<float>() );

	// to enum
	EXPECT_EQ( co::TK_NULL, a0.get<co::TypeKind>() );
	EXPECT_EQ( co::TK_INT16, a1.get<co::TypeKind>() );
	EXPECT_THROW( a2.get<co::TypeKind>(), co::IllegalCastException );

	// invalid retrieval: correct primitive, but it's a value, not a reference!
	EXPECT_THROW( a1.get<double&>(), co::IllegalCastException );

	// invalid retrieval: not a pointer/service
	EXPECT_THROW( a1.get<co::IService*>(), co::IllegalCastException );
}

TEST( AnyTests, coercionsFromEnum )
{
	co::Any a0( co::TK_NULL );
	co::Any a1( co::TK_FLOAT );
	co::Any a2( co::TK_COMPONENT );

	EXPECT_ANY_STREQ( a0, "in co.TypeKind: TK_NULL" );
	EXPECT_ANY_STREQ( a1, "in co.TypeKind: TK_FLOAT" );
	EXPECT_ANY_STREQ( a2, "in co.TypeKind: TK_COMPONENT" );

	// to bool
	EXPECT_FALSE( a0.get<bool>() );
	EXPECT_TRUE( a1.get<bool>() );
	EXPECT_TRUE( a2.get<bool>() );

	// to int16
	EXPECT_EQ( 0, a0.get<co::int16>() );
	EXPECT_EQ( 8, a1.get<co::int16>() );
	EXPECT_EQ( 17, a2.get<co::int16>() );

	// to float
	EXPECT_EQ( 0.0f, a0.get<float>() );
	EXPECT_EQ( 8.0f, a1.get<float>() );
	EXPECT_EQ( 17.0f, a2.get<float>() );

	// NOTE: if we had a second enum type in the core, we could
	// test direct enum-to-enum coercion

	// invalid retrievals: enums should not be retrieved by reference
	EXPECT_THROW( a0.get<co::TypeKind&>(), co::IllegalCastException );
}

TEST( AnyTests, coercionBetweenEnums )
{
	co::Any a0( co::TK_NULL );
	co::Any a1( static_cast<short>( 1 ) );
	co::Any a2( co::TK_INT8 );
	co::Any a3( co::TK_UINT32 );
	co::Any a4( co::ModuleState_Integrated );
	co::Any a5( co::__ModuleState__FORCE_SIZEOF_UINT32 );

	EXPECT_ANY_STREQ( a0, "in co.TypeKind: TK_NULL" );
	EXPECT_ANY_STREQ( a1, "in int16: 1" );
	EXPECT_ANY_STREQ( a2, "in co.TypeKind: TK_INT8" );
	EXPECT_ANY_STREQ( a3, "in co.TypeKind: TK_UINT32" );
	EXPECT_ANY_STREQ( a4, "in co.ModuleState: ModuleState_Integrated" );
	EXPECT_ANY_STREQ( a5, "in co.ModuleState: <INVALID ID #4294967295>" );

	EXPECT_EQ( 0, a0.get<short>() );
	EXPECT_EQ( co::ModuleState_None, a0.get<co::ModuleState>() );

	EXPECT_EQ( co::TK_BOOL, a1.get<co::TypeKind>() );
	EXPECT_EQ( co::ModuleState_Initialized, a1.get<co::ModuleState>() );

	EXPECT_EQ( 2, a2.get<co::int8>() );
	EXPECT_EQ( co::ModuleState_Integrated, a2.get<co::ModuleState>() );
	EXPECT_EQ( co::TK_INT8, a4.get<co::TypeKind>() );

	EXPECT_EQ( 7, a3.get<co::int16>() );
	EXPECT_THROW( a3.get<co::ModuleState>(), co::IllegalCastException );

	EXPECT_EQ( 4294967295U, a5.get<co::uint32>() );
	EXPECT_THROW( a5.get<co::TypeKind>(), co::IllegalCastException );
}

TEST( AnyTests, coercionsFromServices )
{
	co::Any a0( reinterpret_cast<co::IType*>( 0 ) );
	co::Any a1( co::typeOf<co::INamespace>::get() );

	EXPECT_ANY_STREQ( a0, "in co.IType: null" );
	EXPECT_ANY_TYPE_STREQ( a1, "in co.IInterface" );

	// to primitives (invalid retrievals):
	EXPECT_THROW( a0.get<bool>(), co::IllegalCastException );
	EXPECT_THROW( a1.get<co::int32>(), co::IllegalCastException );
	EXPECT_THROW( a1.get<std::string&>(), co::IllegalCastException );
	EXPECT_THROW( a0.get<co::TypeKind>(), co::IllegalCastException );

	// valid coercions to super-types:
	EXPECT_TRUE( a0.get<co::IService*>() == NULL );
	EXPECT_TRUE( a1.get<co::IService*>() != NULL );
	EXPECT_TRUE( a1.get<co::IType*>() != NULL );

	// valid coercion from null to any type
	EXPECT_TRUE( a0.get<co::INamespace*>() == NULL );

	// try coercions from a RefPtr
	co::ITypeRef typeRef( co::typeOf<co::INamespace>::get() );
	co::Any aRef( typeRef );

	EXPECT_NE( a1, aRef );
	EXPECT_EQ( a1, aRef.asIn() );

	EXPECT_EQ( typeRef, aRef.get<co::ITypeRef&>() );
	EXPECT_THROW( aRef.get<co::IServiceRef&>(), co::IllegalCastException );
	EXPECT_EQ( typeRef.get(), aRef.get<co::IType*>() );
	EXPECT_EQ( typeRef.get(), aRef.get<co::IService*>() );
}

TEST( AnyTests, coercionsFromSlice )
{
	// ValueType
	co::Slice<float> valueRange;
	co::Any valueRangeAny( valueRange );
	EXPECT_ANY_TYPE_STREQ( valueRangeAny, "in float[]" );

	// SubInterface*
	co::Slice<co::IInterface*> interfaceRange;
	co::Any interfaceRangeAny( interfaceRange );
	EXPECT_ANY_TYPE_STREQ( interfaceRangeAny, "in co.IInterface[]" );

	// SuperInterface*
	co::Slice<co::IType*> superInterfaceRange;
	co::Any superInterfaceRangeAny( superInterfaceRange );
	EXPECT_ANY_TYPE_STREQ( superInterfaceRangeAny, "in co.IType[]" );

	// ILLEGAL: ValueType -> DifferentValueType
	EXPECT_THROW( valueRangeAny.get<co::Slice<double> >(), co::IllegalCastException );

	// SubInterface* -> SuperInterface*
	EXPECT_TRUE( interfaceRangeAny.get<co::Slice<co::IType*> >().isEmpty() );

	// ILLEGAL: SuperInterface* -> SubInterface*
	EXPECT_THROW( superInterfaceRangeAny.get<co::Slice<co::IInterface*> >(), co::IllegalCastException );
}

TEST( AnyTests, coercionsFromRefVector )
{
	// SubInterface*
	std::vector<co::IInterfaceRef> refVector;
	refVector.push_back( co::typeOf<co::IModule>::get() );
	refVector.push_back( co::typeOf<co::INamespace>::get() );

	co::Any refVectorAny;
	refVectorAny.set<std::vector<co::IInterfaceRef>&>( refVector );
	EXPECT_ANY_TYPE_STREQ( refVectorAny, "out co.IInterface[]" );

	// std::vector<SubInterfaceRef> -> co::Slice<SubInterface*>
	EXPECT_EQ( 2, refVectorAny.get<co::Slice<co::IInterface*> >().getSize() );

	// std::vector<SubInterfaceRef> -> co::Slice<SuperInterface*>
	EXPECT_EQ( 2, refVectorAny.get<co::Slice<co::IType*> >().getSize() );

	co::Slice<co::IType*> r = refVectorAny.get<co::Slice<co::IType*> >();
	EXPECT_EQ( "co.IModule", r.getFirst()->getFullName() );
	EXPECT_EQ( "co.INamespace", r.getLast()->getFullName() );
}

TEST( AnyTests, coercionsFromStdVector )
{
	// ValueType
	std::vector<int> intVec;
	intVec.push_back( -1 );
	intVec.push_back( 5 );
	intVec.push_back( 7 );

	co::Any intVecAny;
	intVecAny.set<std::vector<int>&>( intVec );
	EXPECT_ANY_STREQ( intVecAny, "out int32[]: {-1, 5, 7}" );

	// SubInterface*
	std::vector<co::IInterface*> subVec;
	subVec.push_back( co::typeOf<co::IModule>::get() );
	subVec.push_back( co::typeOf<co::INamespace>::get() );

	co::Any subVecAny;
	subVecAny.set<std::vector<co::IInterface*>&>( subVec );
	EXPECT_ANY_TYPE_STREQ( subVecAny, "out co.IInterface[]" );

	// SuperInterface*
	std::vector<co::IType*> superVec;
	co::Any superVecAny;
	superVecAny.set<std::vector<co::IType*>&>( superVec );
	EXPECT_ANY_TYPE_STREQ( superVecAny, "out co.IType[]" );

	// std::vector<ValueType> -> co::Slice<ValueType>
	EXPECT_EQ( 3, intVecAny.get<co::Slice<int> >().getSize() );

	// ILLEGAL: std::vector<ValueType> -> co::Slice<DifferentValueType>
	EXPECT_THROW( intVecAny.get<co::Slice<short> >(), co::IllegalCastException );

	// std::vector<SubInterface*> -> co::Slice<SubInterface*>
	EXPECT_EQ( 2, subVecAny.get<co::Slice<co::IInterface*> >().getSize() );

	// ILLEGAL: std::vector<SubInterface*> -> std::vector<SuperInterface*>
	EXPECT_EQ( 2, subVecAny.get<std::vector<co::IInterface*>&>().size() );
	EXPECT_THROW( subVecAny.get<std::vector<co::IType*>&>(), co::IllegalCastException );

	// std::vector<SubInterface*> -> co::Slice<SuperInterface*>
	EXPECT_EQ( 2, subVecAny.get<co::Slice<co::IType*> >().getSize() );

	// ILLEGAL: std::vector<SuperInterface*> -> co::Slice<SubInterface*>
	EXPECT_THROW( superVecAny.get<co::Slice<co::IInterface*> >(), co::IllegalCastException );
}

TEST( AnyTests, putService )
{
	co::ITypeRef typeRef; co::Any aTypeRef( typeRef );

	co::IService* service = co::typeOf<co::IService>::get();
	co::IServiceRef ref( service );

	aTypeRef.put( service );
	EXPECT_EQ( typeRef.get(), service );

	aTypeRef.put( co::Any() );
	EXPECT_FALSE( typeRef );

	aTypeRef.put( ref );
	EXPECT_EQ( typeRef.get(), ref.get() );
}

TEST( AnyTests, putConversions )
{
	std::string str; co::Any aStr( str );
	double dbl; co::Any aDbl( dbl );
	
	aStr.put( 3.14 );
	EXPECT_EQ( "3.14", str );
	
	aDbl.put( str );
	EXPECT_EQ( 3.14, dbl );
	
	aStr.put( true );
	EXPECT_EQ( "true", str );
	
	aDbl.put( false );
	EXPECT_EQ( 0, dbl );
}

/*****************************************************************************/
/* Tests for the custom variable setters and constructors                    */
/*****************************************************************************/

TEST( AnyTests, setService )
{
	co::IInterface* nsType = co::typeOf<co::INamespace>::get();

	co::Any automatic( nsType );
	co::Any manual( true, nsType->getInterface(), &nsType );
	EXPECT_EQ( automatic, manual );
}

TEST( AnyTests, setVariable )
{
	const co::int16 anInt16 = 5;
	double aDouble = 3.14;
	const std::string aString( "Hey Joe" );
	co::TypeKind anEnum = co::TK_FLOAT;

	co::Any automatic( anInt16 );
	co::Any manual( true, co::typeOf<co::int16>::get(), &anInt16 );
	EXPECT_EQ( automatic, manual );

	automatic.set( aDouble );
	manual.set( false, co::typeOf<double>::get(), &aDouble );
	EXPECT_EQ( automatic, manual );

	automatic.set( aString );
	manual.set( true, co::typeOf<std::string>::get(), &aString );
	EXPECT_EQ( automatic, manual );

	automatic.set( anEnum );
	manual.set( false, co::typeOf<co::TypeKind>::get(), &anEnum );
	EXPECT_EQ( automatic, manual );
}

TEST( AnyTests, setArray )
{
	std::vector<co::int8> int8Vec;
	int8Vec.push_back( -1 );
	int8Vec.push_back( 5 );
	int8Vec.push_back( 7 );

	std::vector<co::IInterfaceRef> refVector;
	refVector.push_back( co::typeOf<co::IModule>::get() );
	refVector.push_back( co::typeOf<co::INamespace>::get() );

	co::Slice<float> floatArrayRange;
	co::Slice<co::int8> int8VecRange( int8Vec );

	co::Any automatic( int8Vec );
	co::Any manual( false, co::getType( "int8[]" ), &int8Vec );
	EXPECT_EQ( automatic, manual );

	automatic.set<std::vector<co::IInterfaceRef>&>( refVector );
	manual.set( false, co::getType( "co.IInterface[]" ), &refVector );
	EXPECT_EQ( automatic, manual );

	automatic.set( floatArrayRange );
	manual.set( true, co::getType( "float[]" ), NULL );
	EXPECT_EQ( automatic, manual );

	automatic.set( int8VecRange );
	manual.set( true, co::getType( "int8[]" ), &int8Vec.front(), int8Vec.size() );
	EXPECT_EQ( automatic, manual );
}

/*****************************************************************************/
/* Tests for Swap                                                            */
/*****************************************************************************/

TEST( AnyTests, swap )
{
	co::IType* type = co::typeOf<co::IType>::get();

	co::Any a( type ), b( 7 ), c( 3.14 );
	EXPECT_EQ( type, a.get<co::IType*>() );
	EXPECT_EQ( 7, b.get<int>() );
	EXPECT_EQ( 3.14, c.get<double>() );

	a.swap( b );
	EXPECT_EQ( 7, a.get<int>() );
	EXPECT_EQ( type, b.get<co::IType*>() );
	EXPECT_EQ( 3.14, c.get<double>() );

	c.swap( b );
	EXPECT_EQ( 7, a.get<int>() );
	EXPECT_EQ( 3.14, b.get<double>() );
	EXPECT_EQ( type, c.get<co::IType*>() );
}

/*****************************************************************************/
/* Tests for Reflective Array Accesses                                       */
/*****************************************************************************/

TEST( AnyTests, accessRangeOfDoubles )
{
	double dblArray[] = { -7.75, 3.14, 1000 };

	co::Any array( dblArray );
	ASSERT_TRUE( array.isValid() );
	EXPECT_TRUE( array.isIn() );
	EXPECT_EQ( co::TK_ARRAY, array.getType()->getKind() );

	EXPECT_EQ( 3, array.getCount() );
	EXPECT_EQ( sizeof(double), array.getElementSize() );

	for( co::uint32 i = 0; i < CORAL_ARRAY_LENGTH(dblArray); ++i )
	{
		co::Any elem = array[i];
		ASSERT_TRUE( elem.isValid() );
		EXPECT_TRUE( elem.isIn() );
		EXPECT_EQ( co::TK_DOUBLE, elem.getType()->getKind() );
		EXPECT_EQ( dblArray[i], elem.get<double>() );

		// cannot write to an 'in' array / element
		EXPECT_THROW( elem.put( 3.14 ), co::IllegalStateException );
	}
}

TEST( AnyTests, accessRangeOfStructs )
{
	co::CSLError cslErrorArray[2];
	cslErrorArray[0].filename = "filename1";
	cslErrorArray[0].message = "message1";
	cslErrorArray[0].line = 5;
	cslErrorArray[1].filename = "filename2";
	cslErrorArray[1].message = "message2";
	cslErrorArray[1].line = 9;

	co::Any elem;
	co::Any array( cslErrorArray );

	ASSERT_TRUE( array.isValid() );
	EXPECT_TRUE( array.isIn() );
	EXPECT_EQ( co::TK_ARRAY, array.getType()->getKind() );

	EXPECT_EQ( 2, array.getCount() );
	EXPECT_EQ( sizeof(co::CSLError), array.getElementSize() );

	elem = array[0];
	ASSERT_TRUE( elem.isValid() );
	EXPECT_TRUE( elem.isIn() );
	EXPECT_EQ( "co.CSLError", elem.getType()->getFullName() );
	EXPECT_EQ( "filename1", elem.get<const co::CSLError&>().filename );
	EXPECT_EQ( "message1", elem.get<const co::CSLError&>().message );
	EXPECT_EQ( 5, elem.get<const co::CSLError&>().line );

	elem = array[1];
	ASSERT_TRUE( elem.isValid() );
	EXPECT_TRUE( elem.isIn() );
	EXPECT_EQ( "co.CSLError", elem.getType()->getFullName() );
	EXPECT_EQ( "filename2", elem.get<const co::CSLError&>().filename );
	EXPECT_EQ( "message2", elem.get<const co::CSLError&>().message );
	EXPECT_EQ( 9, elem.get<const co::CSLError&>().line );
}

TEST( AnyTests, accessVectorOfStrings )
{
	std::vector<std::string> strVector;
	strVector.push_back( "foo" );
	strVector.push_back( "bar" );
	strVector.push_back( "omg" );
	strVector.push_back( "great" );

	co::Any array, elem;
	array.set( strVector );

	ASSERT_TRUE( array.isValid() );
	EXPECT_TRUE( array.isOut() );
	EXPECT_EQ( co::TK_ARRAY, array.getType()->getKind() );

	EXPECT_EQ( 4, array.getCount() );

	elem = array[0];
	ASSERT_TRUE( elem.isValid() );
	EXPECT_TRUE( elem.isOut() );
	EXPECT_EQ( co::TK_STRING, elem.getType()->getKind() );
	EXPECT_EQ( "foo", elem.get<const std::string&>() );

	elem = array[1];
	EXPECT_EQ( "bar", elem.get<const std::string&>() );

	elem = array[2];
	EXPECT_EQ( "omg", elem.get<const std::string&>() );

	elem = array[3];
	EXPECT_EQ( "great", elem.get<const std::string&>() );

	// test writing to the array element
	elem.put( std::string( "awesome" ) );
	EXPECT_EQ( "awesome", strVector[3] );
}
