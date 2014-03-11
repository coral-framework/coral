#include "File_Adapter.h"

bool moduleA::File_Adapter::getIsOpen( moduleA::File& file )
{
	return file != NULL;
}

co::uint32 moduleA::File_Adapter::getPos( moduleA::File& file )
{
	return static_cast<co::uint32>( ftell( file ) );
}

void moduleA::File_Adapter::setPos( moduleA::File& file, co::uint32 pos )
{
	fseek( file, pos, SEEK_SET );
}

void moduleA::File_Adapter::open( moduleA::File& file, const std::string& filename, const std::string& mode )
{
	if( file )
		fclose( file );
	file = fopen( filename.c_str(), mode.c_str() );
}

void moduleA::File_Adapter::close( moduleA::File& file )
{
	if( file )
		fclose( file );
	file = NULL;
}

void moduleA::File_Adapter::write( moduleA::File& file, const std::string& line )
{
	fputs( line.c_str(), file );
	fputc( '\n', file );
}

void moduleA::File_Adapter::read( moduleA::File& file, std::string& line )
{
	char buffer[128];
	line = fgets( buffer, sizeof(buffer), file );
}
