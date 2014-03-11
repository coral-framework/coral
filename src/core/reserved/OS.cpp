#include "OS.h"
#include "../utils/StringTokenizer.h"
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

// enable large file support on Linux
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#ifdef CORAL_OS_WIN
	#if _INTEGRAL_MAX_BITS >= 64
		#define CORAL_STAT_STRUCT struct _stati64
		#define CORAL_STAT_FUNC _stati64
	#else
		#define CORAL_STAT_STRUCT struct _stati32
		#define CORAL_STAT_FUNC _stati32
	#endif
#else
	#define CORAL_STAT_STRUCT struct stat
	#define CORAL_STAT_FUNC stat
#endif

#ifdef CORAL_OS_WIN
	#ifndef S_ISDIR
		#define S_ISDIR( mode )  ( mode & _S_IFDIR )
	#endif
	#ifndef S_ISREG
		#define S_ISREG( mode )  ( mode & _S_IFREG )
	#endif
#endif

#if defined(CORAL_OS_WIN)
	#include <direct.h>
	#define getCWD _getcwd
#else
	#include <errno.h>
	#include <unistd.h>
	#define getCWD getcwd
#endif

#if defined(CORAL_OS_WIN)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#elif defined(CORAL_OS_MAC)
	#include <mach-o/dyld.h>
#endif

bool co::OS::getApplicationDir( std::string& path )
{
	char buffer[FILENAME_MAX];

#if defined(CORAL_OS_WIN)
	DWORD count;
	if( ( count = GetModuleFileNameA( NULL, buffer, sizeof(buffer) ) ) == 0 )
		return false;
	path.assign( buffer, count );
#elif defined(CORAL_OS_LINUX)
	ssize_t count = readlink( "/proc/self/exe", buffer, sizeof(buffer) - 1 );
	if( count == -1 )
		return false;
	path.assign( buffer, count );
#elif defined(CORAL_OS_MAC)
	uint32_t count = sizeof(buffer);
	if( _NSGetExecutablePath( buffer, &count ) != 0 )
		return false;
	char* realPath = realpath( buffer, NULL );
	path = realPath;
	free( realPath );
#else
	#error Unknown or unsupported platform.
#endif

	// remove the trailing "/filename"
	size_t lastSep = path.rfind( CORAL_OS_DIR_SEP );
	if( lastSep == std::string::npos )
	{
		assert( false );
		return false;
	}

	path.resize( lastSep );
	return true;
}

bool co::OS::getCurrentWorkingDir( std::string& path )
{
	char buffer[FILENAME_MAX];
	if( !getCWD( buffer, sizeof(buffer) ) )
	{
		assert( false );
		return false;
	}
	path = buffer;
	return true;
}

bool co::OS::isAbs( const std::string& path )
{
#if defined(CORAL_OS_WIN)
	return path.size() > 2 && path[1] == ':';
#else
	return !path.empty() && path[0] == '/';
#endif
}

bool co::OS::isFile( const std::string& path )
{
	CORAL_STAT_STRUCT info;
	if( CORAL_STAT_FUNC( path.c_str(), &info ) )
		return false;
	return S_ISREG( info.st_mode ) != 0;
}

bool co::OS::isDir( const std::string& path )
{
	CORAL_STAT_STRUCT info;
	if( CORAL_STAT_FUNC( path.c_str(), &info ) )
		return false;
	return S_ISDIR( info.st_mode ) != 0;
}

void co::OS::convertDotsToDirSeps( std::string& dotSeparatedPath )
{
	size_t len = dotSeparatedPath.length();
	for( size_t i = 0; i < len; ++i )
	{
		if( dotSeparatedPath[i] == '.' )
			dotSeparatedPath[i] = CORAL_OS_DIR_SEP;
	}
}

void co::OS::normalizePath( std::string& path )
{
	if( path.empty() )
		return;

#if defined(CORAL_OS_WIN)
	size_t len = path.length();
	for( size_t i = 0; i < len; ++i )
		if( path[i] == '/' )
			path[i] = CORAL_OS_DIR_SEP;
#endif

	// 'A//B', 'A/B/', 'A/./B' and 'A/foo/../B' all become 'A/B'
	co::StringTokenizer st( path, CORAL_OS_DIR_SEP_STR );
	path = ( path[0] == CORAL_OS_DIR_SEP ? CORAL_OS_DIR_SEP_STR : "" );
	while( st.nextToken() )
	{
		const std::string& tk = st.getToken();
		size_t len = tk.length();

		if( len == 0 || ( len == 1 && tk[0] == '.' ) )
			continue;

		if( len == 2 && tk[0] == '.' && tk[1] == '.' )
		{
			// remove the last token added to the path
			size_t pos = path.rfind( CORAL_OS_DIR_SEP );
			if( pos != std::string::npos )
				path.erase( pos );
			continue;
		}

		len = path.length();
		if( len != 0 && ( len > 1 || path[0] != CORAL_OS_DIR_SEP ) )
			path.push_back( CORAL_OS_DIR_SEP );
		path.append( tk );
	}		
}

void co::OS::makeAbs( std::string& path )
{
	if( !isAbs( path ) )
	{
		std::string tmp( path );
		getCurrentWorkingDir( path );
		path.reserve( path.size() + tmp.size() + 1 );
		path.push_back( CORAL_OS_DIR_SEP );
		path.append( tmp );
	}
	normalizePath( path );
}

bool co::OS::searchFile2( co::Slice<std::string> listA, co::Slice<std::string> listB,
						  std::string& path, std::string* partA, std::string* partB )
{
	std::string filePath;
	for( co::Slice<std::string> itA( listA ); itA; itA.popFirst() )
	{
		for( co::Slice<std::string> itB( listB ); itB; itB.popFirst() )
		{
			filePath.assign( itA.getFirst() );
			if( *filePath.rbegin() != CORAL_OS_DIR_SEP )
				filePath.push_back( CORAL_OS_DIR_SEP );
			filePath.append( itB.getFirst() );

			if( isFile( filePath ) )
			{
				path = filePath;

				if( partA ) *partA = itA.getFirst();
				if( partB ) *partB = itB.getFirst();

				return true;
			}
		}
	}
	return false;
}

bool co::OS::searchFile3( co::Slice<std::string> listA,
						  co::Slice<std::string> listB,
						  co::Slice<std::string> listC,
						  std::string& path, std::string* partA, std::string* partB, std::string* partC )
{
	std::string filePath;
	for( co::Slice<std::string> itC( listC ); itC; itC.popFirst() )
	{
		for( co::Slice<std::string> itB( listB ); itB; itB.popFirst() )
		{
			for( co::Slice<std::string> itA( listA ); itA; itA.popFirst() )
			{
				filePath.assign( itA.getFirst() );
				if( *filePath.rbegin() != CORAL_OS_DIR_SEP )
					filePath.push_back( CORAL_OS_DIR_SEP );
				filePath.append( itB.getFirst() );
				if( *filePath.rbegin() != CORAL_OS_DIR_SEP )
					filePath.push_back( CORAL_OS_DIR_SEP );
				filePath.append( itC.getFirst() );

				if( isFile( filePath ) )
				{
					path = filePath;

					if( partA ) *partA = itA.getFirst();
					if( partB ) *partB = itB.getFirst();
					if( partC ) *partC = itC.getFirst();

					return true;
				}
			}
		}
	}
	return false;
}
