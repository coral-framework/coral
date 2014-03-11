#ifndef _CO_ENDIAN_H_
#define _CO_ENDIAN_H_

#include <co/Platform.h>

namespace co {

/*!
	Swaps the byte order of a 16-bit integer.
	\ingroup platform
 */
inline uint16 endianSwap( uint16 x )
{
	return ( x >> 8 ) | ( x << 8 );
}

/*!
	Swaps the byte order of a 32-bit integer.
 	\ingroup platform
 */
inline uint32 endianSwap( uint32 x )
{
	return 	( x >> 24 ) |
			( ( x << 8 ) & 0x00FF0000 ) |
			( ( x >> 8 ) & 0x0000FF00 ) |
			( x << 24 );
}

/*!
	\brief Converts \a v from big-endian byte order to the host platform's byte order.

	On big-endian platforms (such as PowerPC), this will return \a v unmodified.

	\ingroup platform
 */
template<typename T>
inline T fromBigEndian( T v )
{
#if CORAL_BYTE_ORDER == CORAL_LITTLE_ENDIAN
	return endianSwap( v );
#else
	return v;
#endif
}

/*!
	\brief Converts \a v from little-endian byte order to the host platform's byte order.

	On little-endian platforms (such as x86), this will return \a v unmodified.

	\ingroup platform
 */
template<typename T>
inline T fromLittleEndian( T v )
{
#if CORAL_BYTE_ORDER == CORAL_LITTLE_ENDIAN
	return v;
#else
	return endianSwap( v );
#endif
}

/*!
	\brief Converts \a v from the host platform's byte order to big-endian byte order.

	On big-endian platforms (such as PowerPC), this will return \a v unmodified.

	\ingroup platform
 */
template<typename T>
inline T toBigEndian( T v )
{
#if CORAL_BYTE_ORDER == CORAL_LITTLE_ENDIAN
	return endianSwap( v );
#else
	return v;
#endif
}

/*!
	\brief Converts \a v from the host platform's byte order to little-endian byte order.

	On little-endian platforms (such as x86), this will return \a v unmodified.

	\ingroup platform
 */
template<typename T>
inline T toLittleEndian( T v )
{
#if CORAL_BYTE_ORDER == CORAL_LITTLE_ENDIAN
	return v;
#else
	return endianSwap( v );
#endif
}

} // namespace co

#endif // _CO_ENDIAN_H_
