#include "Utils.h"
#include <core/utils/Properties.h>
#include <core/utils/StringTokenizer.h>

#if defined(CORAL_OS_WINDOWS)
#define EXE_SUFFIX ".exe"
#else
#define EXE_SUFFIX
#endif

enum Mode { Mode_Auto, Mode_Debug, Mode_Release };

Mode resolveMode(const char* str) {
  return strCaseComp(str, "debug") == 0 ? Mode_Debug : Mode_Release;
}

int main(int argc, char* argv[]) {
  // set CORAL_ROOT = this executable's dir
  std::string rootDir;
  co::OS::getApplicationDir(rootDir);
  setEnvVar("CORAL_ROOT", rootDir);

  co::Properties properties;
  properties.load(rootDir + (CORAL_OS_DIR_SEP_STR "coral.properties"));

  // if dir "$CORAL_ROOT/modules" exists, add it to the CORAL_PATH
  std::string coralPath(rootDir);
  coralPath.append(CORAL_OS_DIR_SEP_STR "modules");
  if (!co::OS::isDir(coralPath)) coralPath.clear();

  // if property "path" is defined, add it to the CORAL_PATH
  const std::string& pathProperty = properties.getProperty("path");
  if (!pathProperty.empty()) {
    coralPath.reserve(coralPath.size() + pathProperty.size() + 2);
    coralPath.append(CORAL_OS_PATH_SEP_STR);
    coralPath.append(pathProperty);
  }

  coralPath.append(CORAL_OS_PATH_SEP_STR);

  // add entries to the CORAL_PATH environment variable
  prependToEnvVar("CORAL_PATH", coralPath);

  // set the OS-specific var that enables us to locate the coral library
  std::string libDir =
      properties.getProperty("libdir", rootDir + CORAL_OS_DIR_SEP_STR "lib");
  libDir.append(CORAL_OS_PATH_SEP_STR);
#if defined(CORAL_OS_WINDOWS)
  prependToEnvVar("PATH", libDir);
#elif defined(CORAL_OS_LINUX)
  prependToEnvVar("LD_LIBRARY_PATH", libDir);
#elif defined(CORAL_OS_DARWIN)
  prependToEnvVar("DYLD_LIBRARY_PATH", libDir);
#else
#error Unknown or unsupported platform.
#endif

// determine in which mode to run the launcher
#ifdef CORAL_NDEBUG
  Mode mode = Mode_Auto;  // default is auto in release build
#else
  Mode mode = Mode_Debug;  // default is debug in debug builds
#endif

  const std::string& modeProperty = properties.getProperty("mode");
  if (!modeProperty.empty()) mode = resolveMode(modeProperty.c_str());

  /*
    Process command-line options. Currently, only one option is supported,
    and if specified, it MUST be at the start of the command-line:

    --mode <string>
      Indicates in which mode to run the Coral Application Launcher.
      Any string other than 'debug' is interpreted as 'release'.
      If this option is specified and no launcher is available under the
    requested
      mode, an error is issued. However, if this option is omitted, the
    front-end
      will use any available launcher, giving preference to one in release mode.
   */
  int argIndex = 1;
  if (argc >= 2 && strCaseComp(argv[1], "--mode") == 0) {
    argIndex = 3;
    mode = resolveMode(argv[2]);
  }

  // determine the launcher executable that should be used
  std::string launcher = properties.getProperty("launcher");
  if (launcher.empty()) {
    std::string dirs[] = {
        properties.getProperty("bindir", rootDir + CORAL_OS_DIR_SEP_STR "bin"),
        rootDir};

    std::string names[] = {
        properties.getProperty("launcher", "launcher" EXE_SUFFIX),
        properties.getProperty("launcher_debug", "launcher_debug" EXE_SUFFIX)};

    co::Slice<std::string> selectedNames;
    switch (mode) {
      case Mode_Auto:
        selectedNames = names;
        break;
      case Mode_Debug:
        selectedNames = co::Slice<std::string>(names + 1, 1);
        break;
      case Mode_Release:
        selectedNames = co::Slice<std::string>(names, 1);
        break;
      default:
        assert(false);
    }

    co::OS::searchFile2(dirs, selectedNames, launcher);
  }

  if (!co::OS::isFile(launcher)) {
    fprintf(stderr, "ERROR: the launcher executable is not available (%s)\n",
            launcher.c_str());
    return -1;
  }

  /*
    Prepare the command-line to invoke the Coral Application Launcher.
    first arg = program path; last arg = NULL;
   */
  std::vector<char*> args;
  args.reserve(argc);
  args.push_back(const_cast<char*>(launcher.c_str()));

  // if property "args" was defined, prepend its arguments first
  std::vector<std::string> prependedArgs;
  const std::string& argsProperty = properties.getProperty("args");
  if (!argsProperty.empty()) {
    std::string token;
    co::StringTokenizer st(argsProperty, ";");
    while (st.getNext(token)) prependedArgs.emplace_back(token);
  }

  // append all args in prependedArgs
  for (size_t i = 0; i < prependedArgs.size(); ++i)
    args.push_back(const_cast<char*>(prependedArgs[i].c_str()));

  // append all unprocessed command-line args
  for (; argIndex < argc; ++argIndex) args.push_back(argv[argIndex]);

  args.push_back(NULL);

  return executeProgram(&args[0]);
}
