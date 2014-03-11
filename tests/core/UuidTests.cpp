#include <co/Uuid.h>
#include <co/Exception.h>
#include <core/utils/CryptoHash.h>
#include <gtest/gtest.h>

using namespace co;

void EXPECT_UUID_STR( const Uuid& uuid, const char* expectedStr )
{
	std::string uuidStr;
	uuid.getString( uuidStr );
	EXPECT_EQ( expectedStr, uuidStr );
}

TEST( UuidTests, constructors )
{
	Uuid uuidTestNull;
	EXPECT_TRUE( uuidTestNull.isNull() );

	uint8 node[6];
	node[0] = 0;
	node[1] = 0;
	node[2] = 0;
	node[3] = 0;
	node[4] = 0;
	node[5] = 0;

	Uuid uuidFromData = Uuid( 0, 0, 0, 0, 0, node );
	EXPECT_TRUE( uuidFromData.isNull() );
	EXPECT_TRUE( uuidFromData.getVersion() == Uuid::VerUnknown );

	// ** Tests with time version (extracted from Linux Man Page of uuid(1) command)
	// http://linux.die.net/man/1/uuid
	/*
	  corresponds to content time:  2004-01-15 16:22:26.376322.1 UTC
				 clock: 15472 (usually random)
				 node:  00:90:27:2f:f7:25 (global unicast)
	 */
	node[5] = 0x00;
	node[4] = 0x90;
	node[3] = 0x27;
	node[2] = 0x2f;
	node[1] = 0xf7;
	node[0] = 0x25;
	uuidFromData = Uuid( 0x01c47915, 0x4777, 0x11d8, 0xbc, 0x70, node );
	EXPECT_UUID_STR( uuidFromData, "01C47915-4777-11D8-BC700090272FF725" );
	EXPECT_FALSE( uuidFromData.isNull() );

	Uuid uuidFromOther = Uuid( uuidFromData );
	EXPECT_UUID_STR( uuidFromOther, "01C47915-4777-11D8-BC700090272FF725" );
	EXPECT_FALSE( uuidFromOther.isNull() );

	// expect version == 1 as specified in DCE standard
	EXPECT_TRUE( uuidFromData.getVersion() == 1 );

	Uuid uuidFromText( "9E107d9D-372B-6382-6Dd81d3542a419d6" );
	EXPECT_FALSE( uuidFromText.isNull() );
	EXPECT_TRUE( uuidFromText.getVersion() == Uuid::Md5 );
	EXPECT_UUID_STR( uuidFromText, "9E107D9D-372B-6382-6DD81D3542A419D6" );

	Uuid shouldBecomeNull( "9E107D9D-malformed-string3542A419D6" );
	EXPECT_TRUE( shouldBecomeNull.isNull() );
	EXPECT_UUID_STR( shouldBecomeNull, "00000000-0000-0000-0000000000000000" );
}

TEST( UuidTests, stringParsing )
{
	Uuid uuid;
	ASSERT_TRUE( uuid.isNull() );

	// it's ok to have only the opening curly brace
	uuid.setString( "{9e107d9d-372b-6382-6bd81d3542a419d6" );
	EXPECT_UUID_STR( uuid, "9E107D9D-372B-6382-6BD81D3542A419D6" );

	// invalid last char
	uuid.setString( "{9e107d9d-372b-6382-6bd81d3542a419d6@" );
	EXPECT_TRUE( uuid.isNull() );

	// it's ok to have only the closing curly brace
	uuid.setString( "9e107d9d-372b-6382-6bd81d3542a419d6}" );
	EXPECT_UUID_STR( uuid, "9E107D9D-372B-6382-6BD81D3542A419D6" );
	
	// missing dashes
	uuid.setString( "9e107d9d372b63826bd81d3542a419d6" );
	EXPECT_TRUE( uuid.isNull() );

	// misplaced dashes
	uuid.setString( "9e107d96d-372b-6382-6bd8d3542a419d6" );
	EXPECT_TRUE( uuid.isNull() );
}

TEST( UuidTests, creationMethods )
{
	CryptoHash hash( CryptoHash::Sha1 );
	hash.addData( "This is a test or what?" );
	CryptoHash::Result result;
	hash.getResult( result );

	Uuid hashUuid;
	result.toUuid( hashUuid );
	EXPECT_FALSE( hashUuid.isNull() );
	EXPECT_TRUE( hashUuid.getVersion() == Uuid::Sha1 );
	std::string strResult;
	hashUuid.getString( strResult );
	EXPECT_EQ( "6B8E76FA-378A-6541-D97DAA3928A8F639", strResult );

	Uuid randomUuid = Uuid::createRandom();
	EXPECT_FALSE( randomUuid.isNull() );
	EXPECT_TRUE( randomUuid.getVersion() == Uuid::Random );
}

TEST( UuidTests, operators )
{
	CryptoHash hash1( CryptoHash::Sha1 );
	hash1.addData( "Is this a test or what?" );
	CryptoHash::Result result;
	hash1.getResult( result );

	Uuid hashUuid;
	result.toUuid( hashUuid );
	EXPECT_FALSE( hashUuid.isNull() );
	EXPECT_TRUE( hashUuid.getVersion() == Uuid::Sha1 );

	Uuid hashCopy = hashUuid;
	EXPECT_TRUE( hashCopy == hashUuid );
	EXPECT_FALSE( hashCopy == Uuid() );

	std::string strResult, strResult2;
	hashUuid.getString( strResult );
	hashCopy.getString( strResult2 );
	EXPECT_TRUE( strResult == strResult2 );

	uint8 node[6];
	node[0] = 0;
	node[1] = 0;
	node[2] = 0;
	node[3] = 0;
	node[4] = 0;
	node[5] = 0;

	Uuid zero( 0, 0, 0, 0, 0, node );
	Uuid one( 0, 0, 0, 0, 1, node );
	Uuid two( 0, 0, 0, 0, 2, node );
	EXPECT_TRUE( zero < one );
	EXPECT_TRUE( one < two );
	EXPECT_FALSE( two < two );

	Uuid big( 0, 0, 1, 0, 0, node );
	Uuid bigger( 1, 0, 0, 0, 0, node );
	EXPECT_TRUE( big < bigger );
}
