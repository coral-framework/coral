/* Scanner for the Coral Specification Language (CSL) */
%top{
#include "parser.hh"

#define YYSTYPE co::csl::Parser::semantic_type

#define YY_NO_INPUT

#ifndef YY_NO_UNISTD_H
#define YY_NO_UNISTD_H 1
#endif
}
%{

#include "Loader.h"
#include <co/Exception.h>
#include <cmath>
#include <cstdio>
#include <iomanip>

#define yyterminate() return TK::END
#define YY_USER_ACTION	yylloc->columns( yyleng );

#define SET_BOOL( arg )		(yylval)->b = arg;
#define SET_NUMBER( arg )	(yylval)->num = arg;
#define SET_CSTR( arg )		(yylval)->cstr = arg;
#define SET_STRING( arg )	(yylval)->str = loader.newString( arg );
#define STR_NEW()			(yylval)->str = str = loader.newString();
#define STR_APPEND( aStr )	{ assert( str ); str->append( aStr ); }
#define STR_PUSH( aChr )	{ assert( str ); str->push_back( aChr ); }

#define LEX_ERROR( msg )	{ PUSH_ERROR( *yylloc, msg ); yyterminate(); }

typedef co::csl::Parser::token TK;

%}

%option	8bit reentrant ecs nometa-ecs bison-bridge bison-locations
%option	warn nodefault nounput noyylineno noyywrap never-interactive
%option	prefix="csl_" outfile="scanner.cc" header-file="scanner.hh"

/* %option debug */

%x CMT CPP STR

BLANK		[ \t]
ID			[a-zA-Z_][a-zA-Z0-9_]*
QUALID		{ID}(\.{ID})+
DIGIT		[0-9]
HEXDIGIT	[0-9a-fA-F]
EXP			[Ee][+-]?{DIGIT}+

%%

%{
	// mark the current location as the start of the next token
	yylloc_param->step();

	int caller = 0;
	std::string* str = NULL;
%}

component			return TK::COMPONENT;
enum				return TK::ENUM;
exception			return TK::EXCEPTION;
extends				return TK::EXTENDS;
false				SET_BOOL( false ) return TK::BOOLEAN;
import				return TK::IMPORT;
in					return TK::IN;
inout				return TK::INOUT;
interface			return TK::INTERFACE;
native{BLANK}+class	return TK::NATIVECLASS;
out					return TK::OUT;
provides			return TK::PROVIDES;
raises				return TK::RAISES;
readonly			return TK::READONLY;
receives			return TK::RECEIVES;
struct				return TK::STRUCT;
true				SET_BOOL( true ) return TK::BOOLEAN;
void				return TK::VOID;
";"					return ';';
","					return ',';
"{"					return '{';
"}"					return '}';
"["					return '[';
"]"					return ']';
"("					return '(';
")"					return ')';
"="					return '=';
"+"					return '+';
"-"					return '-';
"*"					return '*';
"/"					return '/';

any					SET_CSTR( "any" ) return TK::PRIMITIVE_TYPE;
bool				SET_CSTR( "bool" ) return TK::PRIMITIVE_TYPE;
double				SET_CSTR( "double" ) return TK::PRIMITIVE_TYPE;
int16				SET_CSTR( "int16" ) return TK::PRIMITIVE_TYPE;
int32				SET_CSTR( "int32" ) return TK::PRIMITIVE_TYPE;
int64				SET_CSTR( "int64" ) return TK::PRIMITIVE_TYPE;
int8				SET_CSTR( "int8" ) return TK::PRIMITIVE_TYPE;
string				SET_CSTR( "string" ) return TK::PRIMITIVE_TYPE;

{ID}				SET_STRING( yytext ) return TK::ID;
{QUALID}			SET_STRING( yytext ) return TK::QUALIFIED_ID;
\@({ID}|{QUALID})	SET_STRING( yytext + 1 ) return TK::ANNOTATION_ID;

0[xX]{HEXDIGIT}+			SET_NUMBER( strtol( &yytext[2], NULL, 16 ) ) return TK::NUMBER;
{DIGIT}+					SET_NUMBER( strtol( yytext, NULL, 10 ) ) return TK::NUMBER;
{DIGIT}+{EXP}				SET_NUMBER( strtod( yytext, NULL ) ) return TK::NUMBER;
{DIGIT}*\.{DIGIT}+{EXP}?	SET_NUMBER( strtod( yytext, NULL ) ) return TK::NUMBER;
{DIGIT}+\.{DIGIT}*{EXP}?	SET_NUMBER( strtod( yytext, NULL ) ) return TK::NUMBER;

"//".*\n			{	// single-line comments
						yylloc->lines( 1 );
						if( loader.hasDoc() )
						{
							yytext[yyleng - 1] = '\0';
							SET_STRING( &yytext[2] )
							return TK::COMMENT;
						}
						yylloc->step();
					}


"/*"				{	// start of a multi-line comment
						if( loader.hasDoc() ) STR_NEW()
						caller = YY_START; BEGIN(CMT);
					}
<CMT>[^*\n]*		{	// eat anything that's not an '*'
						if( loader.hasDoc() ) STR_APPEND( yytext )
					}
<CMT>"*"+[^*/\n]*	{	// eat up '*'s not followed by '/'s
						if( loader.hasDoc() ) STR_APPEND( yytext )
					}
<CMT>\n				{	// eat up newlines
						if( loader.hasDoc() ) STR_PUSH( '\n' )
						yylloc->lines( 1 );
					}
<CMT>"*"+"/"		{	// end of the multi-line comment
						BEGIN(caller); if( loader.hasDoc() ) return TK::COMMENT;
					}
<CMT><<EOF>>		LEX_ERROR( "unclosed comment" );


"<c++"				{	// start of a C++ code block
						if( loader.hasCppBlocks() ) STR_NEW()
						caller = YY_START; BEGIN(CPP);
					}
<CPP>[^c\n]*		{	// eat anything that's not a 'c'
						if( loader.hasCppBlocks() ) STR_APPEND( yytext )
					}
<CPP>"c"+[^c+\n]*	{	// eat up 'c's not followed by '+'s
						if( loader.hasCppBlocks() ) STR_APPEND( yytext )
					}
<CPP>"c+"[^+\n]*	{	// eat up 'c+'s not followed by '>'s
						if( loader.hasCppBlocks() ) STR_APPEND( yytext )
					}
<CPP>"c++"[^>\n]*	{	// eat up 'c++'s not followed by '>'s
						if( loader.hasCppBlocks() ) STR_APPEND( yytext )
					}
<CPP>\n				{	// eat up newlines
						if( loader.hasCppBlocks() ) STR_PUSH( '\n' )
						yylloc->lines( 1 );
					}
<CPP>"c++>"			{	// end of the C++ code block
						BEGIN(caller); if( loader.hasCppBlocks() ) return TK::CPP_BLOCK;
					}
<CPP><<EOF>>		LEX_ERROR( "unclosed C++ block" );


"\""				/* string literal */ STR_NEW() caller = YY_START; BEGIN(STR);
<STR>"\""			BEGIN(caller); return TK::LITERAL;
<STR>\n				LEX_ERROR( "unterminated string literal" );
<STR>\\[0-9]+		{	/* decimal escape sequence */
						long value = strtol( yytext + 1, NULL, 10 );
						if( value > 0xFF )
							LEX_ERROR( "decimal escape sequence is out of range (0-255)" );
						STR_PUSH( static_cast<char>( value ) );
					}
<STR>\\a			STR_PUSH( '\a' )
<STR>\\b			STR_PUSH( '\b' )
<STR>\\f			STR_PUSH( '\f' )
<STR>\\n			STR_PUSH( '\n' )
<STR>\\r			STR_PUSH( '\r' )
<STR>\\t			STR_PUSH( '\t' )
<STR>\\v			STR_PUSH( '\v' )
<STR>\\.			STR_PUSH( yytext[1] )
<STR>\\\n			yylloc->lines( 1 );
<STR>[^\\\n\x22]+	STR_APPEND( yytext )
<STR>\\				LEX_ERROR( "incomplete escape sequence" );


{BLANK}+		/* eat blanks */ yylloc->step();
\r?\n			yylloc->lines( 1 ); yylloc->step();
.				{
					LEX_ERROR( "unknown character '" << yytext[0] << "' (0x"
						<< std::hex << static_cast<int>( yytext[0] ) << ")" );
				}

%%
