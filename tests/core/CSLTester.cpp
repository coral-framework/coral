#include "CSLTester.h"
#include <co/ISystem.h>
#include <co/Exception.h>
#include <co/ITypeManager.h>
#include <co/types/TypeLoader.h>
#include <co/types/csl/Error.h>
#include <gtest/gtest.h>
#include <sstream>

CSLTester::CSLTester(const std::string& typeName, const LocationInfo& li)
    : _typeName(typeName), _location(li) {
  // empty
}

CSLTester& CSLTester::expectedError(const char* message, const char* filename,
                                    int line) {
  _expectedErrors.push_back(ExpectedError(message, filename, line));
  return *this;
}

#define CSL_ADD_ERROR(message)                                             \
  ::testing::internal::AssertHelper(                                       \
      ::testing::TestPartResult::kNonFatalFailure, _location.file.c_str(), \
      _location.line, heading.c_str()) = message

#define CSL_FAIL() CSL_ADD_ERROR(::testing::Message())

co::IType* CSLTester::run() {
  co::ITypeManager* tm = co::getSystem()->getTypes();
  co::IType* res = tm->findType(_typeName);
  if (res) return res;

  co::TypeLoader loader(_typeName, tm);

  std::stringstream headingSS;
  headingSS << "CSL test failed for type '" << _typeName << "':";
  std::string heading(headingSS.str());

  try {
    res = loader.loadType();
  } catch (co::Exception& e) {
    CSL_FAIL() << "Loader raised " << e.getTypeName() << ": " << e.getMessage();
    return res;
  }

  const co::csl::Error* error = loader.getError();

  if (_expectedErrors.empty()) {
    if (error)
      CSL_FAIL()
          << "Got an error, though no error was expected. CSL Error Stack:\n"
          << *error;
    return res;
  }

  if (!error) {
    CSL_FAIL() << "Type was successfully loaded, though "
               << _expectedErrors.size()
               << (_expectedErrors.size() > 1 ? " errors were" : " error was")
               << " expected.";
    return res;
  }

#define CSL_FAIL_MSG() *(errMsg ? errMsg : (errMsg = new ::testing::Message()))
#define CSL_FAIL_AT() CSL_FAIL_MSG() << "[error#" << i << "] "
  ::testing::Message* errMsg = NULL;

  // check the error hierarchy
  size_t i = 0;
  while (error) {
    if (i < _expectedErrors.size()) {
      const ExpectedError& ee = _expectedErrors[i];

      // compare the error file
      if (!TestHelper::stringEndsWith(error->getFileName(), ee.filename))
        CSL_FAIL_AT() << "filename mismatch ('" << ee.filename
                      << "' expected, got '" << error->getFileName() << "').\n";

      // compare the error line
      if (error->getLine() != ee.line)
        CSL_FAIL_AT() << "line number mismatch (" << ee.line
                      << " expected, got " << error->getLine() << ").\n";

      // check the error message, if an expected substring was provided
      if (ee.message && *ee.message) {
        size_t pos = error->getMessage().find(ee.message);
        if (pos == std::string::npos)
          CSL_FAIL_AT() << "message mismatch ('" << ee.message
                        << "' expected, got '" << error->getMessage()
                        << "').\n";
      }
    } else {
      CSL_FAIL_AT() << "unexpected error: " << error->getFileName() << ":"
                    << error->getLine() << ": " << error->getMessage() << "\n";
    }

    error = error->getInnerError();
    ++i;
  }

  if (i != _expectedErrors.size())
    CSL_FAIL_MSG() << "Error count mismatch (" << _expectedErrors.size()
                   << " expected, got " << i << ").";

  if (errMsg) {
    CSL_ADD_ERROR(*errMsg);
    delete errMsg;
  }

  return res;
}
