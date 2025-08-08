/* ast.h - Abstract Syntax Tree definitions */
#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Forward declarations */
typedef struct AST AST;
typedef struct ASTList ASTList;

/* AST Node Types */
typedef enum {
  AST_PROGRAM,
  AST_FUNCTION_DECL,
  AST_FUNCTION_LIST,
  AST_PARAM,
  AST_PARAM_LIST,
  AST_TYPE,
  AST_BLOCK,
  AST_STATEMENT_LIST,
  AST_VAR_DECL,
  AST_ASSIGNMENT,
  AST_RETURN,
  AST_EXPRESSION_STMT,
  AST_BINARY_OP,
  AST_UNARY_OP,
  AST_FUNCTION_CALL,
  AST_IDENTIFIER,
  AST_INT_LITERAL,
  AST_FLOAT_LITERAL,
  AST_EXPR_LIST
} ASTNodeType;

/* Type information */
typedef enum {
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_VOID,
  TYPE_CUSTOM /* For user-defined types */
} TypeKind;

typedef struct {
  TypeKind kind;
  char *name; /* For custom types or type names */
} TypeInfo;

/* Binary operators */
typedef enum {
  BINOP_ADD, 
  BINOP_SUB, 
  BINOP_MUL, 
  BINOP_DIV, 
  BINOP_MOD, 
  BINOP_EQ,  
  BINOP_NE,  
  BINOP_LT,  
  BINOP_GT,  
  BINOP_LE,  
  BINOP_GE   
} BinaryOperator;

/* Unary operators */
typedef enum {
  UNOP_MINUS, /* - */
  UNOP_PLUS,  /* + */
  UNOP_NOT    /* ! (if you add it later) */
} UnaryOperator;

/* Generic list structure for AST nodes */
struct ASTList {
  AST *node;
  struct ASTList *next;
};

/* Main AST node structure */
struct AST {
  ASTNodeType type;
  int line;   
  int column; 

  union {
    /* Program node */
    struct {
      ASTList *functions;
    } program;

    /* Function declaration */
    struct {
      char *name;
      ASTList *parameters;
      TypeInfo *return_type;
      AST *body;
    } function_decl;

    /* Parameter */
    struct {
      char *name;
      TypeInfo *param_type;
    } param;

    /* Type node */
    struct {
      TypeInfo *type_info;
    } type;

    /* Block statement */
    struct {
      ASTList *statements;
    } block;

    /* Variable declaration */
    struct {
      char *name;
      TypeInfo *var_type;
      AST *initializer; /* Can be NULL */
    } var_decl;

    /* Assignment */
    struct {
      char *name;
      AST *value;
    } assignment;

    /* Return statement */
    struct {
      AST *expression; /* Can be NULL for void returns */
    } return_stmt;

    /* Binary operation */
    struct {
      BinaryOperator bin_op;
      AST *left;
      AST *right;
    } binary_op;

    /* Unary operation */
    struct {
      UnaryOperator unar_op;
      AST *operand;
    } unary_op;

    /* Function call */
    struct {
      char *name;
      ASTList *arguments;
    } function_call;

    /* Identifier */
    struct {
      char *name;
    } identifier;

    /* Integer literal */
    struct {
      long long value;
      char *raw_value;     } int_literal;

    /* Float literal */
    struct {
      double value;
      char *raw_value;     } float_literal;
  } data;
};

/* AST Creation Functions */
AST *ast_create(ASTNodeType type);
AST *ast_program(ASTList *functions);
AST *ast_function_decl(char *name, ASTList *params, TypeInfo *return_type,
                       AST *body);
AST *ast_param(char *name, TypeInfo *type);
AST *ast_type(char *type_name);
TypeInfo *ast_type_int(void);
TypeInfo *ast_type_float(void);
TypeInfo *ast_type_void(void);
TypeInfo *ast_type_custom(char *name);
AST *ast_block(ASTList *statements);
AST *ast_var_decl(char *name, TypeInfo *type, AST *initializer);
AST *ast_assignment(char *name, AST *value);
AST *ast_return(AST *expression);
AST *ast_expression_stmt(AST *expression);
AST *ast_binary_op(char op_char, AST *left, AST *right);
AST *ast_binary_op_enum(BinaryOperator op, AST *left, AST *right);
AST *ast_unary_op(char op_char, AST *operand);
AST *ast_unary_op_enum(UnaryOperator op, AST *operand);
AST *ast_function_call(char *name, ASTList *arguments);
AST *ast_identifier(char *name);
AST *ast_number(char *value_str);
AST *ast_float(char *value_str);

/* List Management Functions */
ASTList *ast_list_create(AST *node);
ASTList *ast_list_append(ASTList *list, AST *node);
int ast_list_length(ASTList *list);
void ast_list_free(ASTList *list);

/* Convenience functions for specific list types */
ASTList *make_function_list(AST *function, ASTList *next);
ASTList *append_function(ASTList *list, AST *function);
ASTList *make_param_list(AST *param, ASTList *next);
ASTList *append_param(ASTList *list, AST *param);
ASTList *make_statement_list(AST *statement, ASTList *next);
ASTList *append_statement(ASTList *list, AST *statement);
ASTList *make_expr_list(AST *expression, ASTList *next);
ASTList *append_expr(ASTList *list, AST *expression);

/* Memory Management */
void ast_free(AST *node);
void ast_free_recursive(AST *node);
TypeInfo *type_info_create(TypeKind kind, char *name);
void type_info_free(TypeInfo *type);

/* Utility Functions */
char *ast_node_type_string(ASTNodeType type);
char *binary_operator_string(BinaryOperator op);
char *unary_operator_string(UnaryOperator op);
char *type_kind_string(TypeKind kind);

/* Tree Traversal and Printing */
void ast_print(AST *node, int indent);
void ast_print_list(ASTList *list, int indent, const char *separator);
void print_ast(AST *node, int indent); /* Alias for compatibility */

/* AST Analysis Functions */
bool ast_is_expression(AST *node);
bool ast_is_statement(AST *node);
bool ast_is_declaration(AST *node);
TypeInfo *ast_get_expression_type(AST *node); /* For type checking */

/* Error Reporting */
void ast_error(AST *node, const char *message, ...);
void ast_warning(AST *node, const char *message, ...);

/* Debugging Support */
#ifdef DEBUG_AST
#define AST_DEBUG(fmt, ...) printf("[AST DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define AST_DEBUG(fmt, ...) ((void)0)
#endif

/* Constants */
#define MAX_IDENTIFIER_LENGTH 256
#define MAX_NUMBER_LENGTH 64

#endif /* AST_H */
