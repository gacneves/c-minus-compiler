#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "parser.tab.h"

#define ENDF 0
#define ERRO2 -1

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 6 // Num palavras reservadas

extern int numlinha; // Variavel auxiliar com a linha atual
extern int iniciolinha; // Variavel auxiliar do scanner para verificacao de comentarios

int getToken();

// Definicoes
typedef int TokenType;

typedef enum{
	statementX, expressionX 
} NodeKind;

typedef enum{
	ifX, whileX, assignX, variableX, functionX, callX, returnX, paramX
} StatementKind;

typedef enum{
	operationX, constantX, idX, vectorX, typeX
} ExpressionIdentifier;

typedef enum{
	voidX, integerX, arrayX, booleanX
} ExpressionType;

#define MAXCHILDREN 3

typedef struct treeNode{ 
	struct treeNode * child[MAXCHILDREN];
   struct treeNode * sibling;
   int lineno;
   NodeKind nodekind;

   union { 
		StatementKind stmt; 
      ExpressionIdentifier exp;
   } kind;

   struct { 
	   TokenType op;
      int val;
      int len;
      char* name; 
      char* scope;	
   } attr;

   ExpressionType type;
} TreeNode;

extern int Error; 

#endif