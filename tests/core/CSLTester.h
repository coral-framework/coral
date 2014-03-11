#ifndef _CSLTESTER_H_
#define _CSLTESTER_H_

#include "TestHelper.h"

/*
	Utility class for testing errors in CSL files (integrated with GTest).
	It works with a stack of expected errors that should occur when loading a type.
	If the stack of expected errors is empty, then the type should load successfully (no errors).
 */
class CSLTester
{
public:
	struct LocationInfo
	{
		std::string file;
		int line;

		LocationInfo( const std::string file, int line )
			: file( file ), line( line ) {;}
	};

public:
	CSLTester( const std::string& typeName, const LocationInfo& li );

	/*
		Adds an expected error message and location (either can be omitted).
		Expected errors should be added from outer-most to inner-most.
		Note that semantic errors don't have a location.
	 */
	CSLTester& expectedError( const char* message, const char* filename = NULL, int line = -1 );

	// Runs the test by loading the specified type and checking the expected errors.
	co::IType* run();

private:
	std::string _typeName;
	LocationInfo _location;

	struct ExpectedError
	{
		const char* message;
		const char* filename;
		int line;

		ExpectedError( const char* message, const char* filename, int line )
			: message( message ), filename( filename ), line( line ) {;}
	};

	std::vector<ExpectedError> _expectedErrors;
};

// Macros to test CSL files that are expected to contain errors:
#define CSL_TEST_BEGIN( typeName ) CSLTester( typeName, CSLTester::LocationInfo( __FILE__, __LINE__ ) )
#define CSL_EXPECT_ERROR( message, file, line ) .expectedError( message, file, line )
#define CSL_EXPECT_SEMANTIC_ERROR( message ) .expectedError( message, "", -1 )
#define CSL_TEST_END() .run();

// Macro to test CSL files that are supposed to load successfully (no errors).
#define CSL_TEST( typeName ) CSL_TEST_BEGIN( typeName ).run()

#endif
