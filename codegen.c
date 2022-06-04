#include "my_c.h"

Node *code[100];
LVar *locals;
int nlabel = 1;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node)); 
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Vector *new_vec() {
	Vector *v = calloc(1, sizeof(Vector));
	v->data = malloc(sizeof(void *) * 16);
	v->capa = 16;
	v->len = 0;
	return v;
}

void vec_push(Vector *v, void *elem) {
	if(v->capa == v->len) {
		v->capa *= 2;
		v->data = realloc(v->data, sizeof(void *) * v->capa);
	}
	v->data[v->len++] = elem;
}
void program() {
  int i = 0;  
  while(!at_eof()) {
    code[i++] = stmt();
  }
  code[i] = NULL;
}

Node *assign() {
  Node *node = equality();
  if(consume("=")) node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *expr() {
  return assign();
}

Node *stmt() {
  Node *node;
  if(consume_kind(TK_RETURN)) { 
    node = calloc(1, sizeof(Node));
		node->kind = ND_RETURN;
    node->lhs = expr();	
	} else if(consume_kind(TK_IF)) {
		if(!consume("(")) error_at(token->str, "'('ではないトークンです");
		node = calloc(1, sizeof(Node));
		node->kind = ND_IF;
		node->cond = expr();
		node->label = nlabel++;
		if(!consume(")"))	error_at(token->str, "')'ではないトークンです");
		node->then = stmt();
		if(consume_kind(TK_ELSE)) node->els = stmt();
		return node;
	} else if(consume_kind(TK_WHILE)) {
		if(!consume("(")) error_at(token->str, "'('ではないトークンです");	
		node = calloc(1, sizeof(Node));
		node->kind = ND_WHILE;
		node->cond = expr();
		node->label = nlabel++;
		if(!consume(")"))	error_at(token->str, "')'ではないトークンです");
		node->then = stmt();
		return node;
	} else if(consume_kind(TK_FOR)) {
		if(!consume("(")) error_at(token->str, "'('ではないトークンです");	
		node = calloc(1, sizeof(Node));
		node->kind = ND_FOR;
		node->label = nlabel++;
		if(!consume(";")) {
			node->init = expr();
			if(!consume(";")) error_at(token->str, "';'ではないトークンです");
		}
		if(!consume(";")) {
			node->cond = expr();
			if(!consume(";"))  error_at(token->str, "';'ではないトークンです");
		}
		if(!consume(")")) {
			node->inc = expr();
			if(!consume(")"))	error_at(token->str, "')'ではないトークンです");
		} 
		node->then = stmt();
		return node;
	} else if(consume("{")) {
		node = calloc(1, sizeof(Node));
		node->kind = ND_BLOCK;
		node->stmts = new_vec();
		while(!consume("}")) {
			vec_push(node->stmts, stmt());	
		}
		return node;
	} else {
    node = expr();
  }
  if(!consume(";")) error_at(token->str, "';'ではないトークンです");
  return node;
}

Node *equality() {
  Node *node = relatinal();
  for(;;) {
    if(consume("==")) node = new_node(ND_EQ, node, relatinal()); 
    else if(consume("!=")) node = new_node(ND_NE, node, relatinal());
    else return node;
  }
}

Node *relatinal() {
  Node *node = add();
  for(;;) {
    if(consume("<")) node = new_node(ND_LT, node, add()); 
    else if(consume("<=")) node = new_node(ND_LE, node, add());
    else if(consume(">")) node = new_node(ND_LT, add(), node);
    else if(consume(">=")) node = new_node(ND_LE, add(), node);
    else return node;
  }
}

Node *add() {
  Node *node = mul();
  for(;;) {
    if(consume("+")) node = new_node(ND_ADD, node, mul()); 
    else if(consume("-")) node = new_node(ND_SUB, node, mul()); 
    else return node; 
  }
}

Node *mul() {
  Node *node = unary();
  for(;;) {
    if(consume("*")) node = new_node(ND_MUL, node, unary());
    else if(consume("/")) node = new_node(ND_DIV, node, unary());
    else return node;
  }
}

Node *unary() {
  if(consume("+")) return primary();
  if(consume("-")) return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if(consume("(")) {
    Node *node = expr();	  
		expect(")");
		return node;
  }
  Token *tok = consume_kind(TK_IDENT);
  if(tok) {
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		LVar *lvar = find_lvar(tok);
		if(lvar) {
      node->offset = lvar->offset;
		} else {
      lvar = calloc(1, sizeof(LVar));
	  	lvar->next = locals;
	  	lvar->name = tok->str;
	  	lvar->len = tok->len;
	  	if(locals == NULL) lvar->offset = 8;
	  	else lvar->offset = locals->offset + 8;
	  	node->offset = lvar->offset;
	  	locals = lvar;
		}
		return node;
  }
  return new_node_num(expect_num());
}

void gen_lval(Node *node) {
  if(node->kind != ND_LVAR) error("代入の左辺値が変数ではありません"); 
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  if(node->kind == ND_RETURN) {
    gen(node->lhs);    
		printf("  pop rax\n");
		printf("  mov rsp, rbp\n");
		printf("  pop rbp\n");
		printf("  ret\n");
		return;
  }
  switch(node->kind) {
  case ND_NUM:
  	printf("  push %d\n", node->val);
		return;
  case ND_LVAR:
		gen_lval(node);
  	printf("  pop rax\n");
		printf("  mov rax, [rax]\n");
		printf("  push rax\n");
		return;
  case ND_ASSIGN:
		gen_lval(node->lhs);
		gen(node->rhs);
		printf("  pop rdi\n");
		printf("  pop rax\n");
		printf("  mov [rax], rdi\n");
		printf("  push rdi\n");
		return;
	case ND_IF:
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .Lelse%d\n", node->label);
		gen(node->then);
		printf("  jmp .Lend%d\n", node->label);
		printf(".Lelse%d:\n", node->label);
		if(node->els) {
			gen(node->els);
		}
		printf(".Lend%d:\n", node->label);
		return;
	case ND_WHILE:
		printf(".Lbegin%d:\n", node->label);
		gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .Lend%d\n", node->label);
		gen(node->then);
		printf("  jmp .Lbegin%d\n", node->label);	
		printf(".Lend%d:\n", node->label);
		return;
	case ND_FOR:
		if(node->init) gen(node->init);
		printf(".Lbegin%d:\n", node->label);
		if(node->cond) gen(node->cond);
		printf("  pop rax\n");
		printf("  cmp rax, 0\n");
		printf("  je .Lend%d\n", node->label);
		gen(node->then);
		if(node->inc) gen(node->inc);
		printf("  jmp .Lbegin%d\n", node->label);
		printf(".Lend%d:\n", node->label);
		return;
	case ND_BLOCK:
		for(int i = 0; i < node->stmts->len; i++) {
			gen(node->stmts->data[i]);
			printf("  pop rax\n");
		}
		return;
	default: break;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");
  switch(node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
		break;
  case ND_SUB:
		printf("  sub rax, rdi\n");
		break;
  case ND_MUL:
		printf("  imul rax, rdi\n");
		break;
  case ND_DIV:
		printf("  cqo\n");
		printf("  idiv rdi\n");
		break;
  case ND_EQ:
		printf("  cmp rax, rdi\n");
		printf("  sete al\n");
		printf("  movzb rax, al\n");
		break;
  case ND_NE:
		printf("  cmp rax, rdi\n");
		printf("  setne al\n");
		printf("  movzb rax, al\n");
		break;
  case ND_LT:
		printf("  cmp rax, rdi\n");
		printf("  setl al\n");
		printf("  movzb rax, al\n");
		break;
  case ND_LE:
		printf("  cmp rax, rdi\n");
		printf("  setle al\n");
		printf("  movzb rax, al\n");
		break;
  default: break;
  }
  printf("  push rax\n");
}
