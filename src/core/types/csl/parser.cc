/* A Bison parser, made by GNU Bison 2.6.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

// Take the name prefix into account.
#define yylex   csl_lex

/* First part of user declarations.  */




#include "parser.hh"

/* User implementation prologue.  */


/* Unqualified %code blocks.  */


	#include "scanner.hh"
	#include <co/Exception.h>

	#define PARSE_EV( ev ) loader.ev; if( loader.getError() ) YYABORT;

	// declare the scanner function
	YY_DECL;





# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace co { namespace csl {


  /// Build a parser object.
  Parser::Parser (co::csl::Loader& loader_yyarg, void* scanner_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      loader (loader_yyarg),
      scanner (scanner_yyarg)
  {
  }

  Parser::~Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  Parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  Parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  Parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    
{
	yylloc.begin.filename = yylloc.end.filename = loader.getCurrentFileName();
}


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc, loader, scanner);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 3:

    {
			if( loader.getError() )
			{
				// we must've got a lexical error, so abort...
			}
			else
			{
				PUSH_ERROR( yylloc, "syntax error near '" << csl_get_text( scanner ) << "'" )
			}
			YYABORT;
		}
    break;

  case 16:

    { PARSE_EV( onImport( (yylocation_stack_[(2) - (1)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 17:

    { PARSE_EV( onComment( (yylocation_stack_[(1) - (1)]), *(yysemantic_stack_[(1) - (1)].str) ) ) }
    break;

  case 22:

    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); }
    break;

  case 23:

    { static std::string sComponent( "component" ); (yyval.str) = &sComponent; }
    break;

  case 24:

    { static std::string sInterface( "interface" ); (yyval.str) = &sInterface; }
    break;

  case 27:

    { PARSE_EV( onAnnotation( (yylocation_stack_[(1) - (1)]), *(yysemantic_stack_[(1) - (1)].str), false ) ) }
    break;

  case 28:

    { PARSE_EV( onAnnotation( (yylocation_stack_[(2) - (1)]), *(yysemantic_stack_[(2) - (1)].str), true ) ) }
    break;

  case 30:

    { PARSE_EV( onAnnotationData( (yylocation_stack_[(1) - (1)]), "value", *(yysemantic_stack_[(1) - (1)].any) ) ) }
    break;

  case 37:

    { PARSE_EV( onAnnotationData( (yyloc), *(yysemantic_stack_[(3) - (1)].str), *(yysemantic_stack_[(3) - (3)].any) ) ) }
    break;

  case 38:

    { PARSE_EV( onTypeSpec( (yylocation_stack_[(2) - (1)]), co::TK_ENUM, (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 40:

    { PARSE_EV( onEnumIdentifier( (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 44:

    { PARSE_EV( onTypeSpec( (yylocation_stack_[(2) - (1)]), co::TK_EXCEPTION, (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 45:

    { PARSE_EV( onTypeSpec( (yylocation_stack_[(2) - (1)]), co::TK_STRUCT, (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 54:

    { PARSE_EV( onField( (yylocation_stack_[(3) - (2)]), *(yysemantic_stack_[(3) - (2)].str), false ) ) }
    break;

  case 55:

    { PARSE_EV( onField( (yylocation_stack_[(4) - (3)]), *(yysemantic_stack_[(4) - (3)].str), true ) ) }
    break;

  case 56:

    { PARSE_EV( onTypeSpec( (yylocation_stack_[(2) - (1)]), co::TK_NATIVECLASS, (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 62:

    { PARSE_EV( onCppBlock( (yylocation_stack_[(1) - (1)]), *(yysemantic_stack_[(1) - (1)].str) ) ) }
    break;

  case 66:

    { PARSE_EV( onTypeDecl( (yylocation_stack_[(1) - (1)]), "void", false ) ) }
    break;

  case 68:

    { PARSE_EV( onMethod( (yylocation_stack_[(1) - (0)]), *(yysemantic_stack_[(1) - (0)].str) ) ) }
    break;

  case 69:

    { PARSE_EV( onMethodEnd( (yylocation_stack_[(6) - (0)])) ) }
    break;

  case 74:

    { int v = (yysemantic_stack_[(3) - (1)].i32);
		PARSE_EV( onParameter( (yylocation_stack_[(3) - (3)]), ( v & 1 ) != 0, ( v & 2 ) != 0, *(yysemantic_stack_[(3) - (3)].str) ) ) }
    break;

  case 75:

    { (yyval.i32) = 1; }
    break;

  case 76:

    { (yyval.i32) = 2; }
    break;

  case 77:

    { (yyval.i32) = 3; }
    break;

  case 78:

    { PUSH_ERROR( (yyloc), "expected 'in', 'out' or 'inout' before '"
		<< csl_get_text( scanner ) << "'" ); YYABORT; }
    break;

  case 83:

    { PARSE_EV( onRaises( (yylocation_stack_[(1) - (1)]), *(yysemantic_stack_[(1) - (1)].str) ) ) }
    break;

  case 84:

    { PARSE_EV( onTypeDecl( (yyloc), (yysemantic_stack_[(2) - (1)].cstr), (yysemantic_stack_[(2) - (2)].b) ) ) }
    break;

  case 85:

    { PARSE_EV( onTypeDecl( (yyloc), *(yysemantic_stack_[(2) - (1)].str), (yysemantic_stack_[(2) - (2)].b) ) ) }
    break;

  case 86:

    { (yyval.b) = false; }
    break;

  case 87:

    { (yyval.b) = true; }
    break;

  case 88:

    { PARSE_EV( onTypeSpec( (yylocation_stack_[(2) - (1)]), co::TK_INTERFACE, (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 91:

    { PARSE_EV( onBaseType( (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 92:

    { PARSE_EV( onTypeSpec( (yylocation_stack_[(2) - (1)]), co::TK_COMPONENT, (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str) ) ) }
    break;

  case 101:

    { PARSE_EV( onTypeDecl( (yylocation_stack_[(2) - (2)]), *(yysemantic_stack_[(2) - (2)].str), false ) ) }
    break;

  case 102:

    { PARSE_EV( onPort( (yylocation_stack_[(5) - (4)]), (yysemantic_stack_[(5) - (1)].b), *(yysemantic_stack_[(5) - (4)].str) ) ) }
    break;

  case 103:

    { (yyval.b) = true; }
    break;

  case 104:

    { (yyval.b) = false; }
    break;

  case 105:

    { (yyval.any) = loader.newAny(); (yyval.any)->set( (yysemantic_stack_[(1) - (1)].b) ); }
    break;

  case 106:

    { (yyval.any) = loader.newAny(); (yyval.any)->set( *(yysemantic_stack_[(1) - (1)].str) ); }
    break;

  case 107:

    { (yyval.any) = loader.newAny(); (yyval.any)->set( *(yysemantic_stack_[(1) - (1)].str) ); }
    break;

  case 108:

    { (yyval.any) = loader.newAny(); (yyval.any)->set( (yysemantic_stack_[(1) - (1)].num) ); }
    break;

  case 109:

    { (yyval.num) = (yysemantic_stack_[(1) - (1)].num); }
    break;

  case 110:

    { (yyval.num) = (yysemantic_stack_[(3) - (2)].num); }
    break;

  case 111:

    { (yyval.num) = (yysemantic_stack_[(3) - (1)].num) + (yysemantic_stack_[(3) - (3)].num); }
    break;

  case 112:

    { (yyval.num) = (yysemantic_stack_[(3) - (1)].num) - (yysemantic_stack_[(3) - (3)].num); }
    break;

  case 113:

    { (yyval.num) = (yysemantic_stack_[(3) - (1)].num) * (yysemantic_stack_[(3) - (3)].num); }
    break;

  case 114:

    { (yyval.num) = (yysemantic_stack_[(3) - (1)].num) / (yysemantic_stack_[(3) - (3)].num); }
    break;

  case 115:

    { (yyval.num) = -(yysemantic_stack_[(2) - (2)].num); }
    break;



	default:
          break;
      }
    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char Parser::yypact_ninf_ = -89;
  const short int
  Parser::yypact_[] =
  {
        82,   -89,   -26,   -89,    10,    13,    22,     0,    23,    29,
      31,    14,   119,   -89,    38,   -89,   -89,   -89,   -89,   -89,
     -89,   -89,   -89,   -89,   -89,   -89,   -89,   -89,   -89,   -89,
     -89,   -89,   -89,   -89,   -89,   -89,   -89,    25,    32,    35,
      65,    40,    43,   -89,    47,   -89,   -89,    17,    17,   -89,
      54,    66,   -89,   -89,    37,     7,   100,     0,    68,    93,
      90,    58,   -89,   113,   -89,    96,    73,    17,    17,    17,
      17,   -89,   -89,   -89,   -89,    72,     7,   -89,   -89,     0,
     -89,   100,   105,   -89,   -30,   -89,    93,   -89,    78,    41,
     -89,   -89,    78,   -89,   -89,   -89,    80,    93,   -89,   -89,
      49,    84,    90,   -89,    49,   -89,   -89,    47,   -89,    96,
     -89,    -4,    -4,   -89,   -89,   -89,   -89,   -89,   -89,   -89,
     100,   -89,    87,    88,   -89,    49,    49,   -89,   -89,   -89,
     -89,   -89,   -89,    15,   -89,   -89,    97,    66,   124,   100,
     -89,   -89,   -89,   104,   112,   -89,   -89,   -89,   106,   -89,
     -89,   -89,     4,   -89,   -89,   -89,   -89,   -89,   103,   111,
     -89,    41,   126,     6,   139,     0,   118,   -89,   -89,   -89,
     120,   -89,   -89,     0,   -89
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  Parser::yydefact_[] =
  {
         0,     3,    27,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     2,     4,     0,     9,     6,     7,    10,    11,
      12,    13,    14,    15,    28,    92,    38,    44,    25,    26,
      16,    88,    56,    45,     1,     5,     8,     0,     0,     0,
      90,     0,     0,   105,    25,   106,   109,     0,     0,   107,
       0,    32,    35,    30,   108,    94,    20,     0,     0,    58,
      47,     0,   115,     0,    29,     0,    33,     0,     0,     0,
       0,   103,   104,    98,    99,     0,    95,    96,   100,     0,
      18,    21,     0,    42,     0,    91,    58,    62,    86,     0,
      66,    51,    86,    52,    64,    53,     0,    59,    60,    63,
       0,     0,    48,    49,     0,    37,   110,     0,    36,     0,
      31,   112,   111,   113,   114,    93,    97,   101,    19,    40,
      20,    39,     0,     0,    84,     0,     0,    85,    57,    61,
      23,    22,    24,     0,    46,    50,     0,    34,     0,    20,
      43,    89,    87,     0,     0,    54,    68,    65,     0,    41,
      55,    67,     0,   102,    78,    75,    77,    76,     0,    71,
      72,     0,    79,     0,     0,     0,     0,    73,    74,    83,
      80,    81,    69,     0,    82
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  Parser::yypgoto_[] =
  {
       -89,   -89,   -89,   145,   -89,   -89,     1,   -89,    20,   -88,
      -7,     3,   -89,   -89,   -89,   -89,    51,    98,   -89,   -89,
      42,   -89,   -89,   -89,   -89,   -89,   -89,   -89,   -56,   -89,
     -89,   -89,    75,   -89,    67,   -89,   -89,    21,   -89,   -89,
     -89,     5,   -89,   -89,   -89,    -6,   -58,    77,   -89,   -89,
     -89,   -89,   -89,   -89,   -89,    94,   -89,   -89,   -89,   110,
      85
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  Parser::yydefgoto_[] =
  {
        -1,    11,    12,    13,    14,    15,    91,    81,    82,   133,
      92,    93,    37,    50,    66,   110,    51,    52,    18,    39,
      83,   139,    84,    19,    20,    42,   101,   102,    94,    95,
      21,    41,    96,    97,    98,    99,   126,   147,   152,   158,
     159,   160,   161,   166,   170,   171,   100,   124,    22,    40,
      58,    23,    38,    75,    76,    77,    78,   138,    79,    53,
      54
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char Parser::yytable_ninf_ = -71;
  const short int
  Parser::yytable_[] =
  {
        30,    16,   104,    17,   103,   154,   120,   154,    24,   121,
       2,    28,     3,    16,    34,    17,   136,   155,   156,   155,
     156,    25,    29,   157,    26,   157,    69,    70,    71,    43,
      49,   125,    72,    27,    31,    46,    44,   143,   144,   -70,
      32,    45,    33,    46,   104,    47,   135,    29,   145,   146,
      85,    48,    28,    47,    49,   130,    73,    80,    74,    48,
     131,    88,    43,    29,   132,    67,    68,    69,    70,    28,
      55,    36,   117,    56,    45,    57,    46,    73,    59,    74,
      29,    60,   118,     1,    61,     2,    47,     3,     4,    64,
       5,     6,    48,     2,     7,     3,     2,     8,     3,     9,
      87,    28,    65,   164,    28,     3,    86,   107,    10,   109,
      88,   115,    29,    88,    89,    29,   119,    89,   123,   128,
      90,    80,     2,   134,     3,     4,   141,     5,     6,   142,
     145,     7,    62,    63,     8,   148,     9,   150,   162,   153,
      80,    67,    68,    69,    70,    10,   146,   163,   106,   165,
     168,   172,   111,   112,   113,   114,   173,    35,   169,   149,
     137,   122,   140,   108,   129,   151,   169,   174,   167,   127,
     116,   105
  };

  /* YYCHECK.  */
  const unsigned char
  Parser::yycheck_[] =
  {
         7,     0,    60,     0,    60,     1,    36,     1,    34,    39,
       3,    11,     5,    12,     0,    12,   104,    13,    14,    13,
      14,    11,    22,    19,    11,    19,    30,    31,    21,     4,
      37,    89,    25,    11,    11,    18,    11,   125,   126,    35,
      11,    16,    11,    18,   102,    28,   102,    22,    33,    34,
      57,    34,    11,    28,    61,     6,    55,    56,    55,    34,
      11,    20,     4,    22,    15,    28,    29,    30,    31,    11,
      38,    33,    79,    38,    16,    10,    18,    76,    38,    76,
      22,    38,    81,     1,    37,     3,    28,     5,     6,    35,
       8,     9,    34,     3,    12,     5,     3,    15,     5,    17,
       7,    11,    36,   161,    11,     5,    38,    11,    26,    36,
      20,    39,    22,    20,    24,    22,    11,    24,    40,    39,
      27,   120,     3,    39,     5,     6,    39,     8,     9,    41,
      33,    12,    47,    48,    15,    11,    17,    33,    35,    33,
     139,    28,    29,    30,    31,    26,    34,    36,    35,    23,
      11,    33,    67,    68,    69,    70,    36,    12,   165,   139,
     109,    86,   120,    65,    97,   144,   173,   173,   163,    92,
      76,    61
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  Parser::yystos_[] =
  {
         0,     1,     3,     5,     6,     8,     9,    12,    15,    17,
      26,    43,    44,    45,    46,    47,    48,    53,    60,    65,
      66,    72,    90,    93,    34,    11,    11,    11,    11,    22,
      52,    11,    11,    11,     0,    45,    33,    54,    94,    61,
      91,    73,    67,     4,    11,    16,    18,    28,    34,    52,
      55,    58,    59,   101,   102,    38,    38,    10,    92,    38,
      38,    37,   102,   102,    35,    36,    56,    28,    29,    30,
      31,    21,    25,    48,    53,    95,    96,    97,    98,   100,
      48,    49,    50,    62,    64,    52,    38,     7,    20,    24,
      27,    48,    52,    53,    70,    71,    74,    75,    76,    77,
      88,    68,    69,    70,    88,   101,    35,    11,    59,    36,
      57,   102,   102,   102,   102,    39,    97,    52,    48,    11,
      36,    39,    74,    40,    89,    88,    78,    89,    39,    76,
       6,    11,    15,    51,    39,    70,    51,    58,    99,    63,
      62,    39,    41,    51,    51,    33,    34,    79,    11,    50,
      33,    79,    80,    33,     1,    13,    14,    19,    81,    82,
      83,    84,    35,    36,    88,    23,    85,    83,    11,    52,
      86,    87,    33,    36,    87
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,    45,    43,
      42,    47,   283,    59,    40,    41,    44,    61,   123,   125,
      91,    93
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  Parser::yyr1_[] =
  {
         0,    42,    43,    43,    44,    44,    45,    45,    45,    46,
      46,    46,    46,    46,    46,    46,    47,    48,    49,    49,
      50,    50,    51,    51,    51,    52,    52,    53,    54,    53,
      56,    55,    55,    57,    57,    58,    58,    59,    61,    60,
      63,    62,    64,    64,    65,    67,    66,    68,    68,    69,
      69,    70,    70,    70,    71,    71,    73,    72,    74,    74,
      75,    75,    76,    76,    76,    77,    78,    77,    80,    79,
      81,    81,    82,    82,    83,    84,    84,    84,    84,    85,
      85,    86,    86,    87,    88,    88,    89,    89,    91,    90,
      92,    92,    94,    93,    95,    95,    96,    96,    97,    97,
      97,    99,    98,   100,   100,   101,   101,   101,   101,   102,
     102,   102,   102,   102,   102,   102
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  Parser::yyr2_[] =
  {
         0,     2,     1,     1,     1,     2,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     2,
       0,     1,     1,     1,     1,     1,     1,     1,     0,     5,
       0,     3,     1,     0,     2,     1,     3,     3,     0,     6,
       0,     4,     1,     3,     2,     0,     6,     0,     1,     1,
       2,     1,     1,     1,     3,     4,     0,     6,     0,     1,
       1,     2,     1,     1,     1,     3,     0,     4,     0,     6,
       0,     1,     1,     3,     3,     1,     1,     1,     1,     0,
       2,     1,     3,     1,     2,     2,     0,     2,     0,     7,
       0,     2,     0,     6,     0,     1,     1,     2,     1,     1,
       1,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     2
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const Parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "\"annotation identifier\"",
  "\"boolean value\"", "\"comment\"", "\"component\"", "\"C++ block\"",
  "\"enum\"", "\"exception\"", "\"extends\"", "\"identifier\"",
  "\"import\"", "\"in\"", "\"inout\"", "\"interface\"",
  "\"string literal\"", "\"native class\"", "\"number\"", "\"out\"",
  "\"primitive type\"", "\"provides\"", "\"qualified identifier\"",
  "\"raises\"", "\"readonly\"", "\"receives\"", "\"struct\"", "\"void\"",
  "'-'", "'+'", "'*'", "'/'", "NEG", "';'", "'('", "')'", "','", "'='",
  "'{'", "'}'", "'['", "']'", "$accept", "csl_file", "sentence_list",
  "sentence", "statement", "import_clause", "comment", "comment_list",
  "opt_comment_list", "identifier", "qualified_id", "annotation", "$@1",
  "annotation_data", "$@2", "opt_field_value_list", "field_value_list",
  "field_value", "enum_spec", "$@3", "enum_identifier", "$@4",
  "enum_identifier_list", "exception_spec", "struct_spec", "$@5",
  "opt_record_member_list", "record_member_list", "record_member",
  "field_decl", "nativeclass_spec", "$@6", "opt_class_member_list",
  "class_member_list", "class_member", "method_decl", "$@7",
  "method_remaining_parts", "$@8", "opt_param_list", "param_list", "param",
  "inout", "opt_exception_list", "exception_list", "exception",
  "type_decl", "opt_array", "interface_spec", "$@9",
  "opt_inheritance_decl", "component_spec", "$@10",
  "opt_component_member_list", "component_member_list", "component_member",
  "port_decl", "$@11", "port_kind", "exp", "num_exp", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const Parser::rhs_number_type
  Parser::yyrhs_[] =
  {
        43,     0,    -1,    44,    -1,     1,    -1,    45,    -1,    44,
      45,    -1,    48,    -1,    53,    -1,    46,    33,    -1,    47,
      -1,    60,    -1,    65,    -1,    66,    -1,    72,    -1,    90,
      -1,    93,    -1,    12,    52,    -1,     5,    -1,    48,    -1,
      49,    48,    -1,    -1,    49,    -1,    11,    -1,     6,    -1,
      15,    -1,    11,    -1,    22,    -1,     3,    -1,    -1,     3,
      34,    54,    55,    35,    -1,    -1,   101,    56,    57,    -1,
      58,    -1,    -1,    36,    58,    -1,    59,    -1,    58,    36,
      59,    -1,    11,    37,   101,    -1,    -1,     8,    11,    61,
      38,    64,    39,    -1,    -1,    50,    11,    63,    50,    -1,
      62,    -1,    64,    36,    62,    -1,     9,    11,    -1,    -1,
      26,    11,    67,    38,    68,    39,    -1,    -1,    69,    -1,
      70,    -1,    69,    70,    -1,    48,    -1,    53,    -1,    71,
      -1,    88,    51,    33,    -1,    24,    88,    51,    33,    -1,
      -1,    17,    11,    73,    38,    74,    39,    -1,    -1,    75,
      -1,    76,    -1,    75,    76,    -1,     7,    -1,    77,    -1,
      70,    -1,    88,    51,    79,    -1,    -1,    27,    78,    51,
      79,    -1,    -1,    34,    80,    81,    35,    85,    33,    -1,
      -1,    82,    -1,    83,    -1,    82,    36,    83,    -1,    84,
      88,    11,    -1,    13,    -1,    19,    -1,    14,    -1,     1,
      -1,    -1,    23,    86,    -1,    87,    -1,    86,    36,    87,
      -1,    52,    -1,    20,    89,    -1,    52,    89,    -1,    -1,
      40,    41,    -1,    -1,    15,    11,    91,    92,    38,    74,
      39,    -1,    -1,    10,    52,    -1,    -1,     6,    11,    94,
      38,    95,    39,    -1,    -1,    96,    -1,    97,    -1,    96,
      97,    -1,    48,    -1,    53,    -1,    98,    -1,    -1,   100,
      52,    99,    11,    33,    -1,    21,    -1,    25,    -1,     4,
      -1,    16,    -1,    52,    -1,   102,    -1,    18,    -1,    34,
     102,    35,    -1,   102,    29,   102,    -1,   102,    28,   102,
      -1,   102,    30,   102,    -1,   102,    31,   102,    -1,    28,
     102,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  Parser::yyprhs_[] =
  {
         0,     0,     3,     5,     7,     9,    12,    14,    16,    19,
      21,    23,    25,    27,    29,    31,    33,    36,    38,    40,
      43,    44,    46,    48,    50,    52,    54,    56,    58,    59,
      65,    66,    70,    72,    73,    76,    78,    82,    86,    87,
      94,    95,   100,   102,   106,   109,   110,   117,   118,   120,
     122,   125,   127,   129,   131,   135,   140,   141,   148,   149,
     151,   153,   156,   158,   160,   162,   166,   167,   172,   173,
     180,   181,   183,   185,   189,   193,   195,   197,   199,   201,
     202,   205,   207,   211,   213,   216,   219,   220,   223,   224,
     232,   233,   236,   237,   244,   245,   247,   249,   252,   254,
     256,   258,   259,   265,   267,   269,   271,   273,   275,   277,
     279,   283,   287,   291,   295,   299
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  Parser::yyrline_[] =
  {
         0,   110,   110,   111,   125,   126,   130,   131,   132,   136,
     137,   138,   139,   140,   141,   142,   146,   150,   154,   155,
     158,   160,   164,   165,   166,   170,   171,   175,   176,   176,
     180,   180,   181,   184,   186,   190,   191,   195,   199,   199,
     204,   204,   208,   209,   213,   217,   217,   221,   223,   227,
     228,   232,   233,   234,   238,   239,   243,   243,   247,   249,
     253,   254,   258,   259,   260,   264,   265,   265,   269,   269,
     273,   275,   279,   280,   284,   289,   290,   291,   292,   296,
     298,   302,   303,   307,   311,   312,   316,   317,   321,   321,
     325,   327,   331,   331,   335,   337,   341,   342,   346,   347,
     348,   352,   352,   357,   358,   362,   363,   364,   365,   369,
     370,   371,   372,   373,   374,   375
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      34,    35,    30,    29,    36,    28,     2,    31,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    33,
       2,    37,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    40,     2,    41,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,    39,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    32
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int Parser::yyeof_ = 0;
  const int Parser::yylast_ = 171;
  const int Parser::yynnts_ = 61;
  const int Parser::yyempty_ = -2;
  const int Parser::yyfinal_ = 34;
  const int Parser::yyterror_ = 1;
  const int Parser::yyerrcode_ = 256;
  const int Parser::yyntokens_ = 42;

  const unsigned int Parser::yyuser_token_number_max_ = 283;
  const Parser::token_number_type Parser::yyundef_token_ = 2;


} } // co::csl




void co::csl::Parser::error( const location_type&, const std::string& )
{
	// ignore
}

