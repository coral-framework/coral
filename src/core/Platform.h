#ifndef _CO_PLATFORM_H_
#define _CO_PLATFORM_H_

#ifndef __cplusplus
#error Coral requires a C++ compiler.
#endif

#include <co/Config.h>
#include <cassert>
#include <cstdint>

//---- Utility Macros ----------------------------------------------------------

// Supresses "unused parameter" warnings.
#define CORAL_UNUSED(x) (void) x;

// Returns the number of elements in a statically-allocated array.
#define CORAL_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

// Stringifies the result of the expansion of a macro argument.
#define CORAL_DO_STRINGIFY(X) #X
#define CORAL_STRINGIFY(X) CORAL_DO_STRINGIFY(X)

//---- Build Mode Detection ----------------------------------------------------

#if defined(_DEBUG) || !defined(NDEBUG)
#define CORAL_BUILD_MODE "debug"
#else
#define CORAL_NDEBUG
#define CORAL_BUILD_MODE "release"
#endif

//---- Recognized Operating Systems (CORAL_OS_x) -------------------------------

#if defined(__APPLE__) && defined(__GNUC__)
#define CORAL_OS_MAC
#define CORAL_OS_NAME "OSX"
#elif defined(__linux__) || defined(__linux)
#define CORAL_OS_LINUX
#define CORAL_OS_NAME "Linux"
#elif defined(_WIN32) || defined(__WIN32__)
#define CORAL_OS_WIN
#define CORAL_OS_NAME "Windows"
#else
#error Oops - unknown or unsupported OS!
#endif

#if !defined(CORAL_OS_WIN)
#define CORAL_OS_UNIX
#endif

//---- Recognized compilers (CORAL_CC_x) ---------------------------------------

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

//---- Determine the CORAL_BUILD_KEY -------------------------------------------

#if defined(__i386) || defined(__x86_64) || defined(_M_IX86) || defined(_M_X64)
#if CORAL_POINTER_SIZE == 4
#define CORAL_ARCH_NAME "x86_32"
#else
#define CORAL_ARCH_NAME "x86_64"
#endif
#else
#error Oops - unknown or unsupported architecture!
#endif

#if defined(CORAL_CC_MSVC)
#define CORAL_CC_NAME "msvc"
#define CORAL_CC_VERSION CORAL_STRINGIFY(_MSC_VER)
#elif defined(CORAL_CC_CLANG)
#define CORAL_CC_NAME "clang"
#define CORAL_CC_VERSION \
  CORAL_STRINGIFY(__clang_major__) "." CORAL_STRINGIFY(__clang_minor__)
#elif defined(CORAL_CC_GNU)
#if defined(CORAL_CC_MINGW)
#define CORAL_CC_NAME "mingw"
#elif defined(CORAL_CC_LLVM)
#define CORAL_CC_NAME "llvm-gcc"
#else
#define CORAL_CC_NAME "gcc"
#endif
#define CORAL_CC_VERSION \
  CORAL_STRINGIFY(__GNUC__) "." CORAL_STRINGIFY(__GNUC_MINOR__)
#else
#error "Unknown compiler! Coral requires one compatible with GCC or MSVC."
#endif

#define CORAL_BUILD_KEY \
  CORAL_OS_NAME " " CORAL_ARCH_NAME " " CORAL_CC_NAME "-" CORAL_CC_VERSION

//---- Portable shared-library interface attributes ----------------------------

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
#else  // assumes the compiler is GCC-compatible
#define CORAL_DLL_EXPORT __attribute__((visibility("default")))
#define CORAL_EXPORT CORAL_DLL_EXPORT
#endif

#if defined(CORAL_OS_WIN)
#define CORAL_EXPORT_EXCEPTION
#else
#define CORAL_EXPORT_EXCEPTION CORAL_DLL_EXPORT
#endif

//---- Fixed-width integer types -----------------------------------------------

namespace co {
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
// Please avoid using unsigned types as they're a common source of bugs!
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
}  // namespace co

#endif  // _CO_PLATFORM_H_
