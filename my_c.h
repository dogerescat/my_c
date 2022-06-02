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
	TK_WHILE,
	TK_FOR,
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
	ND_WHILE,
	ND_FOR,
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
	Node *cond;
	Node *then;
	Node *els;
	Node *init;
	Node *inc;
  int val;
  int offset;
	int label;
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
extern int nlabel;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_kind(TokenKind kind);
void expect(char *op);
int expect_num();
LVar *find_lvar(Token *tok);
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
