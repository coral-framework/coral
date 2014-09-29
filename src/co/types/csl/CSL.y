/* Parser for the Coral Specification Language (CSL) */

/**
%debug
%verbose
/**/

%no-lines

%require "2.5"
%language "C++"
%locations
%defines "parser.hh"
%output "parser.cc"
%define namespace "co::csl"
%define parser_class_name "Parser"
%define filename_type "const std::string"
%name-prefix "csl_"

%parse-param { co::csl::Loader& loader }
%parse-param { void* scanner }
%lex-param   { co::csl::Loader& loader }
%lex-param   { void* scanner }

%code requires
{
	#include "Loader.h"
	#include "location.hh"
	#include <co/Any.h>
	#include <sstream>

	#define YY_DECL int csl_lex( co::csl::Parser::semantic_type* yylval_param, \
		YYLTYPE* yylloc_param, co::csl::Loader& loader, yyscan_t yyscanner )

	#define YYLTYPE co::csl::location

	#define PUSH_ERROR( loc, msg ) { std::stringstream errorSS; \
		errorSS << msg; loader.pushError( loc, errorSS.str() ); }

	namespace co { namespace csl { class Loader; } }
}

%code {
	#include "scanner.hh"
	#include <co/Exception.h>

	#define PARSE_EV( ev ) loader.ev; if( loader.getError() ) YYABORT;

	// declare the scanner function
	YY_DECL;
}

%initial-action
{
	@$.begin.filename = @$.end.filename = loader.getCurrentFileName();
}

%union
{
	bool b;
	co::int32 i32;
	double num;
	std::string* str;
	const char* cstr;
	co::AnyValue* any;
}

%token			END				0 "end of file"
%token	<str>	ANNOTATION_ID	"annotation identifier"
%token	<b>		BOOLEAN			"boolean value"
%token	<str>	COMMENT			"comment"
%token			COMPONENT		"component"
%token	<str>	CPP_BLOCK		"C++ block"
%token			ENUM			"enum"
%token			EXCEPTION		"exception"
%token			EXTENDS			"extends"
%token	<str>	ID				"identifier"
%token			IMPORT			"import"
%token			IN				"in"
%token			INOUT			"inout"
%token			INTERFACE		"interface"
%token	<str>	LITERAL			"string literal"
%token			NATIVECLASS		"native class"
%token	<num>	NUMBER			"number"
%token			OUT				"out"
%token	<cstr>	PRIMITIVE_TYPE	"primitive type"
%token			PROVIDES		"provides"
%token	<str>	QUALIFIED_ID	"qualified identifier"
%token			RAISES			"raises"
%token			READONLY		"readonly"
%token			RECEIVES		"receives"
%token			STRUCT			"struct"
%token			VOID			"void"

%type <b> opt_array port_kind
%type <i32> inout
%type <num> num_exp
%type <str> qualified_id identifier
%type <any> exp

%left '-' '+'
%left '*' '/'
%left NEG

%start csl_file

%%

csl_file
	:	sentence_list
	|	error {
			if( loader.getError() )
			{
				// we must've got a lexical error, so abort...
			}
			else
			{
				PUSH_ERROR( yyla.location, "syntax error near '" << csl_get_text( scanner ) << "'" )
			}
			YYABORT;
		}
	;

sentence_list
	:	sentence
	|	sentence_list sentence
	;

sentence
	: comment
	| annotation
	| statement ';'
	;

statement
	: import_clause
	| enum_spec
	| exception_spec
	| struct_spec
	| nativeclass_spec
	| interface_spec
	| component_spec
	;

import_clause
	: IMPORT qualified_id { PARSE_EV( onImport( @IMPORT, *$qualified_id ) ) }
	;

comment
	: COMMENT { PARSE_EV( onComment( @COMMENT, *$COMMENT ) ) }
	;

comment_list
	: comment
	| comment_list comment
	;

opt_comment_list
	: /* nothing */
	| comment_list
	;

identifier
	: ID			{ $$ = $ID; }
	| COMPONENT		{ static std::string sComponent( "component" ); $$ = &sComponent; }
	| INTERFACE		{ static std::string sInterface( "interface" ); $$ = &sInterface; }
	;

qualified_id
	: ID
	| QUALIFIED_ID
	;

annotation
	: ANNOTATION_ID { PARSE_EV( onAnnotation( @1, *$1, false ) ) }
	| ANNOTATION_ID '(' { PARSE_EV( onAnnotation( @1, *$1, true ) ) } annotation_data ')'
	;

annotation_data
	: exp { PARSE_EV( onAnnotationData( @exp, "value", *$exp ) ) } opt_field_value_list
	| field_value_list
	;

opt_field_value_list
	: /* nothing */
	| ',' field_value_list
	;

field_value_list
	: field_value
	| field_value_list ',' field_value
	;

field_value
	: ID '=' exp { PARSE_EV( onAnnotationData( @$, *$ID, *$exp ) ) }
	;

enum_spec
	: ENUM ID { PARSE_EV( onTypeSpec( @ENUM, co::TK_ENUM, @ID, *$ID ) ) }
		'{' enum_identifier_list '}'
	;

enum_identifier
	: opt_comment_list ID { PARSE_EV( onEnumIdentifier( @ID, *$ID ) ) } opt_comment_list
	;

enum_identifier_list
	: enum_identifier
	| enum_identifier_list ',' enum_identifier
	;

exception_spec
	: EXCEPTION ID { PARSE_EV( onTypeSpec( @EXCEPTION, co::TK_EXCEPTION, @ID, *$ID ) ) }
	;

struct_spec
	: STRUCT ID { PARSE_EV( onTypeSpec( @STRUCT, co::TK_STRUCT, @ID, *$ID ) ) }
		'{' opt_record_member_list '}'
	;

opt_record_member_list
	: /* nothing */
	| record_member_list
	;

record_member_list
	: record_member
	| record_member_list record_member
	;

record_member
	: comment
	| annotation
	| field_decl
	;

field_decl
	: type_decl identifier ';' { PARSE_EV( onField( @identifier, *$identifier, false ) ) }
	| READONLY type_decl identifier ';' { PARSE_EV( onField( @identifier, *$identifier, true ) ) }
	;

nativeclass_spec
	: NATIVECLASS ID { PARSE_EV( onTypeSpec( @NATIVECLASS, co::TK_NATIVECLASS, @ID, *$ID ) ) }
		'{' opt_class_member_list '}'
	;

opt_class_member_list
	: /* nothing */
	| class_member_list
	;

class_member_list
	: class_member
	| class_member_list class_member
	;

class_member
	: CPP_BLOCK { PARSE_EV( onCppBlock( @CPP_BLOCK, *$CPP_BLOCK ) ) }
	| method_decl
	| record_member
	;

method_decl
	: type_decl identifier method_remaining_parts
	| VOID { PARSE_EV( onTypeDecl( @VOID, "void", false ) ) } identifier method_remaining_parts
	;

method_remaining_parts
	: '(' { PARSE_EV( onMethod( @0, *$<str>0 ) ) } opt_param_list ')'
			opt_exception_list ';' { PARSE_EV( onMethodEnd( @0) ) }
	;

opt_param_list
	: /* nothing */
	| param_list
	;

param_list
	: param
	| param_list ',' param
	;

param
	: inout type_decl ID { int v = $inout;
		PARSE_EV( onParameter( @ID, ( v & 1 ) != 0, ( v & 2 ) != 0, *$ID ) ) }
	;

inout
	: IN	{ $$ = 1; }
	| OUT	{ $$ = 2; }
	| INOUT { $$ = 3; }
	| error { PUSH_ERROR( @$, "expected 'in', 'out' or 'inout' before '"
		<< csl_get_text( scanner ) << "'" ); YYABORT; }
	;

opt_exception_list
	: /* nothing */
	| RAISES exception_list
	;

exception_list
	: exception
	| exception_list ',' exception
	;

exception
	: qualified_id { PARSE_EV( onRaises( @qualified_id, *$qualified_id ) ) }
	;

type_decl
	: PRIMITIVE_TYPE opt_array { PARSE_EV( onTypeDecl( @$, $PRIMITIVE_TYPE, $opt_array ) ) }
	| qualified_id opt_array { PARSE_EV( onTypeDecl( @$, *$qualified_id, $opt_array ) ) }
	;

opt_array
	: /* nothing */ { $$ = false; }
	| '[' ']'		{ $$ = true; }
	;

interface_spec
	: INTERFACE ID { PARSE_EV( onTypeSpec( @INTERFACE, co::TK_INTERFACE, @ID, *$ID ) ) }
		opt_inheritance_decl '{' opt_class_member_list '}'
	;

opt_inheritance_decl
	: /* nothing */
	| EXTENDS qualified_id { PARSE_EV( onBaseType( @qualified_id, *$qualified_id ) ) }
	;

component_spec
	: COMPONENT ID { PARSE_EV( onTypeSpec( @COMPONENT, co::TK_COMPONENT, @ID, *$ID ) ) }
		'{' opt_component_member_list '}'
	;

opt_component_member_list
	: /* nothing */
	| component_member_list
	;

component_member_list
	: component_member
	| component_member_list component_member
	;

component_member
	: comment
	| annotation
	| port_decl
	;

port_decl
	: port_kind qualified_id { PARSE_EV( onTypeDecl( @qualified_id, *$qualified_id, false ) ) }
		ID ';' { PARSE_EV( onPort( @ID, $port_kind, *$ID ) ) }
	;

port_kind
	: PROVIDES	{ $$ = true; }
	| RECEIVES	{ $$ = false; }
	;

exp
	:	BOOLEAN			{ $$ = loader.newAny(); $$->set( $1 ); }
	|	LITERAL			{ $$ = loader.newAny(); $$->set( *$1 ); }
	|	qualified_id	{ $$ = loader.newAny(); $$->set( *$1 ); }
	|	num_exp			{ $$ = loader.newAny(); $$->set( $1 ); }
	;

num_exp
	:	NUMBER					{ $$ = $NUMBER; }
	|	'(' num_exp ')'			{ $$ = $2; }
	|	num_exp '+' num_exp		{ $$ = $1 + $3; }
	|	num_exp '-' num_exp		{ $$ = $1 - $3; }
	|	num_exp '*' num_exp		{ $$ = $1 * $3; }
	|	num_exp '/' num_exp		{ $$ = $1 / $3; }
	|	'-' num_exp %prec NEG	{ $$ = -$2; }
	;

%%

void co::csl::Parser::error( const location_type&, const std::string& )
{
	// ignore
}
