#ifndef _UTIL_H_
#define _UTIL_H_

// Funcao para o print de tokens
void printToken(TokenType, const char*, FILE*);

// Cria um no do tipo statement
TreeNode * newStmtNode(StatementKind);

// Cria um no do tipo expression
TreeNode * newExpNode(ExpressionIdentifier);

// Faz uma copia da string
char * copyString( char * );

// Funcao para o print da arvore gerada
void printTree( TreeNode * , FILE *);

// Seta os escopos das declaracoes
void defEscopo(TreeNode*, char*);

// Checa se diretorio existe
int dirExists(char*);

// Cria diretorio
int makeDir(char*);

// Pesquisa as syscalls do sistema
int syscall(char *);

// Pesquisa as syscalls do sistema verificando a corretude na quantidade de parâmetros
int syscallParamQt(char *, int);

#endif