#include "Utils.h"

#ifdef CORAL_NDEBUG
	#define CORAL_DEBUG_SUFFIX
#else
	#define CORAL_DEBUG_SUFFIX "_debug"
#endif

int main( int argc, char* argv[] )
{
	// resolve the CORAL_ROOT
	std::string rootDir;
	co::OS::getApplicationDir( rootDir );

	// call $CORAL_ROOT/coral --csl acd co.compiler.Compile $@
	std::string program( rootDir );
	program += CORAL_OS_DIR_SEP_STR "coral" CORAL_DEBUG_SUFFIX;

	char** args = new char*[argc + 3 + 1];

	int k = 0;
	args[k++] = const_cast<char*>( program.c_str() );
	args[k++] = const_cast<char*>( "--csl" );
	args[k++] = const_cast<char*>( "acd" );
	args[k++] = const_cast<char*>( "co.compiler.Compile" );
	for( int i = 1; i < argc; ++i )
		args[k++] = argv[i];
	args[k] = NULL;

	int res = executeProgram( args );

	delete[] args;

	return res;
}
