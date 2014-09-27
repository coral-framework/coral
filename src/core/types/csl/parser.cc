// A Bison parser, made by GNU Bison 3.0.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Take the name prefix into account.
#define yylex   csl_lex

// First part of user declarations.



# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "parser.hh"

// User implementation prologue.


// Unqualified %code blocks.


	#include "scanner.hh"
	#include <co/Exception.h>

	#define PARSE_EV( ev ) loader.ev; if( loader.getError() ) YYABORT;

	// declare the scanner function
	YY_DECL;




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
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


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyempty = true)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
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
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
  }

  template <typename Base>
  inline
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  Parser::by_type::by_type ()
     : type (empty)
  {}

  inline
  Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.type = empty;
  }

  inline
  int
  Parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  Parser::by_state::by_state ()
    : state (empty)
  {}

  inline
  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.state = empty;
  }

  inline
  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    return state == empty ? 0 : yystos_[state];
  }

  inline
  Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty;
  }

  inline
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YYUSE (yysym.type_get ());
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
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
#endif // YYDEBUG

  inline Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

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
    /// Whether yyla contains a lookahead.
    bool yyempty = true;

    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    
{
	yyla.location.begin.filename = yyla.location.end.filename = loader.getCurrentFileName();
}



    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyempty)
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location, loader, scanner));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
        yyempty = false;
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Discard the token being shifted.
    yyempty = true;

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
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
				PUSH_ERROR( yyla.location, "syntax error near '" << csl_get_text( scanner ) << "'" )
			}
			YYABORT;
		}

    break;

  case 16:

    { PARSE_EV( onImport( yystack_[1].location, *(yystack_[0].value.str) ) ) }

    break;

  case 17:

    { PARSE_EV( onComment( yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 22:

    { (yylhs.value.str) = (yystack_[0].value.str); }

    break;

  case 23:

    { static std::string sComponent( "component" ); (yylhs.value.str) = &sComponent; }

    break;

  case 24:

    { static std::string sInterface( "interface" ); (yylhs.value.str) = &sInterface; }

    break;

  case 27:

    { PARSE_EV( onAnnotation( yystack_[0].location, *(yystack_[0].value.str), false ) ) }

    break;

  case 28:

    { PARSE_EV( onAnnotation( yystack_[1].location, *(yystack_[1].value.str), true ) ) }

    break;

  case 30:

    { PARSE_EV( onAnnotationData( yystack_[0].location, "value", *(yystack_[0].value.any) ) ) }

    break;

  case 37:

    { PARSE_EV( onAnnotationData( yylhs.location, *(yystack_[2].value.str), *(yystack_[0].value.any) ) ) }

    break;

  case 38:

    { PARSE_EV( onTypeSpec( yystack_[1].location, co::TK_ENUM, yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 40:

    { PARSE_EV( onEnumIdentifier( yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 44:

    { PARSE_EV( onTypeSpec( yystack_[1].location, co::TK_EXCEPTION, yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 45:

    { PARSE_EV( onTypeSpec( yystack_[1].location, co::TK_STRUCT, yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 54:

    { PARSE_EV( onField( yystack_[1].location, *(yystack_[1].value.str), false ) ) }

    break;

  case 55:

    { PARSE_EV( onField( yystack_[1].location, *(yystack_[1].value.str), true ) ) }

    break;

  case 56:

    { PARSE_EV( onTypeSpec( yystack_[1].location, co::TK_NATIVECLASS, yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 62:

    { PARSE_EV( onCppBlock( yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 66:

    { PARSE_EV( onTypeDecl( yystack_[0].location, "void", false ) ) }

    break;

  case 68:

    { PARSE_EV( onMethod( yystack_[1].location, *(yystack_[1].value.str) ) ) }

    break;

  case 69:

    { PARSE_EV( onMethodEnd( yystack_[6].location) ) }

    break;

  case 74:

    { int v = (yystack_[2].value.i32);
		PARSE_EV( onParameter( yystack_[0].location, ( v & 1 ) != 0, ( v & 2 ) != 0, *(yystack_[0].value.str) ) ) }

    break;

  case 75:

    { (yylhs.value.i32) = 1; }

    break;

  case 76:

    { (yylhs.value.i32) = 2; }

    break;

  case 77:

    { (yylhs.value.i32) = 3; }

    break;

  case 78:

    { PUSH_ERROR( yylhs.location, "expected 'in', 'out' or 'inout' before '"
		<< csl_get_text( scanner ) << "'" ); YYABORT; }

    break;

  case 83:

    { PARSE_EV( onRaises( yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 84:

    { PARSE_EV( onTypeDecl( yylhs.location, (yystack_[1].value.cstr), (yystack_[0].value.b) ) ) }

    break;

  case 85:

    { PARSE_EV( onTypeDecl( yylhs.location, *(yystack_[1].value.str), (yystack_[0].value.b) ) ) }

    break;

  case 86:

    { (yylhs.value.b) = false; }

    break;

  case 87:

    { (yylhs.value.b) = true; }

    break;

  case 88:

    { PARSE_EV( onTypeSpec( yystack_[1].location, co::TK_INTERFACE, yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 91:

    { PARSE_EV( onBaseType( yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 92:

    { PARSE_EV( onTypeSpec( yystack_[1].location, co::TK_COMPONENT, yystack_[0].location, *(yystack_[0].value.str) ) ) }

    break;

  case 101:

    { PARSE_EV( onTypeDecl( yystack_[0].location, *(yystack_[0].value.str), false ) ) }

    break;

  case 102:

    { PARSE_EV( onPort( yystack_[1].location, (yystack_[4].value.b), *(yystack_[1].value.str) ) ) }

    break;

  case 103:

    { (yylhs.value.b) = true; }

    break;

  case 104:

    { (yylhs.value.b) = false; }

    break;

  case 105:

    { (yylhs.value.any) = loader.newAny(); (yylhs.value.any)->set( (yystack_[0].value.b) ); }

    break;

  case 106:

    { (yylhs.value.any) = loader.newAny(); (yylhs.value.any)->set( *(yystack_[0].value.str) ); }

    break;

  case 107:

    { (yylhs.value.any) = loader.newAny(); (yylhs.value.any)->set( *(yystack_[0].value.str) ); }

    break;

  case 108:

    { (yylhs.value.any) = loader.newAny(); (yylhs.value.any)->set( (yystack_[0].value.num) ); }

    break;

  case 109:

    { (yylhs.value.num) = (yystack_[0].value.num); }

    break;

  case 110:

    { (yylhs.value.num) = (yystack_[1].value.num); }

    break;

  case 111:

    { (yylhs.value.num) = (yystack_[2].value.num) + (yystack_[0].value.num); }

    break;

  case 112:

    { (yylhs.value.num) = (yystack_[2].value.num) - (yystack_[0].value.num); }

    break;

  case 113:

    { (yylhs.value.num) = (yystack_[2].value.num) * (yystack_[0].value.num); }

    break;

  case 114:

    { (yylhs.value.num) = (yystack_[2].value.num) / (yystack_[0].value.num); }

    break;

  case 115:

    { (yylhs.value.num) = -(yystack_[0].value.num); }

    break;



            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state,
                                           yyempty ? yyempty_ : yyla.type_get ()));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyempty)
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyempty = true;
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
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
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
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

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyempty)
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyempty)
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type, symbol_number_type) const
  {
    return YY_("syntax error");
  }


  const signed char Parser::yypact_ninf_ = -89;

  const signed char Parser::yytable_ninf_ = -71;

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
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
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
  "port_decl", "$@11", "port_kind", "exp", "num_exp", YY_NULLPTR
  };


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
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
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
    const unsigned int user_token_number_max_ = 283;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} } // co::csl




void co::csl::Parser::error( const location_type&, const std::string& )
{
	// ignore
}
