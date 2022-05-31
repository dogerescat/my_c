#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_IDENT,
	TK_IF,
	TK_ELSE,
  TK_RETURN,
  TK_EOF,
} TokenKind;

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_ASSIGN,
  ND_LVAR,
  ND_EQ,
  ND_NE,
  ND_LT,
  ND_LE,
  ND_NUM,
  ND_RETURN,
	ND_IF,
	ND_ELSE,
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
};

typedef struct Token Token;
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

typedef struct LVar LVar;
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

extern char *user_input;
extern Token *token;
extern Node *code[100];
extern LVar *locals;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_num();
LVar *find_lvar(Token *tok);
Token *consume_ident();
Token *consume_return();
Token *consume_if();
Token *consume_else();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startSwith(char *p, char *q);
Token *tokenize(char *p);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void program();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relatinal();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);
