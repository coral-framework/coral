#ifndef _CO_PLATFORM_H_
#define _CO_PLATFORM_H_

/*!
	\file
	Platform Info, Portable Types and Utilities
 */

#ifndef __cplusplus
	#error Coral requires a C++ compiler.
#endif

#include <co/Config.h>
#include <cassert>
#include <cstddef>

//------ Build Mode Detection --------------------------------------------------

#if defined(_DEBUG) || !defined(NDEBUG)
	#define CORAL_BUILD_MODE "debug"
#else
	#define CORAL_NDEBUG
	#define CORAL_BUILD_MODE "release"
#endif

//------ Recognized Operating Systems (CORAL_OS_x) -----------------------------

#if defined(__APPLE__) && defined(__GNUC__)
	#define CORAL_OS_MAC
#elif defined(__linux__) || defined(__linux)
	#define CORAL_OS_LINUX
#elif defined(_WIN32) || defined(__WIN32__)
	#define CORAL_OS_WIN
#else
	#error Oops - unknown or unsupported OS!
#endif

#if !defined(CORAL_OS_WIN)
	#define CORAL_OS_UNIX
#endif

//------ Recognized compilers (CORAL_CC_x) -------------------------------------

#if defined(_MSC_VER)
	#define CORAL_CC_MSVC
#elif defined(__GNUC__)
	#define CORAL_CC_GNU
	#if defined(__MINGW32__)
		#define CORAL_CC_MINGW
	#elif defined(__llvm__)
		#define CORAL_CC_LLVM
		#if defined(__clang__)
			#define CORAL_CC_CLANG
		#endif
	#endif
#endif

//------ Determine the CORAL_BUILD_KEY -----------------------------------------

#if defined(CORAL_OS_LINUX)
	#define CORAL_OS_NAME "Linux"
#elif defined(CORAL_OS_MAC)
	#define CORAL_OS_NAME "MacOSX"
#elif defined(CORAL_OS_WIN)
	#define CORAL_OS_NAME "Windows"
#else
	#error "Unsupported operating system! Coral requires Linux, MacOSX or Windows."
#endif

#if defined(__i386) || defined(__x86_64) || defined(_M_IX86) || defined(_M_X64)
	#if CORAL_POINTER_SIZE == 4
		#define CORAL_ARCH_NAME "x86_32"
	#else
		#define CORAL_ARCH_NAME "x86_64"
	#endif
#else
	#error "Unsupported architecture! Coral requires a 32 or 64-bit x86 processor."
#endif

#define CORAL_DO_STRINGIFY(X) #X
#define CORAL_STRINGIFY(X) CORAL_DO_STRINGIFY(X)

#if defined(CORAL_CC_MSVC)
	#define CORAL_CC_NAME "msvc"
	#if _MSC_VER > 1700
		#error "Your MSVC compiler was not recognized by this Coral version (maybe it's too new?)."
	#elif _MSC_VER >= 1700
		#define CORAL_CC_VERSION "11.0"
	#elif _MSC_VER >= 1600
		#define CORAL_CC_VERSION "10.0"
	#else
		#error "Coral requires Visual C++ 2010 or later."
	#endif
#elif defined(CORAL_CC_CLANG)
	#define CORAL_CC_NAME "clang"
	#define CORAL_CC_VERSION CORAL_STRINGIFY(__clang_major__) "." CORAL_STRINGIFY(__clang_minor__)
#elif defined(CORAL_CC_GNU)
	#if defined(CORAL_CC_MINGW)
		#define CORAL_CC_NAME "mingw"
	#elif defined(CORAL_CC_LLVM)
		#define CORAL_CC_NAME "llvm-gcc"
	#else
		#define CORAL_CC_NAME "gcc"
	#endif
	#define CORAL_CC_VERSION CORAL_STRINGIFY(__GNUC__) "." CORAL_STRINGIFY(__GNUC_MINOR__)
#else
	#error "Unknown compiler! Coral requires one compatible with GCC or MSVC."
#endif

#define CORAL_BUILD_KEY		CORAL_OS_NAME " " CORAL_ARCH_NAME " " CORAL_CC_NAME "-" CORAL_CC_VERSION


//------ Portable Integer Types ------------------------------------------------

#if defined(CORAL_CC_GNU)
	#include <sys/types.h>
#endif

//! Coral's main namespace.
namespace co {

#if CORAL_POINTER_SIZE == 4
	typedef char				int8;
	typedef unsigned char		uint8;
	typedef short				int16;
	typedef unsigned short		uint16;
	typedef int					int32;
	typedef unsigned			uint32;
	typedef int32				intptr;
	typedef uint32				uintptr;
#elif CORAL_POINTER_SIZE == 8
	typedef char				int8;
	typedef unsigned char		uint8;
	typedef short				int16;
	typedef unsigned short		uint16;
	typedef int					int32;
	typedef unsigned			uint32;
#else
	#error Portable integers were not defined because CORAL_POINTER_SIZE is invalid.
#endif

// Limits for the portable integer types.
// (current definitions are only valid for x86 architectures)
const int8		MIN_INT8	= -127 - 1;
const int8		MAX_INT8	=  127;
const uint8		MAX_UINT8	=  0xFF;

const int16		MIN_INT16	= -32767 - 1;
const int16		MAX_INT16	=  32767;
const uint16	MAX_UINT16	= 0xFFFF;

const int32		MIN_INT32	= -2147483647 - 1;
const int32		MAX_INT32	=  2147483647;
const uint32	MAX_UINT32	=  0xFFFFFFFF;

} // namespace co

/*!
	\brief Imports all portable integer types into the current namespace.

	This is useful if you want to write \c uint8, \c int32, etc. in your code.
 */
#define USING_CORAL_INTEGER_TYPES	\
	using co::int8;		\
	using co::int16;	\
	using co::int32;	\
	using co::uint8;	\
	using co::uint16;	\
	using co::uint32;

/*!
	\brief Returns the number of elements in a statically-allocated array.
	\param[in] array name of a statically-allocated array.
 */
#define CORAL_ARRAY_LENGTH( array )	( sizeof( array ) / sizeof( array[0] ) )

//! Supresses 'unused parameter' warnings.
#define CORAL_UNUSED( x ) (void)x;


//------ Portable shared-library interface attributes --------------------------

#ifndef DOXYGEN

#if defined(CORAL_NO_EXPORT)
	#define CORAL_EXPORT
	#define CORAL_DLL_EXPORT
#elif defined(CORAL_OS_WIN)
	#define CORAL_DLL_EXPORT __declspec(dllexport)
	#if defined(BUILDING_CORAL_CORE)
		#define CORAL_EXPORT CORAL_DLL_EXPORT
	#else
		#define CORAL_EXPORT __declspec(dllimport)
	#endif
#else // assumes the compiler is GCC-compatible
	#define CORAL_DLL_EXPORT __attribute__((visibility("default")))
	#define CORAL_EXPORT CORAL_DLL_EXPORT
#endif

#if defined(CORAL_OS_WIN)
	#define CORAL_EXPORT_EXCEPTION
#else
	#define CORAL_EXPORT_EXCEPTION CORAL_DLL_EXPORT
#endif

#endif // DOXYGEN

#endif // _CO_PLATFORM_H_
