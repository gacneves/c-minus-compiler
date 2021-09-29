#ifndef _ANALYZE_H_
#define _ANALYZE_H_

void buildSymtab(TreeNode *); // Cria tabela de simbolos

void typeCheck(TreeNode *);  // Checa os tipos na arvore de sintaxe

int paramCounter(TreeNode *t); // Conta a quantidade de parametros

#endif
