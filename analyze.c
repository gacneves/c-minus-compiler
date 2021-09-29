#include "globals.h"
#include "symtab.h"
#include "analyze.h"

static void typeError(TreeNode * t, char * message){ //Escreve o erro no console
    printf("ERRO SEMANTICO: %s LINHA: %d (%s)\n",t->attr.name, t->lineno, message);
    Error = TRUE;
}

static int tmp_param = 0; // Variavel para contar os parametro das funções
static int lastFunMain = 0; // Flag para declaracao da funcao main como ultima funcao declarada

// Funcao recursiva para percorrer a arvore
static void traverse(TreeNode * t, void (* preProc) (TreeNode *), void (* postProc) (TreeNode *)){ 

    if (t != NULL){ 
        preProc(t);{ 
            int i;
            for (i=0; i < MAXCHILDREN; i++)
                traverse(t->child[i], preProc, postProc);
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

int paramCounter(TreeNode *t){ // Funcao para contar os parametros

	int tmpcount = 0;

	t = t->child[0];

	while(t != NULL){
		tmpcount++;
		t = t->sibling;
	}
	return tmpcount;
}

// Funcao faz-nada (gera pre-ordem e pos-ordem)
static void nullProc(TreeNode * t){ 
    if (t==NULL) 
        return;
    else 
        return;
}

static void insertNode( TreeNode * t){ // Funcao para insercao na tabela de simbolos
    
    switch (t->nodekind){ 

        case statementX:
            switch (t->kind.stmt){       
                case variableX:
                    if (!st_lookup(t->attr.name, t->attr.scope) && !st_lookup(t->attr.name, "global")){ // Variavel nao esta na tabela
                        if(t->type != voidX){
                            if(t->type == arrayX){
                                st_insert(t->attr.name, t->lineno, t->attr.scope, "variavel", "vetor", 0, t->attr.len); // Nova def de variavel
                            }
                            else
                                st_insert(t->attr.name, t->lineno, t->attr.scope, "variavel", "inteiro", 0, 1); // Nova def de variavel
                        }
                        else
                            typeError(t, "Variavel declarada como void");   
                    } 
                    else
                        typeError(t, "Variavel ja declarada"); 
                    break;
                
                case paramX:
                    if (!st_lookup(t->attr.name, t->attr.scope) && !st_lookup(t->attr.name, "global")){ // Variavel nao esta na tabela
                        if(t->type != voidX){
                                if(t->type == arrayX)
                                    st_insert(t->attr.name, t->lineno, t->attr.scope, "variavel", "vetor arg", 0, 2); // Nova def de variavel
                                else
                                    st_insert(t->attr.name, t->lineno, t->attr.scope, "variavel", "inteiro", 0, 1); // Nova def de variavel
                        }
                        else
                            typeError(t, "Variavel declarada como void");   
                    } 
                    else
                        typeError(t, "Variavel ja declarada"); 
                    break;
                
                case functionX:
                    
                    tmp_param = paramCounter(t); // Conta os parametros da funcao
                    if (!st_lookup(t->attr.name, t->attr.scope) && !st_lookup(t->attr.name, "global") && lastFunMain == 0){ // Funcao nao esta na tabela e main ainda nao inserida
                        if(strcmp(t->attr.name, "main") == 0) 
                            lastFunMain = 1;
                        if(t->type == integerX)
                            st_insert(t->attr.name,t->lineno, t->attr.scope,"funcao", "inteiro", tmp_param, 1); // Nova def func int
                        else
                            st_insert(t->attr.name,t->lineno, t->attr.scope,"funcao", "void", tmp_param, 1); // Nova def func void
                    }
                    else{
                        if(lastFunMain == 1)
                           typeError(t, "Funcao main deve ser a ultima a ser declarada");
                        else 
                        typeError(t, "Funcao ja declarada");
                    }
                    break;

                case callX:
                    // Garante que nao ocorra erro de funcao nao declarada para a funcao output e input e verifica se outras funcoes nao foram declaradas
                    if ((strcmp(t->attr.name, "input") != 0 && strcmp(t->attr.name, "output") != 0) && (!st_lookup(t->attr.name, t->attr.scope) && !st_lookup(t->attr.name, "global")))
                        typeError(t, "Funcao nao declarada");
                    else
                        st_insert(t->attr.name,t->lineno, t->attr.scope, "chamada", "-------", paramCounter(t), 1); // Insere chamada de funcao na tabela
                    break;

                case returnX: // Retorno
                    break;
                default:
                    break;
            }
            break;
      
        case expressionX:
            switch (t->kind.exp){ 
                case idX:
                    if (!st_lookup(t->attr.name, t->attr.scope) && !st_lookup(t->attr.name, "global")) // Id nao esta na tabela
                        typeError(t,"Identificador nao declarado");
                    else{
                        if(t->type == arrayX)
                            st_insert(t->attr.name, t->lineno, t->attr.scope, "variavel", "vetor", 0, 1); // Insere id na tabela
                        else
                            st_insert(t->attr.name, t->lineno, t->attr.scope, "variavel", "inteiro", 0, 1); // Insere id na tabela
                    }
                    break;
              
                case vectorX:
                    if (!st_lookup(t->attr.name, t->attr.scope) && !st_lookup(t->attr.name, "global"))// Vetor nao esta na tabela
                        typeError(t,"Vetor nao declarado");
                    else{
                        if(t->child[0]->kind.exp == constantX && st_lookup(t->attr.name,"global")){
                            if (t->child[0]->attr.val >= st_lookup_size(t->attr.name,"global"))
                                typeError(t,"Uso de indice de vetor fora do intervalo da declaracao");
                        }
                        else if(t->child[0]->kind.exp == constantX){
                            if (t->child[0]->attr.val >= st_lookup_size(t->attr.name,t->attr.scope) && strcmp(st_lookup_tp(t->attr.name,t->attr.scope),"vetor arg") != 0)
                                typeError(t,"Uso de indice de vetor fora do intervalo da declaracao");
                        }
                        st_insert(t->attr.name,t->lineno, t->attr.scope, "variavel", "vetor", 0, 1); // Insere vetor na tabela
                    }
                    break;
              
                case typeX:
                    break;
            
                default:
                    break;
            }
        break;
    
        default:
            break;
    }
}

void buildSymtab(TreeNode * syntaxTree){    // Cria tabela de simbolos
    traverse(syntaxTree, insertNode, nullProc); 
    if(!st_lookup("main", "global")){
        printf("ERRO SEMANTICO: Funcao main nao declarada.");
        Error = TRUE; 
    }

    printSymTab();
}

static void checkNode(TreeNode * t){ // Checa os tipos
    switch (t->nodekind){ 
        case statementX:
            switch (t->kind.stmt){ 
                case ifX:
                    if (t->child[0]->type == integerX && t->child[1]->type == integerX) // Comparacao invalida if
                        typeError(t->child[0],"\'If\' Compararacao nao booleana");
                    break;
                
                case whileX:
                    if (t->child[0]->type == integerX && t->child[1]->type == integerX) // Comparacao invalida while
                        typeError(t->child[0],"\'While\' Compararacao nao booleana");
                    break;
                
                case variableX:
                    if (t->type == arrayX && t->child[0] == NULL && t->attr.len == 0)
                        typeError(t,"Uso de vetor sem indexação");
                    else if (st_lookup(t->attr.name, "global")){ // Variavel declarada no escopo global com conflitos no uso
                        if(t->type == integerX && strcmp(st_lookup_tp(t->attr.name, "global"), "vetor") == 0)
                            typeError(t,"Conflito da declaracao e uso: Variavel declarada como vetor mas sendo usada como inteiro");
                        else if (t->type == arrayX && strcmp(st_lookup_tp(t->attr.name, "global"), "inteiro") == 0)
                            typeError(t,"Conflito da declaracao e uso: Variavel declarada como inteiro mas sendo usada como vetor");
                    }
                    break;

                case assignX:
                    if (t->child[0]->type == voidX || t->child[1]->type == voidX) // Atribuir variável void
                        typeError(t->child[0],"Atribuicao invalida de dado");
                    else if(t->child[1]->kind.stmt == callX && strcmp(t->child[1]->attr.name, "input") != 0){ // Funcao retorna void
                        if(strcmp(st_lookup_tp(t->child[1]->attr.name, "global"), "inteiro"))
                            typeError(t->child[1],"Atribuicao invalida de dado: Funcao do tipo void");
                    }
                    break;
                    
                case callX:
                    if(strcmp(t->attr.name, "input") == 0 && st_lookup_paramQt(t->attr.name, t->attr.scope) != 0){ // Erro input com parametro
                        typeError(t, "Quantidade de parametros diferente da definicao");
                    }
                    else if(strcmp(t->attr.name, "output") == 0 && st_lookup_paramQt(t->attr.name, t->attr.scope) != 1) { // Erro output com parametro conflitante
                        typeError(t, "Quantidade de parametros diferente da definicao");
                    }
			        else if( (strcmp(t->attr.name, "input") != 0 && strcmp(t -> attr.name, "output") != 0) && (st_lookup_paramQt(t->attr.name, t->attr.scope) != st_lookup_paramQt(t->attr.name, "global"))){ // Erro funcao com parametro diferente da declaracao
				        typeError(t, "Quantidade de parametros diferente da definicao");
			        }
		            break;
                
                default:
                    break;
            }
            break;
        
        default:
            break;
    }
}

void typeCheck(TreeNode * syntaxTree){ // Checa os tipos na arvore de sintaxe
    traverse(syntaxTree, nullProc, checkNode);
}
