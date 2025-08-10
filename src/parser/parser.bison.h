/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_IDENTIFIER = 258,
     TOKEN_INT_LIT = 259,
     TOKEN_FLOAT_LIT = 260,
     TOKEN_KEYWORD_FN = 261,
     TOKEN_KEYWORD_INT = 262,
     TOKEN_KEYWORD_FLOAT = 263,
     TOKEN_KEYWORD_RETURN = 264,
     TOKEN_OPERATOR_PLUS = 265,
     TOKEN_OPERATOR_MINUS = 266,
     TOKEN_OPERATOR_MULTIPLY = 267,
     TOKEN_OPERATOR_DIVIDE = 268,
     TOKEN_OPERATOR_ASSIGN = 269,
     TOKEN_OP_EQ = 270,
     TOKEN_OP_NE = 271,
     TOKEN_OP_LT = 272,
     TOKEN_OP_GT = 273,
     TOKEN_OP_LE = 274,
     TOKEN_OP_GE = 275,
     TOKEN_PAREN_L = 276,
     TOKEN_PAREN_R = 277,
     TOKEN_BRACKET_L = 278,
     TOKEN_BRACKET_R = 279,
     TOKEN_COLON = 280,
     TOKEN_SEMICOLON = 281,
     TOKEN_COMMA = 282,
     TOKEN_EOF = 283,
     UMINUS = 284
   };
#endif
/* Tokens.  */
#define TOKEN_IDENTIFIER 258
#define TOKEN_INT_LIT 259
#define TOKEN_FLOAT_LIT 260
#define TOKEN_KEYWORD_FN 261
#define TOKEN_KEYWORD_INT 262
#define TOKEN_KEYWORD_FLOAT 263
#define TOKEN_KEYWORD_RETURN 264
#define TOKEN_OPERATOR_PLUS 265
#define TOKEN_OPERATOR_MINUS 266
#define TOKEN_OPERATOR_MULTIPLY 267
#define TOKEN_OPERATOR_DIVIDE 268
#define TOKEN_OPERATOR_ASSIGN 269
#define TOKEN_OP_EQ 270
#define TOKEN_OP_NE 271
#define TOKEN_OP_LT 272
#define TOKEN_OP_GT 273
#define TOKEN_OP_LE 274
#define TOKEN_OP_GE 275
#define TOKEN_PAREN_L 276
#define TOKEN_PAREN_R 277
#define TOKEN_BRACKET_L 278
#define TOKEN_BRACKET_R 279
#define TOKEN_COLON 280
#define TOKEN_SEMICOLON 281
#define TOKEN_COMMA 282
#define TOKEN_EOF 283
#define UMINUS 284




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 23 "src/parser/parser.y"
{
    char* str;
    AST *ast_node;
    ASTList *ast_list;
    TypeInfo *type_info;
    BinaryOperator  bin_op;
    UnaryOperator   un_op;
}
/* Line 1529 of yacc.c.  */
#line 116 "src/parser/parser.bison.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

