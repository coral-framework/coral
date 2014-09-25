#ifndef _CO_RESERVED_OS_H_
#define _CO_RESERVED_OS_H_

//! \file OS.h Portable OS types, macros and functions.

#include <co/Platform.h>
#include <co/Slice.h>
#include <string>

#if defined(CORAL_OS_WINDOWS)
	#define CORAL_OS_DIR_SEP '\\'
	#define CORAL_OS_DIR_SEP_STR "\\"
	#define CORAL_OS_PATH_SEP ';'
	#define CORAL_OS_PATH_SEP_STR ";"
#else
	#define CORAL_OS_DIR_SEP '/'
	#define CORAL_OS_DIR_SEP_STR "/"
	#define CORAL_OS_PATH_SEP ':'
	#define CORAL_OS_PATH_SEP_STR ":"
#endif

namespace co {

/*!
	\brief Portable OS functions in a static class.
 */
class CORAL_EXPORT OS
{
public:
	//! Gets the application's directory (where its executable file is located).
	static bool getApplicationDir( std::string& path );

	//! Gets the current working directory.
	static bool getCurrentWorkingDir( std::string& path );

	/*!
		\brief Returns true if \a path is an absolute path.
		On UNIX, that means it begins with a slash; on Windows that it begins with a drive letter.
	 */
	static bool isAbs( const std::string& path );

	//! Returns true if \a path is an existing regular file.
	static bool isFile( const std::string& path );

	//! Returns true if \a path is an existing directory.
	static bool isDir( const std::string& path );

	/*!
		\brief Converts a (Coral-style) dot-separated path to an OS-specific path.
		This method changes the string inplace, replacing every dot with a directory separator.
	 */
	static void convertDotsToDirSeps( std::string& dotSeparatedPath );

	/*!
		\brief Removes redundant separators and level-up references, so that
			'A//B', 'A/B/', 'A/./B' and 'A/foo/../B' all become 'A/B'.
		On Windows, it also converts forward slashes to backward slashes.
	 */
	static void normalizePath( std::string& path );

	/*!
		\brief Returns a normalized, absolutized version of \a path.
		This prepends the current working dir to any relative path.
	 */
	static void makeAbs( std::string& path );

	/*!
		\brief Searches for a file in a space defined by a combination of two string lists.

		File path combinations are evaluated in the same order they're listed, and in case of
		ambiguities the search always returns the first match.

		\param[in] listA List of base directory paths (e.g. pass the CORAL_PATH here).
		\param[in] listB List of relative file paths (e.g. potential file names).
		\param[out] path Path to the first file found (only set if the method returns true).
		\param[out] partA optional; set with the substring of \a path that came from \a listA.
		\param[out] partB optional; set with the substring of \a path that came from \a listB.

		\return true if a file is found; false otherwise.
	*/
	static bool searchFile2( co::Slice<std::string> listA,
							 co::Slice<std::string> listB,
							 std::string& path,
							 std::string* partA = 0,
							 std::string* partB = 0 );

	/*!
		\brief Searches for a file in a space defined by a combination of three string lists.

		File path combinations are evaluated in the same order they're listed, and in case of
		ambiguities the search always returns the first match.

		\param[in] listA Outer-most list of base directory paths (e.g. pass the CORAL_PATH here).
		\param[in] listB Central list of relative directory paths (e.g. pass a list of module names here).
		\param[in] listC Inner-most list of relative file paths (e.g. pass a list of file names here).
		\param[out] path Path to the first file found (only set if the method returns true).
		\param[out] partA optional; set with the substring of \a path that came from \a listA.
		\param[out] partB optional; set with the substring of \a path that came from \a listB.
		\param[out] partC optional; set with the substring of \a path that came from \a listC.

		\return true if a file is found; false otherwise.
	*/
	static bool searchFile3( co::Slice<std::string> listA,
							 co::Slice<std::string> listB,
							 co::Slice<std::string> listC,
							 std::string& path,
							 std::string* partA = 0,
							 std::string* partB = 0,
							 std::string* partC = 0 );
};

} // namespace co

#endif // _CO_RESERVED_OS_H_
