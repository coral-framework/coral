#include "Log.h"
#include <cstdlib>
#include <iostream>

namespace co {

static void defaultLogHandler( const Log& log )
{
	static const char* s_levelName[] = { "INFO", "WARNING", "ERROR", "FATAL" };

	LogLevel level = log.level;
	if( level < LOG_INFO || level > LOG_FATAL )
		level = LOG_FATAL;

	const char* levelName = s_levelName[level];
	const char* debugPrefix = log.isDebug ? "D" : "";

	std::ostream& os = ( level == LOG_INFO ? std::cout : std::cerr );
	os << '[' << debugPrefix << levelName << "] " << log.message << ::std::endl;

	if( level == LOG_FATAL )
		abort();
}

static LogHandler sg_logHandler( defaultLogHandler );

LogHandler setLogHandler( LogHandler handler )
{
	LogHandler previous = sg_logHandler;
	sg_logHandler = handler ? handler : defaultLogHandler;
	return previous;
}

LogLevel Logger::sm_minSeverity( LOG_INFO );

void Logger::setMinSeverity( LogLevel level )
{
	sm_minSeverity = level;
}

Logger::~Logger()
{
	Log log;
	log.level = _level;
	log.isDebug = _isDebug;
	_os.str().swap( log.message );
	sg_logHandler( log );
}

} // namespace co
