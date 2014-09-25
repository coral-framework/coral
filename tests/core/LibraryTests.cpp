#include <core/utils/Library.h>
#include <co/ModuleLoadException.h>
#include <gtest/gtest.h>

class TestData : public co::RefCounted
{
public:
	std::string libcPath;
	std::string libOnePath;
	std::string libTwoPath;
	std::string invalidLibPath;
	std::vector<std::string> libOneValidSymbols;

	TestData()
	{
#if defined(CORAL_OS_WINDOWS)

		libcPath = ""; // the libc test is not performed on Windows

		const char* winDir = getenv( "windir" );
		assert( winDir != NULL );

		libOnePath = winDir;
		libOnePath.append( "\\system32\\user32.dll" );

		libTwoPath = winDir;
		libTwoPath.append( "\\system32\\shell32.dll" );

		invalidLibPath = TESTS_DATA_DIR "/invalidLibrary.dll";

		libOneValidSymbols.push_back( "MessageBoxA" );
		libOneValidSymbols.push_back( "CloseWindow" );
		libOneValidSymbols.push_back( "CreateIcon" );
		libOneValidSymbols.push_back( "CreateMenu" );

#else // UNIX

		libcPath = LIBC_PATH;
		libOnePath = LIBPTHREAD_PATH;
		libTwoPath = libcPath;
		invalidLibPath = TESTS_DATA_DIR "/invalidLibrary.so";
printf( "LIB1 = %s\nLIB2 = %s\n", libOnePath.c_str(), libTwoPath.c_str() ); 

		libOneValidSymbols.push_back( "pthread_mutex_init" );
		libOneValidSymbols.push_back( "pthread_mutex_destroy" );
		libOneValidSymbols.push_back( "pthread_cond_wait" );
		libOneValidSymbols.push_back( "pthread_equal" );
#endif
	}
};

static const TestData& getData()
{
	static co::RefPtr<TestData> s_data;
	if( !s_data )
		s_data = new TestData;
	return *s_data.get();
}

TEST( LibraryTests, initialization )
{
	co::Library lib( "foo.lib" );
	EXPECT_EQ( "foo.lib", lib.getFileName() );
	EXPECT_FALSE( lib.isLoaded() );
	EXPECT_EQ( 0, lib.getLoadHints() );
}

TEST( LibraryTests, simpleLoading )
{
	// load and unload lib one
	co::Library libOne( getData().libOnePath );
	EXPECT_FALSE( libOne.isLoaded() );
	EXPECT_NO_THROW( libOne.load() );
	EXPECT_TRUE( libOne.isLoaded() );
	EXPECT_NO_THROW( libOne.unload() );
	EXPECT_FALSE( libOne.isLoaded() );

	// redundant load() and unload() calls on lib two
	co::Library libTwo( getData().libTwoPath );
	EXPECT_FALSE( libTwo.isLoaded() );
	EXPECT_NO_THROW( libTwo.load() );
	EXPECT_TRUE( libTwo.isLoaded() );
	EXPECT_NO_THROW( libTwo.load() );
	EXPECT_TRUE( libTwo.isLoaded() );
	EXPECT_NO_THROW( libTwo.unload() );
	EXPECT_FALSE( libTwo.isLoaded() );
	EXPECT_NO_THROW( libTwo.unload() );
	EXPECT_FALSE( libTwo.isLoaded() );

	// try to load an invalid dynamic library
	co::Library invalidLib( getData().invalidLibPath );
	EXPECT_THROW( invalidLib.load(), co::ModuleLoadException );
	EXPECT_FALSE( invalidLib.isLoaded() );
	EXPECT_NO_THROW( invalidLib.unload() );
	EXPECT_FALSE( invalidLib.isLoaded() );
	EXPECT_THROW( invalidLib.resolve( "foo" ), co::ModuleLoadException );
}

TEST( LibraryTests, redundantLoads )
{
	co::Library libA( getData().libOnePath );
	co::Library libB( getData().libOnePath );

	EXPECT_FALSE( libA.isLoaded() );
	EXPECT_FALSE( libB.isLoaded() );

	EXPECT_NO_THROW( libA.load() );
	EXPECT_NO_THROW( libB.load() );

	EXPECT_TRUE( libA.isLoaded() );
	EXPECT_TRUE( libB.isLoaded() );

	EXPECT_EQ( libA.getHandle(), libB.getHandle() );

	EXPECT_NO_THROW( libA.unload() );
	EXPECT_NO_THROW( libB.unload() );

	EXPECT_FALSE( libA.isLoaded() );
	EXPECT_FALSE( libB.isLoaded() );
}

TEST( LibraryTests, resolveSymbols )
{
	co::Library libOne( getData().libOnePath );
	EXPECT_EQ( NULL, libOne.resolve( "invalidSymbol" ) );

	const std::vector<std::string>& libOneValidSymbols = getData().libOneValidSymbols;
	for( unsigned int i = 0; i < libOneValidSymbols.size(); ++i )
		EXPECT_TRUE( libOne.resolve( libOneValidSymbols[i].c_str() ) != NULL );

	// unload() doesn't forbid calls to resolve(), which re-load the library
	EXPECT_NO_THROW( libOne.unload() );
	EXPECT_FALSE( libOne.isLoaded() );
	EXPECT_TRUE( libOne.resolve( libOneValidSymbols[0].c_str() ) != NULL );
	EXPECT_TRUE( libOne.isLoaded() );
}

/****************************************************************************/
/* UNIX-specific Tests                                                      */
/****************************************************************************/

#if defined(CORAL_OS_UNIX)

TEST( LibraryTests, printf )
{
	co::Library libc( getData().libcPath );

	typedef void (*LoadedPrintf)( const char *, ... );
	LoadedPrintf loadedPrintf = (LoadedPrintf)libc.resolve( "printf" );

	ASSERT_TRUE( loadedPrintf != NULL );
	loadedPrintf( "Successfully loaded 'printf' from the libc dynamic library.\n" );
}

TEST( LibraryTests, lazyBindFunctionsHint )
{
	co::Library pThreadLib( getData().libOnePath );
	pThreadLib.setLoadHints( co::Library::LazyBindFunctionsHint );

	EXPECT_TRUE( pThreadLib.resolve( "pthread_mutex_lock" ) != NULL );
}

TEST( LibraryTests, globalSymbolsHint )
{
	co::Library pThreadLib( getData().libOnePath );
	pThreadLib.setLoadHints( co::Library::GlobalSymbolsHint );

	EXPECT_TRUE( pThreadLib.resolve( "pthread_mutex_unlock" ) != NULL );
}

#endif // defined(CORAL_OS_UNIX)
