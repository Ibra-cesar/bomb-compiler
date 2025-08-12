%{
#include "../../include/ast.h"
#include "../../include/lexer.h" 
#include <stdio.h>
#include <stdlib.h>

/* External var & func*/
extern int yylex(void);
extern int yyparse(void);
extern char* yytext;
extern int yylineno;
extern char *inp_buff;

void yyerror(const char *s);

/*AST root*/
AST *root;
%}

/* 
* semantic union
*/
%union {
    char* str;
    AST *ast_node;
    ASTList *ast_list;
    TypeInfo *type_info;
    BinaryOperator  bin_op;
    UnaryOperator   un_op;
}

/* 
* tokens.
*/
%token <str> TOKEN_IDENTIFIER TOKEN_INT_LIT TOKEN_FLOAT_LIT
%token TOKEN_KEYWORD_FN TOKEN_KEYWORD_INT TOKEN_KEYWORD_FLOAT TOKEN_KEYWORD_RETURN
%token TOKEN_OPERATOR_PLUS TOKEN_OPERATOR_MINUS TOKEN_OPERATOR_MULTIPLY TOKEN_OPERATOR_DIVIDE
%token TOKEN_OPERATOR_ASSIGN
%token TOKEN_OP_EQ TOKEN_OP_NE TOKEN_OP_LT TOKEN_OP_GT TOKEN_OP_LE TOKEN_OP_GE /* Comparison Ops */
%token TOKEN_PAREN_L TOKEN_PAREN_R TOKEN_BRACKET_L TOKEN_BRACKET_R
%token TOKEN_COLON TOKEN_SEMICOLON TOKEN_COMMA
%token TOKEN_EOF 

/* 
* types for all non-terminal that produce a value and the union ty[pe.
 */
%type <ast_node> Program FunctionDecl Block Statement VarDecl Assignment ReturnStmt ExpressionStmt
%type <ast_node> Expression Comparison Addition Term Factor FunctionCall Identifier
%type <ast_list> FunctionDeclList ParamListOpt ParamList StatementList ArgListOpt ArgList
%type <ast_node> Param 
%type <type_info> Type

/* 
* operator precedence and associativity.
 */
%left TOKEN_OP_EQ TOKEN_OP_NE
%left TOKEN_OP_LT TOKEN_OP_GT TOKEN_OP_LE TOKEN_OP_GE
%left TOKEN_OPERATOR_PLUS TOKEN_OPERATOR_MINUS
%left TOKEN_OPERATOR_MULTIPLY TOKEN_OPERATOR_DIVIDE
%right UMINUS /* For unary minus */

/* starting of the grammar */
%start Start

%%

/*********************************
 * Top-Level Rules
 *********************************/
Start:
    Program TOKEN_EOF { return 0; }
    ;

Program:
    FunctionDeclList { root = ast_program($1); }
    | { root = ast_program(NULL); }
    ;

FunctionDeclList:
    /* The base case: a single function */
    FunctionDecl { $$ = make_function_list($1, NULL); }
    /* The recursive case: add a function to an existing list */
    | FunctionDeclList FunctionDecl { $$ = append_function($1, $2); }
    ;

FunctionDecl:
    /* Example syntax: fn main(a: int, b: float): int { ... } */
    TOKEN_KEYWORD_FN Identifier TOKEN_PAREN_L ParamListOpt TOKEN_PAREN_R TOKEN_COLON Type Block
    {
        // $2 is the Identifier AST node
        $$ = ast_function_decl($2->data.identifier.name, $4, $7, $8);
        ast_free($2); // Free the temporary identifier node
    }
    ;

/*********************************
 * Parameter and Type Rules
 *********************************/

ParamListOpt:
    /* A list of parameters is optional */
    ParamList   { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;

ParamList:
    Param { $$ = make_param_list($1, NULL); }
    | ParamList TOKEN_COMMA Param { $$ = append_param($1, $3); }
    ;

Param:
    /* A parameter is a name and a type, e.g., "x: int" */
    Identifier TOKEN_COLON Type 
    { 
        $$ = ast_param($1->data.identifier.name, $3);
        ast_free($1); // Free the temporary identifier node
    }
    ;

Type:
    TOKEN_KEYWORD_INT   { $$ = ast_type_int(); }
    | TOKEN_KEYWORD_FLOAT { $$ = ast_type_float(); }
    | Identifier          { $$ = ast_type_custom($1->data.identifier.name); free($1); }
    ;

/*********************************
 * Statement Rules
 *********************************/

Block:
    TOKEN_BRACKET_L StatementList TOKEN_BRACKET_R { $$ = ast_block($2); }
    | TOKEN_BRACKET_L TOKEN_BRACKET_R             { $$ = ast_block(NULL); }
    ;

StatementList:
    Statement { $$ = make_statement_list($1, NULL); }
    | StatementList Statement { $$ = append_statement($1, $2); }
    ;

Statement:
    VarDecl         { $$ = $1; }
    | Assignment      { $$ = $1; }
    | ReturnStmt      { $$ = $1; }
    | ExpressionStmt  { $$ = $1; }
    | Block           { $$ = $1; }
    ;

VarDecl:
    /* Declaration with initialization: x: int = 5; */
    Identifier TOKEN_COLON Type TOKEN_OPERATOR_ASSIGN Expression TOKEN_SEMICOLON
    { 
        $$ = ast_var_decl($1->data.identifier.name, $3, $5); 
        ast_free($1);
    }
    /* Declaration without initialization: y: float; */
    | Identifier TOKEN_COLON Type TOKEN_SEMICOLON
    { 
        $$ = ast_var_decl($1->data.identifier.name, $3, NULL); 
        ast_free($1);
    }
    ;

Assignment:
    /* Assignment to an existing variable: x = 10; */
    Identifier TOKEN_OPERATOR_ASSIGN Expression TOKEN_SEMICOLON
    { 
        $$ = ast_assignment($1->data.identifier.name, $3);
        ast_free($1);
    }
    ;

ReturnStmt:
    TOKEN_KEYWORD_RETURN Expression TOKEN_SEMICOLON { $$ = ast_return($2); }
    | TOKEN_KEYWORD_RETURN TOKEN_SEMICOLON           { $$ = ast_return(NULL); }
    ;

ExpressionStmt:
    /* An expression used for its side-effects, e.g., a function call */
    Expression TOKEN_SEMICOLON { $$ = ast_expression_stmt($1); }
    ;

/*********************************
 * Expression Rules (with precedence)
 *********************************/

Expression:
    Comparison { $$ = $1; }
    ;

Comparison:
    Addition { $$ = $1; }
    | Comparison TOKEN_OP_EQ Addition { $$ = ast_binary_op_enum(BINOP_EQ, $1, $3); }
    | Comparison TOKEN_OP_NE Addition { $$ = ast_binary_op_enum(BINOP_NE, $1, $3); }
    | Comparison TOKEN_OP_LT Addition { $$ = ast_binary_op_enum(BINOP_LT, $1, $3); }
    | Comparison TOKEN_OP_GT Addition { $$ = ast_binary_op_enum(BINOP_GT, $1, $3); }
    | Comparison TOKEN_OP_LE Addition { $$ = ast_binary_op_enum(BINOP_LE, $1, $3); }
    | Comparison TOKEN_OP_GE Addition { $$ = ast_binary_op_enum(BINOP_GE, $1, $3); }
    ;

Addition:
    Term { $$ = $1; }
    | Addition TOKEN_OPERATOR_PLUS Term  { $$ = ast_binary_op_enum(BINOP_ADD, $1, $3); }
    | Addition TOKEN_OPERATOR_MINUS Term { $$ = ast_binary_op_enum(BINOP_SUB, $1, $3); }
    ;

Term:
    Factor { $$ = $1; }
    | Term TOKEN_OPERATOR_MULTIPLY Factor { $$ = ast_binary_op_enum(BINOP_MUL, $1, $3); }
    | Term TOKEN_OPERATOR_DIVIDE Factor   { $$ = ast_binary_op_enum(BINOP_DIV, $1, $3); }
    ;

Factor:
    TOKEN_INT_LIT   { $$ = ast_number($1); }
    | TOKEN_FLOAT_LIT { $$ = ast_float($1); }
    | Identifier      { $$ = $1; }
    | FunctionCall    { $$ = $1; }
    | TOKEN_PAREN_L Expression TOKEN_PAREN_R { $$ = $2; }
    | TOKEN_OPERATOR_MINUS Factor %prec UMINUS { $$ = ast_unary_op_enum(UNOP_MINUS, $2); }
    ;

Identifier:
    TOKEN_IDENTIFIER { $$ = ast_identifier($1); }
    ;

/*********************************
 * Function Call Rules
 *********************************/

FunctionCall:
    Identifier TOKEN_PAREN_L ArgListOpt TOKEN_PAREN_R
    {
        $$ = ast_function_call($1->data.identifier.name, $3);
        ast_free($1);
    }
    ;

ArgListOpt:
    ArgList     { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;

ArgList:
    Expression { $$ = make_expr_list($1, NULL); }
    | ArgList TOKEN_COMMA Expression { $$ = append_expr($1, $3); }
    ;

%%

/* Global variables for lexer integration */

/* Error handling function */
void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}

