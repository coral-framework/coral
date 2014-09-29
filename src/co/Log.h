#ifndef _CO_LOG_H_
#define _CO_LOG_H_

#include <co/Platform.h>
#include <sstream>

namespace co {

/*!
	\brief Log severity levels, in increasing order of severity.

	\ingroup log
 */
enum LogLevel
{
	LOG_INFO,	 //!< A harmless observation.
	LOG_WARNING, //!< Something worth checking.
	LOG_ERROR,	 //!< Potential data loss and malfunction.
	LOG_FATAL	 //!< Default action is to abort the application.
};

/*!
	\brief A log record.

	\ingroup log
 */
struct Log
{
	std::string message;	//!< Log message.
	LogLevel level;			//!< Log severity level.
	bool isDebug;			//!< Whether this is for \e debugging.
};

/*!
	\brief Signature of a LogHandler function. See setLogHandler().

	\ingroup log
 */
typedef void (*LogHandler)( const Log& log );

/*!
	\brief Sets the log handler function and returns the previous handler.

	A log handler is the function that receives all logs. The default handler
	prints all logs to \c stderr and aborts the application in case of a fatal
	error. Only a single handler can be in effect, and it should generally be
	set before the framework is initialized.

	To restore the default handler, call <tt>setLogHandler( NULL )</tt>.

	\ingroup log
 */
CORAL_EXPORT LogHandler setLogHandler( LogHandler handler );

/*!
	\brief Class responsible for recording and dispatching log records.
 
	This class is not meant to be used directly. You should use the macros
 	CORAL_LOG( level ) or CORAL_DLOG( level ) instead.

	\ingroup log
 */
class CORAL_EXPORT Logger
{
public:
	//! Returns the minimum severity level needed for a log to be recorded.
	inline static LogLevel getMinSeverity() { return sm_minSeverity; }

	//! Sets the minimum severity level needed for a log to be recorded.
	static void setMinSeverity( LogLevel level );

public:
	inline Logger() {;}
	~Logger();

	// Starts recording a log message.
	inline std::ostringstream& start( LogLevel level = LOG_INFO, bool isDebug = false )
	{
		_isDebug = isDebug;
		_level = level;
		return _os;
	}

private:
	// forbid copies
	Logger( const Logger& );
	Logger& operator=( const Logger& );

	static LogLevel sm_minSeverity;

private:
	bool _isDebug;
	LogLevel _level;
	std::ostringstream _os;
};

} // namespace co

/*!
	\def CORAL_LOG_MIN_SEVERITY
	\brief Logs below this severity level are disabled at compile time.
	Defaults to LOG_INFO.

	\ingroup log
 */
#ifndef CORAL_LOG_MIN_SEVERITY
#define CORAL_LOG_MIN_SEVERITY co::LOG_INFO
#endif

#define CORAL_LOG_LEVEL_DEBUG( level, debug ) \
	if( level < CORAL_LOG_MIN_SEVERITY ) ; \
	else if( level < co::Logger::getMinSeverity() ) ; \
	else co::Logger().start( level, debug )

/*!
	\def CORAL_LOG( level )
	\brief Records a log with the specified severity level.
	The macro returns an output stream for recording the log message.

	\sa setLogHandler()
	\ingroup log
 */
#define CORAL_LOG( level ) CORAL_LOG_LEVEL_DEBUG( co::LOG_##level, false )

/*!
	\def CORAL_DLOG( level )
	\brief Just like CORAL_LOG(), but meant for debugging.
	This kind of log is removed from release builds.

	\ingroup log
 */
#ifdef CORAL_NDEBUG
	#define CORAL_DLOG( level ) if( false ) co::Logger().start()
#else
	#define CORAL_DLOG( level ) CORAL_LOG_LEVEL_DEBUG( co::LOG_##level, true )
#endif

#endif // _CO_LOG_H_
