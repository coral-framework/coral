#ifndef _CO_CRYPTOHASH_H_
#define _CO_CRYPTOHASH_H_

#include <co/Platform.h>
#include <co/reserved/Uuid.h>
#include <string>
#include <cstring>

namespace co {

class CryptoHashAlgorithm;

/*!
	The CryptoHash class provides a way to generate cryptographic hashes.
	CryptoHash can be used to generate cryptographic hashes of binary or text data.
	Currently only SHA1 is supported.
 */
class CORAL_EXPORT CryptoHash
{
public:
	enum Algorithm
	{
		Sha1
	};

	struct CORAL_EXPORT Result
	{
		Algorithm algorithm;
		int8 numBytes;
		union
		{
			uint8  bytes[20];
			uint32 dwords[5];
		}
		data;

		//! Writes the hash result to 'str' in hexadecimal.
		void toString( std::string& str );

		//! Uses the hash result to create a co::Uuid.
		void toUuid( Uuid& uuid );
	};

public:
	//! Constructs an object that uses the specified 'method' to create cryptographic hashes.
	CryptoHash( Algorithm method );

	//! Destructor.
	~CryptoHash();

	/*!
		Adds the first length chars of data to the cryptographic hash.
		\throw IllegalStateException if the hash result was already calculated.
	 */
	void addData( const uint8* data, size_t length );

	//! This is an overloaded member function, provided for convenience. See addData().
	//@{
	inline void addData( const char* cstr )
	{ 
		addData( reinterpret_cast<const uint8*>( cstr ), strlen( cstr ) );
	}

	inline void addData( const std::string& str )
	{ 
		addData( reinterpret_cast<const uint8*>( str.data() ), str.length() );
	}

	template<typename T>
	inline void addData( const T& data )
	{ 
		addData( reinterpret_cast<const uint8*>( &data ), sizeof(data) );
	}
	//@}

	//! Calculates/retrieves the final hash result.
	void getResult( Result& res ) const;

	//! Resets the object.
	void reset();

private:
	// forbid copies.
	CryptoHash( const CryptoHash& other );
	CryptoHash& operator=( const CryptoHash& other );

private:
	CryptoHashAlgorithm* _algorithm;
};

} // namespace co

#endif // _CO_CRYPTOHASH_H_
