/* A Bison parser, made by GNU Bison 3.4.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

    #include <vector>
    #include <string>
    #include "genArm.hpp"
    #include "type.hpp"
    #include "hashMap.hpp"
    extern int yylex();
    extern char* yytext;
    extern int yylineno;
    extern int genArrayIndex(NIdent* ident);
    extern bool to_debug;
    extern NBlock* prog_block;
    int loop_depth = 0;
    // 语法分析时符号表
    HashMap* parse_time_symtbl = new HashMap();
    // 循环内声明表
    vector<NVarDecl*> loop_decls;
    void yyerror(const char* s) {
        printf("Error %s: %s at line %d\n", s, yytext, yylineno);
        exit(1);
    }

#line 93 "parser.cpp"

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUMBER = 258,
    IDENT = 259,
    BREAK = 260,
    CONTINUE = 261,
    EQ = 262,
    NE = 263,
    GE = 264,
    LE = 265,
    IF = 266,
    ELSE = 267,
    WHILE = 268,
    RETURN = 269,
    AND = 270,
    OR = 271,
    CONST = 272,
    INT = 273,
    VOID = 274,
    LOW_PREC = 275
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 23 "parser.y"

    NBlock*                         block;
    NStmt*                          stmt;
    NExpr*                          expr;
    Assignlist*                     assign_list;
    Varlist*                        var_list;
    Exprlist*                       expr_list;
    NAssign*                        assign;
    Type*                           type;
    NIdent*                         ident;
    NArray*                         array;
    NVarDecl*                       vardecl;
    NFuncDef*                       func_def;
    int                             token;

#line 173 "parser.cpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_HPP_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   228

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  170

#define YYUNDEFTOK  2
#define YYMAXUTOK   275

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,     2,     2,    35,     2,     2,
      28,    29,    33,    30,    21,    31,     2,    34,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    23,
      36,    24,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    22,     2,    25,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    26,     2,    27,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    50,    50,    54,    59,    63,    68,    69,    72,    84,
      88,    94,   103,   115,   121,   124,   127,   132,   136,   142,
     153,   157,   163,   167,   174,   177,   180,   185,   189,   195,
     198,   201,   204,   208,   212,   217,   222,   237,   244,   252,
     252,   262,   266,   271,   274,   278,   289,   293,   296,   299,
     302,   305,   308,   313,   316,   320,   320,   340,   343,   348,
     351,   355,   357,   360,   364,   369,   372,   375,   379,   382,
     385,   388,   391,   394,   398,   402,   408,   411,   414,   417,
     421,   424,   427,   432,   433,   437,   440,   445,   448,   453,
     456,   461,   464,   467,   470,   474,   477,   481
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "IDENT", "BREAK", "CONTINUE",
  "EQ", "NE", "GE", "LE", "IF", "ELSE", "WHILE", "RETURN", "AND", "OR",
  "CONST", "INT", "VOID", "LOW_PREC", "','", "'['", "';'", "'='", "']'",
  "'{'", "'}'", "'('", "')'", "'+'", "'-'", "'!'", "'*'", "'/'", "'%'",
  "'<'", "'>'", "$accept", "program", "comp_unit", "decl", "const_decl",
  "const_def_list", "const_def", "declarator_const", "const_init_val",
  "const_init_val_list", "var_decl", "var_def_list", "var_def", "init_val",
  "init_val_list", "func_def", "func_fparams", "func_fparam",
  "declarator_func", "block", "$@1", "block_item_list", "block_item",
  "stmt", "if_stmt", "while_stmt", "$@2", "jmp_stmt", "ret_stmt", "exp",
  "cond", "lval", "primary_exp", "unary_exp", "func_rparams", "mul_exp",
  "add_exp", "rel_exp", "eq_exp", "l_and_exp", "l_or_exp", "rel_op",
  "eq_op", "const_exp", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    44,    91,    59,    61,    93,   123,   125,    40,    41,
      43,    45,    33,    42,    47,    37,    60,    62
};
# endif

#define YYPACT_NINF -141

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-141)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -141,    19,    85,  -141,     5,    29,    57,  -141,  -141,  -141,
    -141,    73,    35,    -6,    69,  -141,    61,  -141,    92,  -141,
     100,   -12,    25,   175,    73,  -141,    40,    73,  -141,   187,
      89,    88,    39,  -141,  -141,   102,    25,    25,    25,    25,
      95,  -141,  -141,    77,   109,   112,   157,  -141,  -141,   109,
    -141,    88,    59,  -141,   166,  -141,  -141,  -141,   105,  -141,
    -141,   124,    88,   196,   125,  -141,  -141,  -141,    25,    25,
      25,    25,    25,    25,  -141,  -141,  -141,    -7,  -141,    88,
    -141,  -141,    43,     6,   115,  -141,  -141,  -141,  -141,    76,
    -141,   119,  -141,  -141,  -141,    77,    77,   175,  -141,  -141,
     187,  -141,  -141,   130,   134,   139,   135,   136,    18,    73,
    -141,  -141,  -141,    68,  -141,  -141,  -141,  -141,  -141,  -141,
     142,   101,    25,  -141,  -141,  -141,  -141,  -141,  -141,  -141,
      25,    25,  -141,   143,  -141,  -141,  -141,    25,  -141,   138,
     109,    30,    99,   159,   156,   151,  -141,   158,   145,  -141,
    -141,  -141,  -141,    25,  -141,  -141,    25,    25,    25,  -141,
    -141,   170,   109,    30,    99,   159,   145,   145,  -141,  -141
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     2,     1,     0,     0,     0,     3,     6,     7,
       4,     0,    13,    22,     0,    21,     0,    13,     0,    10,
       0,     0,     0,     0,     0,    19,     0,     0,     8,     0,
       0,     0,     0,    34,    67,    64,     0,     0,     0,     0,
      66,    68,    79,    82,    97,     0,     0,    23,    24,    61,
      20,     0,     0,     9,     0,    11,    14,    38,    35,    39,
      30,     0,     0,     0,     0,    71,    72,    73,     0,     0,
       0,     0,     0,     0,    12,    26,    28,     0,    32,     0,
      15,    18,     0,     0,     0,    33,    29,    69,    75,     0,
      65,     0,    76,    77,    78,    80,    81,     0,    25,    31,
       0,    16,    37,     0,     0,     0,     0,     0,     0,     0,
      52,    43,    47,     0,    42,    44,    48,    49,    50,    51,
       0,    66,     0,    70,    63,    27,    17,    36,    57,    58,
       0,     0,    60,     0,    40,    41,    46,     0,    74,     0,
      83,    85,    87,    89,    62,     0,    59,     0,     0,    92,
      91,    93,    94,     0,    95,    96,     0,     0,     0,    55,
      45,    53,    84,    86,    88,    90,     0,     0,    56,    54
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -141,  -141,  -141,   184,  -141,  -141,   168,    48,   -49,  -141,
    -141,  -141,   178,   -43,  -141,  -141,   182,   148,  -141,   -27,
    -141,  -141,    91,  -140,  -141,  -141,  -141,  -141,  -141,   -21,
      79,   -83,  -141,   -26,  -141,    80,   -22,    55,    63,    54,
    -141,  -141,  -141,   192
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   111,     8,    18,    19,    13,    55,    82,
       9,    14,    15,    47,    77,    10,    32,    33,    58,   112,
      84,   113,   114,   115,   116,   117,   166,   118,   119,   120,
     139,    40,    41,    42,    89,    43,    49,   141,   142,   143,
     144,   153,   156,    56
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      44,   121,    48,    76,    60,    81,    30,    44,   161,    34,
      35,    65,    66,    67,    97,    64,    22,    31,    23,     3,
      98,    34,    35,    11,    78,    48,   168,   169,    34,    35,
     121,   102,    44,    12,    36,    86,    37,    38,    39,   149,
     150,   132,    88,    92,    93,    94,    36,    91,    37,    38,
      39,   126,    99,    36,   125,    37,    38,    39,    30,    20,
      61,    16,   103,    21,   100,   121,   151,   152,    62,    51,
     101,    34,    35,   104,   105,    20,    48,    17,    44,   106,
      61,   107,   108,   121,   121,     4,   109,   133,    79,    26,
      24,   110,    25,    57,    59,   134,    36,   122,    37,    38,
      39,   138,     4,     5,     6,   123,   154,   155,   140,   140,
      69,    70,    71,    27,    59,    28,   147,    68,    34,    35,
     104,   105,    22,    68,    29,   137,   106,    83,   107,   108,
      63,   162,     4,   109,   140,   140,   140,    74,   110,    72,
      73,    59,    30,    36,   124,    37,    38,    39,    34,    35,
     104,   105,    95,    96,    90,   127,   106,   128,   107,   108,
      34,    35,   129,   130,   131,   136,   146,   148,   110,    34,
      35,    59,   158,    36,   157,    37,    38,    39,    34,    35,
     159,   160,   167,    46,    75,    36,     7,    37,    38,    39,
      34,    35,    54,    80,    36,    53,    37,    38,    39,    34,
      35,    46,    50,    36,   135,    37,    38,    39,    52,    85,
     145,   163,   165,    54,    45,    36,     0,    37,    38,    39,
     164,     0,     0,     0,    36,    87,    37,    38,    39
};

static const yytype_int16 yycheck[] =
{
      22,    84,    23,    46,    31,    54,    18,    29,   148,     3,
       4,    37,    38,    39,    21,    36,    22,    29,    24,     0,
      27,     3,     4,    18,    51,    46,   166,   167,     3,     4,
     113,    25,    54,     4,    28,    62,    30,    31,    32,     9,
      10,    23,    63,    69,    70,    71,    28,    68,    30,    31,
      32,   100,    79,    28,    97,    30,    31,    32,    18,    11,
      21,     4,    83,    28,    21,   148,    36,    37,    29,    29,
      27,     3,     4,     5,     6,    27,    97,     4,   100,    11,
      21,    13,    14,   166,   167,    17,    18,   108,    29,    28,
      21,    23,    23,     4,    26,    27,    28,    21,    30,    31,
      32,   122,    17,    18,    19,    29,     7,     8,   130,   131,
      33,    34,    35,    21,    26,    23,   137,    22,     3,     4,
       5,     6,    22,    22,    24,    24,    11,    22,    13,    14,
      28,   153,    17,    18,   156,   157,   158,    25,    23,    30,
      31,    26,    18,    28,    25,    30,    31,    32,     3,     4,
       5,     6,    72,    73,    29,    25,    11,    23,    13,    14,
       3,     4,    23,    28,    28,    23,    23,    29,    23,     3,
       4,    26,    16,    28,    15,    30,    31,    32,     3,     4,
      29,    23,    12,    26,    27,    28,     2,    30,    31,    32,
       3,     4,    26,    27,    28,    27,    30,    31,    32,     3,
       4,    26,    24,    28,   113,    30,    31,    32,    26,    61,
     131,   156,   158,    26,    22,    28,    -1,    30,    31,    32,
     157,    -1,    -1,    -1,    28,    29,    30,    31,    32
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    39,    40,     0,    17,    18,    19,    41,    42,    48,
      53,    18,     4,    45,    49,    50,     4,     4,    43,    44,
      45,    28,    22,    24,    21,    23,    28,    21,    23,    24,
      18,    29,    54,    55,     3,     4,    28,    30,    31,    32,
      69,    70,    71,    73,    74,    81,    26,    51,    67,    74,
      50,    29,    54,    44,    26,    46,    81,     4,    56,    26,
      57,    21,    29,    28,    67,    71,    71,    71,    22,    33,
      34,    35,    30,    31,    25,    27,    51,    52,    57,    29,
      27,    46,    47,    22,    58,    55,    57,    29,    67,    72,
      29,    67,    71,    71,    71,    73,    73,    21,    27,    57,
      21,    27,    25,    67,     5,     6,    11,    13,    14,    18,
      23,    41,    57,    59,    60,    61,    62,    63,    65,    66,
      67,    69,    21,    29,    25,    51,    46,    25,    23,    23,
      28,    28,    23,    67,    27,    60,    23,    24,    67,    68,
      74,    75,    76,    77,    78,    68,    23,    67,    29,     9,
      10,    36,    37,    79,     7,     8,    80,    15,    16,    29,
      23,    61,    74,    75,    76,    77,    64,    12,    61,    61
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    38,    39,    40,    40,    40,    41,    41,    42,    43,
      43,    44,    45,    45,    46,    46,    46,    47,    47,    48,
      49,    49,    50,    50,    51,    51,    51,    52,    52,    53,
      53,    53,    53,    54,    54,    55,    56,    56,    56,    58,
      57,    59,    59,    60,    60,    61,    61,    61,    61,    61,
      61,    61,    61,    62,    62,    64,    63,    65,    65,    66,
      66,    67,    68,    69,    69,    70,    70,    70,    71,    71,
      71,    71,    71,    71,    72,    72,    73,    73,    73,    73,
      74,    74,    74,    75,    75,    76,    76,    77,    77,    78,
      78,    79,    79,    79,    79,    80,    80,    81
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     0,     1,     1,     4,     3,
       1,     3,     4,     1,     1,     2,     3,     3,     1,     3,
       3,     1,     1,     3,     1,     3,     2,     3,     1,     6,
       5,     6,     5,     3,     1,     2,     4,     3,     1,     0,
       4,     2,     1,     1,     1,     4,     2,     1,     1,     1,
       1,     1,     1,     5,     7,     0,     6,     2,     2,     3,
       2,     1,     1,     4,     1,     3,     1,     1,     1,     3,
       4,     2,     2,     2,     3,     1,     3,     3,     3,     1,
       3,     3,     1,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 50 "parser.y"
    {
    prog_block = (yyvsp[0].block);
}
#line 1401 "parser.cpp"
    break;

  case 3:
#line 54 "parser.y"
    {
            (yyvsp[-1].block)->append((yyvsp[0].stmt));
            (yyval.block) = (yyvsp[-1].block);
            (yyvsp[0].vardecl)->is_global_ = true;
        }
#line 1411 "parser.cpp"
    break;

  case 4:
#line 59 "parser.y"
    {
            (yyvsp[-1].block)->append((yyvsp[0].stmt));
            (yyval.block) = (yyvsp[-1].block);
        }
#line 1420 "parser.cpp"
    break;

  case 5:
#line 63 "parser.y"
    {
            (yyval.block) = new NBlock();
        }
#line 1428 "parser.cpp"
    break;

  case 6:
#line 68 "parser.y"
    {(yyval.stmt) = (yyvsp[0].stmt);}
#line 1434 "parser.cpp"
    break;

  case 7:
#line 69 "parser.y"
    {(yyval.stmt) = (yyvsp[0].stmt);}
#line 1440 "parser.cpp"
    break;

  case 8:
#line 72 "parser.y"
    {
                (yyvsp[-2].type)->is_const_ = true;
                NVarDecl* decl = new NVarDecl((yyvsp[-2].type), (yyvsp[-1].assign_list));
                if(loop_depth > 0) {
                    printf("in loop const decl\n");
                    loop_decls.push_back(decl);
                    decl->is_in_loop_ = true;
                }
                (yyval.stmt) = decl;
            }
#line 1455 "parser.cpp"
    break;

  case 9:
#line 84 "parser.y"
    {
                    (yyvsp[-2].assign_list)->push_back((yyvsp[0].assign));
                    (yyval.assign_list) = (yyvsp[-2].assign_list);
                }
#line 1464 "parser.cpp"
    break;

  case 10:
#line 88 "parser.y"
    {
                    (yyval.assign_list) = new vector<NAssign*>();
                    (yyval.assign_list)->push_back((yyvsp[0].assign));
                }
#line 1473 "parser.cpp"
    break;

  case 11:
#line 94 "parser.y"
    {
                parse_time_symtbl->push((yyvsp[-2].ident));
                // printf("const push %s\n", $<ident>1->name_.c_str());
                (yyval.assign) = new NAssign((yyvsp[-2].ident), (yyvsp[0].expr));
                // 设置ident的值,类型
                (yyvsp[-2].ident)->setValue((yyvsp[0].expr));
            }
#line 1485 "parser.cpp"
    break;

  case 12:
#line 103 "parser.y"
    {
                    (yyval.ident) = (yyvsp[-3].ident);
                    int len = (yyvsp[-1].expr)->getValue();
                    TArray* t = dynamic_cast<TArray*>((yyval.ident)->type_.get());
                    if(t) {
                        TArray* tail = t->getTail();
                        TArray* new_type = new TArray(tail->base_type_, new NInt(len));
                        tail->base_type_ = shared_ptr<Type>(new_type);
                    } else {
                        (yyval.ident)->setType(new TArray((yyval.ident)->type_, new NInt(len)));
                    }
                }
#line 1502 "parser.cpp"
    break;

  case 13:
#line 115 "parser.y"
    {
                    (yyval.ident) = (yyvsp[0].ident);
                    (yyval.ident)->setType(new TInt());
                }
#line 1511 "parser.cpp"
    break;

  case 14:
#line 121 "parser.y"
    {
                (yyval.expr) = (yyvsp[0].expr);
            }
#line 1519 "parser.cpp"
    break;

  case 15:
#line 124 "parser.y"
    {
                (yyval.expr) = new NArray();
            }
#line 1527 "parser.cpp"
    break;

  case 16:
#line 127 "parser.y"
    {
                (yyval.expr) = (yyvsp[-1].array);
            }
#line 1535 "parser.cpp"
    break;

  case 17:
#line 132 "parser.y"
    {
                (yyvsp[-2].array)->append((yyvsp[0].expr));
                (yyval.array) = (yyvsp[-2].array);
            }
#line 1544 "parser.cpp"
    break;

  case 18:
#line 136 "parser.y"
    {
                (yyval.array) = new NArray();
                (yyval.array)->append((yyvsp[0].expr));
            }
#line 1553 "parser.cpp"
    break;

  case 19:
#line 142 "parser.y"
    {
                NVarDecl* decl = new NVarDecl((yyvsp[-2].type), (yyvsp[-1].assign_list));
                if(loop_depth > 0) {
                    printf("in loop decl\n");
                    loop_decls.push_back(decl);
                    decl->is_in_loop_ = true;
                }
                (yyval.stmt) = decl;
            }
#line 1567 "parser.cpp"
    break;

  case 20:
#line 153 "parser.y"
    {
                (yyvsp[-2].assign_list)->push_back((yyvsp[0].assign));
                (yyval.assign_list) = (yyvsp[-2].assign_list);
            }
#line 1576 "parser.cpp"
    break;

  case 21:
#line 157 "parser.y"
    {
                (yyval.assign_list) = new vector<NAssign*>();
                (yyval.assign_list)->push_back((yyvsp[0].assign));
            }
#line 1585 "parser.cpp"
    break;

  case 22:
#line 163 "parser.y"
    {
            parse_time_symtbl->push((yyvsp[0].ident));
            (yyval.assign) = new NAssign((yyvsp[0].ident));     
        }
#line 1594 "parser.cpp"
    break;

  case 23:
#line 167 "parser.y"
    {
            parse_time_symtbl->push((yyvsp[-2].ident));   
            (yyval.assign) = new NAssign((yyvsp[-2].ident), (yyvsp[0].expr));
            (yyvsp[-2].ident)->setValue((yyvsp[0].expr));
        }
#line 1604 "parser.cpp"
    break;

  case 24:
#line 174 "parser.y"
    {
            (yyval.expr) = (yyvsp[0].expr);    
        }
#line 1612 "parser.cpp"
    break;

  case 25:
#line 177 "parser.y"
    {
            (yyval.expr) = (yyvsp[-1].array);
        }
#line 1620 "parser.cpp"
    break;

  case 26:
#line 180 "parser.y"
    {
            (yyval.expr) = new NArray();
        }
#line 1628 "parser.cpp"
    break;

  case 27:
#line 185 "parser.y"
    {
            (yyvsp[-2].array)->append((yyvsp[0].expr));
            (yyval.array) = (yyvsp[-2].array);
        }
#line 1637 "parser.cpp"
    break;

  case 28:
#line 189 "parser.y"
    {
            (yyval.array) = new NArray();
            (yyval.array)->append((yyvsp[0].expr));
        }
#line 1646 "parser.cpp"
    break;

  case 29:
#line 195 "parser.y"
    {
            (yyval.stmt) = new NFuncDef((yyvsp[-5].type), (yyvsp[-4].ident), (yyvsp[-2].var_list), (yyvsp[0].block));
        }
#line 1654 "parser.cpp"
    break;

  case 30:
#line 198 "parser.y"
    {
           (yyval.stmt) = new NFuncDef((yyvsp[-4].type), (yyvsp[-3].ident), (yyvsp[0].block));
        }
#line 1662 "parser.cpp"
    break;

  case 31:
#line 201 "parser.y"
    {
            (yyval.stmt) = new NFuncDef((yyvsp[-5].type), (yyvsp[-4].ident), (yyvsp[-2].var_list), (yyvsp[0].block));
        }
#line 1670 "parser.cpp"
    break;

  case 32:
#line 204 "parser.y"
    {
            (yyval.stmt) = new NFuncDef((yyvsp[-4].type), (yyvsp[-3].ident), (yyvsp[0].block));
        }
#line 1678 "parser.cpp"
    break;

  case 33:
#line 208 "parser.y"
    {
            (yyvsp[-2].var_list)->push_back((yyvsp[0].vardecl));
            (yyval.var_list) = (yyvsp[-2].var_list);
        }
#line 1687 "parser.cpp"
    break;

  case 34:
#line 212 "parser.y"
    {
            (yyval.var_list) = new vector<NVarDecl*>();
            (yyval.var_list)->push_back((yyvsp[0].vardecl));
        }
#line 1696 "parser.cpp"
    break;

  case 35:
#line 217 "parser.y"
    {
            (yyval.vardecl) = new NVarDecl((yyvsp[-1].type), (yyvsp[0].assign_list));
        }
#line 1704 "parser.cpp"
    break;

  case 36:
#line 222 "parser.y"
    {
                (yyval.assign_list) = (yyvsp[-3].assign_list);
                /* set array type */
                NIdent* id = (yyvsp[-3].assign_list)->back()->id_;
                int len = (yyvsp[-1].expr)->getValue();
                TArray* t = dynamic_cast<TArray*>(id->type_.get());
                if(t) {
                    TArray* tail = t->getTail();
                    TArray* new_type = new TArray(tail->base_type_, new NInt(len));
                    tail->base_type_ = shared_ptr<Type>(new_type);
                } else {
                    id->setType(new TArray(id->type_, new NInt(len)));
                }
                int depth = id->type_->getDepth();
            }
#line 1724 "parser.cpp"
    break;

  case 37:
#line 237 "parser.y"
    {
                (yyval.assign_list) = (yyvsp[-2].assign_list);
                NIdent* id = (yyvsp[-2].assign_list)->back()->id_;
                /* 暂时还没管省略维度的参数类型，假设长度为1 */
                id->setType(new TArray(id->type_, new NInt(1)));
                int depth = id->type_->getDepth();
            }
#line 1736 "parser.cpp"
    break;

  case 38:
#line 244 "parser.y"
    {
                (yyval.assign_list) = new vector<NAssign*>();
                NAssign* a = new NAssign((yyvsp[0].ident));
                /*set int type*/
                (yyvsp[0].ident)->setType(new TInt());
                (yyval.assign_list)->push_back(a);
            }
#line 1748 "parser.cpp"
    break;

  case 39:
#line 252 "parser.y"
    {
            parse_time_symtbl = new HashMap(parse_time_symtbl);
        }
#line 1756 "parser.cpp"
    break;

  case 40:
#line 256 "parser.y"
    {
            (yyval.block) = (yyvsp[-1].block);
            parse_time_symtbl = parse_time_symtbl->getFather();
            parse_time_symtbl->pop();
        }
#line 1766 "parser.cpp"
    break;

  case 41:
#line 262 "parser.y"
    {
                    (yyvsp[-1].block)->append((yyvsp[0].stmt));
                    (yyval.block) = (yyvsp[-1].block);
                }
#line 1775 "parser.cpp"
    break;

  case 42:
#line 266 "parser.y"
    {
                    (yyval.block) = new NBlock();
                    (yyval.block)->append((yyvsp[0].stmt));
                }
#line 1784 "parser.cpp"
    break;

  case 43:
#line 271 "parser.y"
    {
            (yyval.stmt) = (yyvsp[0].stmt);
            }
#line 1792 "parser.cpp"
    break;

  case 44:
#line 274 "parser.y"
    {
            (yyval.stmt) = (yyvsp[0].stmt);
            }
#line 1800 "parser.cpp"
    break;

  case 45:
#line 278 "parser.y"
    {
        (yyval.stmt) = new NAssign((yyvsp[-3].ident), (yyvsp[-1].expr));
        NIdent* origin_id = parse_time_symtbl->findVar((yyvsp[-3].ident)->name_.c_str());
        if(origin_id) {
            if(origin_id->type_->isArray()) {
                origin_id->value_ = (yyvsp[-1].expr);
            } else {
                origin_id->value_ = (yyvsp[-1].expr);
            }
        }
    }
#line 1816 "parser.cpp"
    break;

  case 46:
#line 289 "parser.y"
    {
        /*doubt*/
        (yyval.stmt) = new NExprStmt((yyvsp[-1].expr));
    }
#line 1825 "parser.cpp"
    break;

  case 47:
#line 293 "parser.y"
    {
        (yyval.stmt) = (yyvsp[0].stmt);
    }
#line 1833 "parser.cpp"
    break;

  case 48:
#line 296 "parser.y"
    {
        (yyval.stmt) = (yyvsp[0].stmt);
    }
#line 1841 "parser.cpp"
    break;

  case 49:
#line 299 "parser.y"
    {
        (yyval.stmt) = (yyvsp[0].stmt);
    }
#line 1849 "parser.cpp"
    break;

  case 50:
#line 302 "parser.y"
    {
        (yyval.stmt) = (yyvsp[0].stmt);
    }
#line 1857 "parser.cpp"
    break;

  case 51:
#line 305 "parser.y"
    {
        (yyval.stmt) = (yyvsp[0].stmt);
    }
#line 1865 "parser.cpp"
    break;

  case 52:
#line 308 "parser.y"
    {
        (yyval.stmt) = new NEmptyStmt(); 
    }
#line 1873 "parser.cpp"
    break;

  case 53:
#line 313 "parser.y"
    {
            (yyval.stmt) = new NIfStmt((yyvsp[-2].expr), (yyvsp[0].block), nullptr);
        }
#line 1881 "parser.cpp"
    break;

  case 54:
#line 316 "parser.y"
    {
            (yyval.stmt) = new NIfStmt((yyvsp[-4].expr), (yyvsp[-2].block), (yyvsp[0].block));
        }
#line 1889 "parser.cpp"
    break;

  case 55:
#line 320 "parser.y"
    {
                loop_depth ++;
            }
#line 1897 "parser.cpp"
    break;

  case 56:
#line 322 "parser.y"
    {
                NWhileStmt* while_stmt = new NWhileStmt((yyvsp[-3].expr), (yyvsp[0].block));
                loop_depth --;
                if(loop_depth == 0 && loop_decls.size() > 0) {
                    NBlock* opt_while_stmt = new NBlock();
                    for(int i = 0; i < loop_decls.size(); i++) {
                        printf("put decl out\n");
                        opt_while_stmt->append(loop_decls.at(i));
                    }
                    opt_while_stmt->append(while_stmt);
                    loop_decls.resize(0);
                    (yyval.stmt) = opt_while_stmt;
                } else {
                    (yyval.stmt) = while_stmt;
                }
            }
#line 1918 "parser.cpp"
    break;

  case 57:
#line 340 "parser.y"
    {
            (yyval.stmt) = new NJmpStmt(AST_BREAK_JMP);
        }
#line 1926 "parser.cpp"
    break;

  case 58:
#line 343 "parser.y"
    {
            (yyval.stmt) = new NJmpStmt(AST_CONTINUE_JMP);
        }
#line 1934 "parser.cpp"
    break;

  case 59:
#line 348 "parser.y"
    {
            (yyval.stmt) = new NRetStmt((yyvsp[-1].expr));
        }
#line 1942 "parser.cpp"
    break;

  case 60:
#line 351 "parser.y"
    {
            (yyval.stmt) = new NRetStmt();
        }
#line 1950 "parser.cpp"
    break;

  case 61:
#line 355 "parser.y"
    {(yyval.expr) = (yyvsp[0].expr);}
#line 1956 "parser.cpp"
    break;

  case 62:
#line 357 "parser.y"
    {(yyval.expr) = (yyvsp[0].expr);}
#line 1962 "parser.cpp"
    break;

  case 63:
#line 360 "parser.y"
    {
        (yyvsp[-3].ident)->appendArray((yyvsp[-1].expr));
        (yyval.ident) = (yyvsp[-3].ident);
    }
#line 1971 "parser.cpp"
    break;

  case 64:
#line 364 "parser.y"
    {
        (yyval.ident) = (yyvsp[0].ident);
    }
#line 1979 "parser.cpp"
    break;

  case 65:
#line 369 "parser.y"
    {
        (yyval.expr) = (yyvsp[-1].expr);
    }
#line 1987 "parser.cpp"
    break;

  case 66:
#line 372 "parser.y"
    {
        (yyval.ident) = (yyvsp[0].ident);
    }
#line 1995 "parser.cpp"
    break;

  case 67:
#line 375 "parser.y"
    {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 2003 "parser.cpp"
    break;

  case 68:
#line 379 "parser.y"
    {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 2011 "parser.cpp"
    break;

  case 69:
#line 382 "parser.y"
    {
        (yyval.expr) = new NFuncCall((yyvsp[-2].ident), nullptr);
    }
#line 2019 "parser.cpp"
    break;

  case 70:
#line 385 "parser.y"
    {
        (yyval.expr) = new NFuncCall((yyvsp[-3].ident), (yyvsp[-1].expr_list));
    }
#line 2027 "parser.cpp"
    break;

  case 71:
#line 388 "parser.y"
    {
        (yyval.expr) = new NUnaryOp('+', (yyvsp[0].expr));
    }
#line 2035 "parser.cpp"
    break;

  case 72:
#line 391 "parser.y"
    {
        (yyval.expr) = new NUnaryOp('-', (yyvsp[0].expr));
    }
#line 2043 "parser.cpp"
    break;

  case 73:
#line 394 "parser.y"
    {
        (yyval.expr) = new NUnaryOp('!', (yyvsp[0].expr));
    }
#line 2051 "parser.cpp"
    break;

  case 74:
#line 398 "parser.y"
    {
                (yyvsp[-2].expr_list)->push_back((yyvsp[0].expr));
                (yyval.expr_list) = (yyvsp[-2].expr_list);
            }
#line 2060 "parser.cpp"
    break;

  case 75:
#line 402 "parser.y"
    {
                (yyval.expr_list) = new vector<NExpr*>();
                (yyval.expr_list)->push_back((yyvsp[0].expr));
            }
#line 2069 "parser.cpp"
    break;

  case 76:
#line 408 "parser.y"
    {
        (yyval.expr) = new NBinaryOp('*', (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2077 "parser.cpp"
    break;

  case 77:
#line 411 "parser.y"
    {
        (yyval.expr) = new NBinaryOp('/', (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2085 "parser.cpp"
    break;

  case 78:
#line 414 "parser.y"
    {
        (yyval.expr) = new NBinaryOp('%', (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2093 "parser.cpp"
    break;

  case 79:
#line 417 "parser.y"
    {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 2101 "parser.cpp"
    break;

  case 80:
#line 421 "parser.y"
    {
        (yyval.expr) = new NBinaryOp('+', (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2109 "parser.cpp"
    break;

  case 81:
#line 424 "parser.y"
    {
        (yyval.expr) = new NBinaryOp('-', (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2117 "parser.cpp"
    break;

  case 82:
#line 427 "parser.y"
    {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 2125 "parser.cpp"
    break;

  case 84:
#line 433 "parser.y"
    {
        (yyval.expr) = new NBinaryOp((yyvsp[-1].token), (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2133 "parser.cpp"
    break;

  case 85:
#line 437 "parser.y"
    {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 2141 "parser.cpp"
    break;

  case 86:
#line 440 "parser.y"
    {
        (yyval.expr) = new NBinaryOp((yyvsp[-1].token), (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2149 "parser.cpp"
    break;

  case 87:
#line 445 "parser.y"
    {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 2157 "parser.cpp"
    break;

  case 88:
#line 448 "parser.y"
    {
        (yyval.expr) = new NBinaryOp((yyvsp[-1].token), (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2165 "parser.cpp"
    break;

  case 89:
#line 453 "parser.y"
    {
        (yyval.expr) = (yyvsp[0].expr);
    }
#line 2173 "parser.cpp"
    break;

  case 90:
#line 456 "parser.y"
    {
        (yyval.expr) = new NBinaryOp((yyvsp[-1].token), (yyvsp[-2].expr), (yyvsp[0].expr));
    }
#line 2181 "parser.cpp"
    break;

  case 91:
#line 461 "parser.y"
    {
        (yyval.token) = (yyvsp[0].token);
    }
#line 2189 "parser.cpp"
    break;

  case 92:
#line 464 "parser.y"
    {
        (yyval.token) = (yyvsp[0].token);
    }
#line 2197 "parser.cpp"
    break;

  case 93:
#line 467 "parser.y"
    {
        (yyval.token) = (yyvsp[0].token);
    }
#line 2205 "parser.cpp"
    break;

  case 94:
#line 470 "parser.y"
    {
        (yyval.token) = (yyvsp[0].token);
    }
#line 2213 "parser.cpp"
    break;

  case 95:
#line 474 "parser.y"
    {
        (yyval.token) = (yyvsp[0].token);
    }
#line 2221 "parser.cpp"
    break;

  case 96:
#line 477 "parser.y"
    {
        (yyval.token) = (yyvsp[0].token);
    }
#line 2229 "parser.cpp"
    break;

  case 97:
#line 481 "parser.y"
    {
    (yyval.expr) = (yyvsp[0].expr);
}
#line 2237 "parser.cpp"
    break;


#line 2241 "parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 484 "parser.y"
