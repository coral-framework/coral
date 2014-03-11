#include <co/Log.h>
#include <gtest/gtest.h>

static int sg_logCount( 0 );
static co::Log sg_lastLog;

static void myLogHandler( const co::Log& log )
{
	++sg_logCount;
	sg_lastLog = log;
}

TEST( LogTests, customEventHandler )
{
	co::LogHandler originalHandler = co::setLogHandler( myLogHandler );

	EXPECT_EQ( 0, sg_logCount );

	CORAL_LOG(INFO) << "some info";
	EXPECT_EQ( 1, sg_logCount );
	EXPECT_EQ( "some info", sg_lastLog.message );
	EXPECT_EQ( co::LOG_INFO, sg_lastLog.level );
	EXPECT_FALSE( sg_lastLog.isDebug );

	CORAL_LOG(WARNING) << "some warning";
	EXPECT_EQ( 2, sg_logCount );
	EXPECT_EQ( "some warning", sg_lastLog.message );
	EXPECT_EQ( co::LOG_WARNING, sg_lastLog.level );
	EXPECT_FALSE( sg_lastLog.isDebug );

	CORAL_LOG(ERROR) << "some error";
	EXPECT_EQ( 3, sg_logCount );
	EXPECT_EQ( "some error", sg_lastLog.message );
	EXPECT_EQ( co::LOG_ERROR, sg_lastLog.level );
	EXPECT_FALSE( sg_lastLog.isDebug );

	CORAL_LOG(FATAL) << "fatal error";
	EXPECT_EQ( 4, sg_logCount );
	EXPECT_EQ( "fatal error", sg_lastLog.message );
	EXPECT_EQ( co::LOG_FATAL, sg_lastLog.level );
	EXPECT_FALSE( sg_lastLog.isDebug );

	CORAL_DLOG(WARNING) << "test debug warning";

#ifdef CORAL_NDEBUG
	EXPECT_EQ( 4, sg_logCount );	
#else
	EXPECT_EQ( 5, sg_logCount );
	EXPECT_EQ( "test debug warning", sg_lastLog.message );
	EXPECT_EQ( co::LOG_WARNING, sg_lastLog.level );
	EXPECT_TRUE( sg_lastLog.isDebug );	
#endif

	co::setLogHandler( originalHandler );
}
