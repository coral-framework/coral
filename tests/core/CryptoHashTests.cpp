#include <core/utils/CryptoHash.h>
#include <gtest/gtest.h>

using namespace co;

const uint8 BINARY_INPUT[] = {
	0x00, 0x83, 0xc4, 0x0c, 0xe8, 0x17, 0x06, 0x00,
	0x00, 0xe8, 0x22, 0x07, 0x00, 0x00, 0x83, 0xec 
};

TEST( CryptoHashTests, sha1TextInput )
{
	// Test generation of cryptographic Sha1 hashes from text data.
	CryptoHash::Result result;
	std::string textResult;

	CryptoHash sha1( CryptoHash::Sha1 );
	sha1.addData( "The quick brown fox jumps over the lazy dog" );
	sha1.getResult( result );
	result.toString( textResult );

	EXPECT_TRUE( textResult == "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12" );

	sha1.reset();
	sha1.addData( "The quick brown fox jumps over the lazy cog" );
	sha1.getResult( result );
	result.toString( textResult );

	EXPECT_TRUE( textResult == "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3" );

	// test the Zero Sha1 hash (empty)
	sha1.reset();
	sha1.addData( "" );
	sha1.getResult( result );
	result.toString( textResult );

	EXPECT_TRUE( textResult == "da39a3ee5e6b4b0d3255bfef95601890afd80709" );

	sha1.reset();
	sha1.addData( "O rato roeu a roupa do rei de roma!" );
	sha1.getResult( result );
	result.toString( textResult );

	EXPECT_TRUE( textResult == "1afdba2672374d713efe596edf1927fc6976bca3" );

	sha1.reset();
	sha1.addData( "Mr fanferson is nice guy! Sure he is!" );
	sha1.getResult( result );
	result.toString( textResult );

	EXPECT_TRUE( textResult == "822ed20c81ed5ca3af71b3789766ccacebd15aac" );
}

TEST( CryptoHashTests, binaryInput )
{
	// Test generation of cryptographic hashes from binary data.
	CryptoHash::Result result;
	std::string textResult;

	CryptoHash sha1( CryptoHash::Sha1 );
	sha1.addData( BINARY_INPUT, CORAL_ARRAY_LENGTH(BINARY_INPUT) );
	sha1.getResult( result );
	result.toString( textResult );

	EXPECT_TRUE( textResult == "083d1a9c812a0c419c0e6bc796a025aff8f562c6" );
}

TEST( CryptoHashTests, convenienceMethods )
{
	CryptoHash hashA( CryptoHash::Sha1 ), hashB( CryptoHash::Sha1 );

	CryptoHash::Result resultA, resultB;
	std::string textResultA, textResultB;

	double PI = 3.14159265;
	hashA.addData( reinterpret_cast<const co::uint8*>( &PI ), sizeof(double) );
	hashB.addData( PI );

	hashA.getResult( resultA );
	resultA.toString( textResultA );

	hashB.getResult( resultB );
	resultB.toString( textResultB );

	EXPECT_EQ( textResultA, textResultB );

	hashA.reset();
	hashB.reset();

	co::uint32 v = 798643;

	hashA.addData( reinterpret_cast<const co::uint8*>( &v ), sizeof(v) );
	hashB.addData( v );

	hashA.getResult( resultA );
	resultA.toString( textResultA );
	
	hashB.getResult( resultB );
	resultB.toString( textResultB );

	EXPECT_EQ( textResultA, textResultB );
}

TEST( CryptoHashTests, getBinaryResult )
{
	// test generation of binary Sha1 digests
	CryptoHash::Result sha1BinaryResult;

	CryptoHash sha1( CryptoHash::Sha1 );
	sha1.addData( BINARY_INPUT, CORAL_ARRAY_LENGTH(BINARY_INPUT ) );
	sha1.getResult( sha1BinaryResult );

	EXPECT_EQ( sha1BinaryResult.data.bytes[0],  0x08 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[1],  0x3d );
	EXPECT_EQ( sha1BinaryResult.data.bytes[2],  0x1a );
	EXPECT_EQ( sha1BinaryResult.data.bytes[3],  0x9c );
	EXPECT_EQ( sha1BinaryResult.data.bytes[4],  0x81 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[5],  0x2a );
	EXPECT_EQ( sha1BinaryResult.data.bytes[6],  0x0c );
	EXPECT_EQ( sha1BinaryResult.data.bytes[7],  0x41 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[8],  0x9c );
	EXPECT_EQ( sha1BinaryResult.data.bytes[9],  0x0e );
	EXPECT_EQ( sha1BinaryResult.data.bytes[10], 0x6b );
	EXPECT_EQ( sha1BinaryResult.data.bytes[11], 0xc7 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[12], 0x96 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[13], 0xa0 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[14], 0x25 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[15], 0xaf );
	EXPECT_EQ( sha1BinaryResult.data.bytes[16], 0xf8 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[17], 0xf5 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[18], 0x62 );
	EXPECT_EQ( sha1BinaryResult.data.bytes[19], 0xc6 );
}

TEST( CryptoHashTests, resultStructTest )
{
	CryptoHash::Result result;

	CryptoHash sha1( CryptoHash::Sha1 );
	sha1.addData( BINARY_INPUT, CORAL_ARRAY_LENGTH(BINARY_INPUT ) );
	sha1.getResult( result );

	EXPECT_EQ( result.algorithm, CryptoHash::Sha1 );
	EXPECT_EQ( result.numBytes, 20 );
}
