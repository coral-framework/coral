#include <co/Coral.h>
#include <co/RefPtr.h>
#include <co/ITypeManager.h>
#include <co/private/LexicalUtils.h>
#include <gtest/gtest.h>

TEST( LexicalUtilsTests, isValidIdentifier )
{
	ASSERT_TRUE( co::LexicalUtils::isValidIdentifier( "A" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidIdentifier( "a" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidIdentifier( "Ab_cd" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidIdentifier( "d123" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidIdentifier( "lalalalalala" ) );

	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( "" ) );
	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( "1lalla" ) );
	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( " lalala" ) );
	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( "lalala " ) );
	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( "lalala!" ) );
	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( "lalala lelele" ) );
	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( " " ) );
	ASSERT_FALSE( co::LexicalUtils::isValidIdentifier( "." ) );
}

TEST( LexicalUtilsTests, isValidField )
{
	ASSERT_TRUE( co::LexicalUtils::isValidFieldName( "a" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidFieldName( "ala" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidFieldName( "ab_cd" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidFieldName( "d123" ) );
	ASSERT_TRUE( co::LexicalUtils::isValidFieldName( "lalalalalala" ) );

	ASSERT_FALSE( co::LexicalUtils::isValidFieldName( "" ) );
	ASSERT_FALSE( co::LexicalUtils::isValidFieldName( "Ae" ) );
	ASSERT_FALSE( co::LexicalUtils::isValidFieldName( "Baaaa" ) );
}

TEST( LexicalUtilsTests, formatAccessor )
{
	std::string fieldName;

	co::LexicalUtils::formatAccessor( "name", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( fieldName, "getName" );

	co::LexicalUtils::formatAccessor( "name", co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( fieldName, "setName" );

	co::LexicalUtils::formatAccessor( "Name", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( fieldName, "getName" );

	co::LexicalUtils::formatAccessor( "Name", co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( fieldName, "setName" );

	co::LexicalUtils::formatAccessor( "a", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( fieldName, "getA" );

	co::LexicalUtils::formatAccessor( "a", co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( fieldName, "setA" );
}

TEST( LexicalUtilsTests, parseAccessor )
{
	int res;
	std::string fieldName;

	res = co::LexicalUtils::parseAccessor( "getName", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( "name", fieldName );
	ASSERT_EQ( co::LexicalUtils::Getter, res );

	res = co::LexicalUtils::parseAccessor( "getName", co::LexicalUtils::Getter | co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( "name", fieldName );
	ASSERT_EQ( co::LexicalUtils::Getter, res );

	res = co::LexicalUtils::parseAccessor( "setName", co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( "name" , fieldName );
	ASSERT_EQ( co::LexicalUtils::Setter, res );

	res = co::LexicalUtils::parseAccessor( "setName", co::LexicalUtils::Getter | co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( "name", fieldName );
	ASSERT_EQ( co::LexicalUtils::Setter, res );

	res = co::LexicalUtils::parseAccessor( "getname", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "GetName", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "getName", co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "get", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "is", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "set", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "a", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "setName", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "isName", co::LexicalUtils::Getter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "isName", co::LexicalUtils::Getter | co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( -1, res );

	res = co::LexicalUtils::parseAccessor( "setName", co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( "name" , fieldName );
	ASSERT_EQ( co::LexicalUtils::Setter, res );

	res = co::LexicalUtils::parseAccessor( "setName", co::LexicalUtils::Getter | co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( "name", fieldName );
	ASSERT_EQ( co::LexicalUtils::Setter, res );

	res = co::LexicalUtils::parseAccessor( "isName", co::LexicalUtils::Setter, fieldName );
	ASSERT_EQ( -1, res );
}
