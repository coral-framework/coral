#include "Utils.h"
#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/IObject.h>
#include <co/Exception.h>
#include <co/IReflector.h>
#include <co/IMethod.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>
#include <co/IParameter.h>
#include <co/IModuleManager.h>
#include <iostream>

/*
	Helper function to initialize the Coral path.
 */
static void addDefaultPaths()
{
	// if the CORAL_PATH environment variable is specified, add it to the path
	char* coralPathEnv = getenv( "CORAL_PATH" );
	if( coralPathEnv )
		co::addPath( coralPathEnv );

	// add the '~/modules' dir (relative to the executable's dir) to the path
	std::string path;
	if( co::OS::getApplicationDir( path ) )
	{
		path.append( "/modules" );
		if( co::OS::isDir( path ) )
			co::addPath( path );
	}
	else
	{
		std::cerr << "Oops! Could not determine the current executable's path.\n";
	}

	// add the current working dir to the path
	if( co::OS::getCurrentWorkingDir( path ) )
	{
		// if the CWD contains a './modules' subdir, add it instead
		std::string subDir( path );
		subDir.append( "/modules" );
		if( co::OS::isDir( subDir ) )
			co::addPath( subDir );
		else
			co::addPath( path );
	}
	else
	{
		std::cerr << "Oops! Could not determine the current working directory.\n";
	}

#if defined(CORAL_PATH)
	// if specified at compile time, add the contents of 'CORAL_PATH' to the path;
	co::addPath( CORAL_PATH );
#endif
}

static void resolveCallee( const std::string& calleeName, co::IComponent*& component, co::IPort*& facet, co::IMethod*& method )
{
	// was a method name specified?
	size_t colonPos = calleeName.rfind( ':' );
	if( colonPos == std::string::npos )
	{
		// resolve component type
		co::IType* type = co::getType( calleeName );
		if( !type || type->getKind() != co::TK_COMPONENT )
			CORAL_THROW( co::Exception, "'" << calleeName << "' is not a component." );

		component = static_cast<co::IComponent*>( type );

		// search the component's list of facets
		co::TSlice<co::IPort*> facets = component->getFacets();
		for( ; facets; facets.popFirst() )
		{
			facet = facets.getFirst();
			co::IMember* member = facet->getType()->getMember( "main" );
			method = static_cast<co::IMethod*>( member );
            if( member && member->getKind() == co::MemberKind::Method )
				return;
		}

		CORAL_THROW( co::Exception, "none of " << calleeName << "'s facets has a main() method." );
	}

	// parse the component.facet:method names

	std::string methodName( calleeName, colonPos + 1 );

	size_t facetDotPos = calleeName.rfind( '.', colonPos );
	if( facetDotPos == std::string::npos )
		CORAL_THROW( co::Exception, "methods must be specified within a component's facet "
						"(syntax: module.component.facet:method)." );

	std::string facetName( calleeName, facetDotPos + 1, colonPos - facetDotPos - 1 );
	std::string componentName( calleeName, 0, facetDotPos );

	// resolve component type
	co::IType* type = co::getType( componentName );
	if( !type || type->getKind() != co::TK_COMPONENT )
		CORAL_THROW( co::Exception, "'" << componentName << "' is not a component." );

	component = static_cast<co::IComponent*>( type );

	// resolve facet type
	co::IMember* member = component->getMember( facetName );
	if( !member )
		CORAL_THROW( co::Exception, "'" << componentName << "' has no facet named '" << facetName << "'." );

	facet = static_cast<co::IPort*>( member );
    if( member->getKind() != co::MemberKind::Port || !facet->getIsFacet() )
		CORAL_THROW( co::Exception, "component port '" << componentName << "." << facetName
						<< "' is a receptacle, not a facet." );

	// resolve method
	member = facet->getType()->getMember( methodName );
	if( !member )
		CORAL_THROW( co::Exception, "interface '" << facet->getType()->getFullName()
						<< "' has no method named '" << methodName << "'." );

    if( member->getKind() != co::MemberKind::Method )
		CORAL_THROW( co::Exception, "interface member '" << facet->getType()->getFullName()
						<< ":" << methodName << "' is a field, not a method." );

	method = static_cast<co::IMethod*>( member );
}

/*
	The main() function.
 */
int main( int argc, char* argv[] )
{
	int index = 1;

	// process command-line options
	bool abiChecks = true;
	while( index < argc && argv[index][0] == '-' )
	{
		if( argv[index][1] == 'p' && argv[index][2] == '\0' )
		{
			if( ++index >= argc )
			{
				std::cerr << "Error: expected DIR LIST after option '-p'.";
				return EXIT_FAILURE;
			}
			co::addPath( argv[index] );
		}
		else if( strCaseComp( argv[index] + 1, "-csl" ) == 0 )
		{
			if( ++index >= argc )
			{
				std::cerr << "Error: expected FLAGS after option '--csl'.";
				return EXIT_FAILURE;
			}
			co::int8 flags = 0;
			const char* args = argv[index];
			while( char c = *args++ )
			{
				switch( c )
				{
				case 'a':	flags |= co::CSL_ANNOTATIONS;	break;
				case 'c':	flags |= co::CSL_CPPBLOCKS;		break;
				case 'd':	flags |= co::CSL_DOCUMENTATION;	break;
				default:
					std::cerr << "Error: unrecognized CSL flag '" << c << "'.";
					return EXIT_FAILURE;
				}
			}
			co::setCSLFlags( flags );
		}
		else if( strCaseComp( argv[index] + 1, "-no-abi-checks" ) == 0 )
		{
			abiChecks = false;
		}
		else
		{
			std::cerr << "Error: unrecognized option '" << argv[index] << "'.";
			return EXIT_FAILURE;
		}
		++index;
	}

	// print help if not enough args were passed
	if( index >= argc )
	{
		std::cout <<
			"Coral Application Launcher v" CORAL_VERSION_STR " (" CORAL_BUILD_KEY " " CORAL_BUILD_MODE ")\n"
			"Usage: coral [options] callee [ARG] ...\n"
			"Description:\n"
			"  A 'callee' is either a component or a method name within a component facet.\n"
			"  If a method is not specified, one named 'main' will be searched for in all\n"
			"  facets of the specified component. The called method must receive either no\n"
			"  argument or an array of strings. If the method returns a number, it will be\n"
			"  used as the application's return status.\n"
			"Examples:\n"
			"  coral myModule.MyComponent arg1 arg2 arg3\n"
			"  coral someModule.SomeComponent.someFacet:someMethod arg1 arg2\n"
			"Available Options:\n"
			"  -p EXTRA,DIRS  Add a list of repositories to the Coral path.\n"
			"  --csl FLAGS    Controls optional language features.\n"
			"                 Flags:  (sample usage: --csl acd)\n"
			"                   a - Enable annotations (on by default).\n"
			"                   c - Load C++ code blocks (off by default).\n"
			"                   d - Load documentation (off by default).\n"
			"  --no-checks    Disable module compatibility checks.\n"
		;
		return EXIT_FAILURE;
	}

	addDefaultPaths();

	int exitStatus = EXIT_SUCCESS;
	try
	{
		// initialize the Coral framework
		co::ISystem* system;
		try
		{
			system = co::getSystem();
			system->getModules()->setCompatibilityChecking( abiChecks );
            system->setup({});
		}
		catch( std::exception& e )
		{
			std::cerr << "System initialization failed.\nException: " << e.what();
			throw;
		}

		// resolve the callee
		co::IComponent* ct = NULL;
		co::IPort* facet = NULL;
		co::IMethod* method = NULL;
		try
		{
			resolveCallee( argv[index], ct, facet, method );
			assert( ct && facet && method );
		}
		catch( std::exception& e )
		{
			std::cerr << "Invalid callee '" << argv[index] << "'.\nException: " << e.what();
			throw;
		}

		// check the method signature
		try
		{
			co::TSlice<co::IParameter*> params = method->getParameters();
			if( !params.isEmpty() && ( params.getSize() > 1 || params[0]->getType()->getFullName() != "string[]" ) )
				throw co::Exception( "method can only have a single parameter, of type 'string[]'" );
		}
		catch( std::exception& e )
		{
			std::cerr << "Invalid method '" << facet->getType()->getFullName()
					  << ':' << method->getName() << "'.\nException: " << e.what();
			throw;
		}

		// instantiate the component & obtain the service
		co::RefPtr<co::IObject> object;
		co::IService* service;
		try
		{
			object = ct->getReflector()->newInstance();
			assert( object.isValid() );

			service = object->getServiceAt( facet );
			assert( service );
		}
		catch( std::exception& e )
		{
			std::cerr << "Instantiation failed: " << e.what();
			throw;
		}

		// prepare to invoke the method
		co::IReflector* reflector;
		try
		{
			reflector = method->getOwner()->getReflector();
		}
		catch( std::exception& e )
		{
			std::cerr << "Invocation failed: " << e.what();
			throw;
		}

		// invoke the method
		try
		{
			std::vector<std::string> args;
			while( ++index < argc )
				args.push_back( argv[index] );

			co::AnyValue res;
			co::Any arg( args );
			reflector->invoke( service, method, co::Slice<co::Any>( &arg, 1 ), res );

			// if the result is a number, use it as the return status; otherwise, print it
			if( res.isValid() )
			{
				if( co::isNumber( res.getType()->getKind() ) )
					exitStatus = res.get<int>();
				else
					std::cout << "Method returned " << res;
			}
		}
		catch( std::exception& e )
		{
			std::cerr << "Program terminated with an unhandled exception.\nMessage: " << e.what();
			throw;
		}
	}
	catch( std::exception& )
	{
		exitStatus = EXIT_FAILURE;
	}

	co::shutdown();

	return exitStatus;
}
