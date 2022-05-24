#include "my_c.h"

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");	
  exit(1);
}

LVar *find_lvar(Token *tok) {
  for(LVar *var = locals; var; var = var->next) {
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)) return var;
  }
  return NULL;
}
bool consume(char *op) {
  if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if(token->kind == TK_IDENT) return token;
  else return NULL;
}

void expect(char *op) {
  if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)) error_at(token->str, "expected \"%s\"", op);
  token = token->next;
}

int expect_num() {
  if(token->kind != TK_NUM) error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startSwith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
} 

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;
  while(*p) {
    if(isspace(*p)) {
	  p++;
	  continue;
	}
	if(startSwith(p, ">=") || startSwith(p, "<=") || startSwith(p, "==") || startSwith(p, "!=")) {
	  cur = new_token(TK_RESERVED, cur, p, 2);
	  p += 2;
      continue;
	}
	if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == ';') {
	  cur = new_token(TK_RESERVED, cur, p++, 1);
	  continue;
	}
	if(isdigit(*p)) {
	  cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
	  cur->val = strtol(p, &p, 10);
	  cur->len = p - q;	 
	  continue;
	}
	if('a' <= *p || 'z' <= *p) {
      cur = new_token(TK_IDENT, cur, p++, 1);
	  continue;
	}
	error("トークナイズできません");
  }
  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

