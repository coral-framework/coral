#include "Library.h"
#include <co/ModuleLoadException.h>
#include <sstream>

#if defined(CORAL_OS_UNIX)
	#include <dlfcn.h>
#else
	#include <windows.h>
#endif

namespace co {

Library::Library( const std::string& fileName )
	: _fileName( fileName ), _handle( 0 ), _loadHints( 0 ), _noDlClose( false )
{
	// empty
}

Library::~Library()
{
	unload();
}

void Library::load()
{
	if( isLoaded() )
		return;

	doLoad();
}

void* Library::resolve( const char* symbolName )
{
	load();
	return doResolve( symbolName );
}

void Library::unload()
{
	if( isLoaded() )
		doUnload();
}

/****************************************************************************/
/* Load                                                                     */
/****************************************************************************/

#ifdef CORAL_OS_WINDOWS

// Windows implementation
void Library::doLoad()
{
	// avoid 'Bad Image' message boxes
	UINT oldMode = SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

	_handle = LoadLibraryA( _fileName.c_str() );

	SetErrorMode( oldMode );

	assertIsLoaded();
}

#else

// UNIX implementation
void Library::doLoad()
{
	int dlFlags = 0;

	if( _loadHints & LazyBindFunctionsHint )
	{
		// each external function reference is bound the first time the function is called.
		dlFlags |= RTLD_LAZY;
	}
	else
	{
		// all external function references are bound immediately during the call to dlopen().
		dlFlags |= RTLD_NOW;
	}

	if( _loadHints & GlobalSymbolsHint )
	{
		// all symbols are available for relocation processing of other modules.
		dlFlags |= RTLD_GLOBAL;
	}
	else
	{
		// all symbols are not made available for relocation processing by other modules.
		dlFlags |= RTLD_LOCAL;
	}

	_handle = dlopen( _fileName.c_str(), dlFlags );

	assertIsLoaded();
}

#endif // UNIX

/****************************************************************************/
/* Resolve                                                                  */
/****************************************************************************/

void* Library::doResolve( const char* symbolName )
{
	load();

#ifdef CORAL_OS_WINDOWS
	void* address = (void*)GetProcAddress( static_cast<HINSTANCE>( _handle ), symbolName );
#else
	void* address = dlsym( _handle, symbolName );
#endif

	return address;
}

/****************************************************************************/
/* Unload                                                                   */
/****************************************************************************/

void Library::doUnload()
{
#ifdef CORAL_OS_WINDOWS
	bool unloaded = ( FreeLibrary( static_cast<HINSTANCE>( _handle ) ) != FALSE );
#else
	bool unloaded = ( _noDlClose || ( dlclose( _handle ) == 0 ) );
#endif

	if( !unloaded )
	{
		std::string errorMsg;
		getLastErrorMessage( errorMsg );
		CORAL_THROW( ModuleLoadException, "error unloading library '" << _fileName << "': " << errorMsg );
	}

	_handle = 0;
}

/****************************************************************************/
/* Misc Platform-Specific Methods                                           */
/****************************************************************************/

void Library::getLastErrorMessage( std::string& message )
{
#ifdef CORAL_OS_WINDOWS
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL
	);
	message = (char*)lpMsgBuf;
	LocalFree( lpMsgBuf );
#else
	const char* errorMsg = dlerror();
	message = ( errorMsg == NULL ? "" : errorMsg );
#endif
}

void Library::assertIsLoaded()
{
	if( !isLoaded() )
	{
		std::string errorMsg;
		getLastErrorMessage( errorMsg );
		CORAL_THROW( ModuleLoadException, "error loading library '" << _fileName << "': " << errorMsg );
	}
}

} // namespace co
