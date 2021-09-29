%{
    #define YYPARSER
    #include "globals.h"
    #include "util.h"
    #include "parse.h"

    #define YYSTYPE TreeNode *
    static TreeNode * savedTree;
    static int yylex();

    extern int numlinha;
    extern int iniciolinha;

    extern char *yytext;
    extern int yyparse();
    extern void yyerror();
%}

%start programa
%token ERRO
%token ID NUM
%token IF INT RETURN VOID WHILE
%left IGUAL DIF
%left MEN MENE MAI MAIE
%left SOM SUBT
%left MULT DIVI
%token ATRIB
%token PVIR
%token VIR
%token AP
%token ACOL FCOL
%token ACHA FCHA

%precedence FP
%precedence ELSE

%%

programa: declaracao_lista 
            { 
                savedTree = $1; 
            };

declaracao_lista: declaracao_lista declaracao 
                    {
                        YYSTYPE t = $1; 
                        if(t!=NULL){
                            while(t->child[1] != NULL) 
                                t = t->child[1];
                            t->child[1] = $2;
                            $$ = $1;
                        }
                        else
                            $$ = $2;
                    }
                | declaracao
                    {
                        $$ = $1;
                    };

declaracao: var_declaracao
                {
                    $$ = $1;
                }
            | func_declaracao
                {
                    $$ = $1;
                };

var_declaracao: INT id PVIR
                    {
                        $$ = newExpNode(typeX);
                        $$->type = integerX; 
                        $$->attr.name = "inteiro";
                        $$->child[0] = $2;
                        $2->nodekind = statementX;
                        $2->kind.stmt = variableX;
                        $2->type = integerX;
                        $2->attr.len = 1; 
                    }
                | VOID id PVIR
                    {
                        $$ = newExpNode(typeX);
				        $$->type = voidX;
					    $$->attr.name = "void";
					    $$->child[0] = $2;
					    $2->nodekind = statementX;
					    $2->kind.stmt = variableX;
					    $2->type = voidX;
					    $2->attr.len = 1; 
                    }
                | INT id ACOL num FCOL PVIR
                    {
                        $$ = newExpNode(typeX);
                        $$->type = integerX; 
                        $$->attr.name = "vetor inteiro";
                        $$->child[0] = $2;
                        $2->nodekind = statementX;
                        $2->kind.stmt = variableX;
                        $2->type = arrayX;
                        $2->attr.len = $4->attr.val;
                    }
                | VOID id ACOL num FCOL PVIR
                    {
                        $$ = newExpNode(typeX);
						$$->type = voidX;
						$$->attr.name = "void";
						$$->child[0] = $2;
						$2->nodekind = statementX;
						$2->kind.stmt = variableX;
						$2->type = voidX;
						$2->attr.len = $4->attr.val; 
                    };

func_declaracao: INT id AP params FP composto_decl
                    {
                        $$ = newExpNode(typeX);
                        $$->type = integerX;
                        $$->attr.name = "inteiro";
                        $$->child[0] = $2;
                        $2->child[0] = $4;
                        $2->child[1] = $6;
                        $2->nodekind = statementX;
                        $2->kind.stmt = functionX;
						$2->type = integerX;
						defEscopo($2->child[0], $2->attr.name);
						defEscopo($2->child[1], $2->attr.name);
                    }

                | VOID id AP params FP composto_decl
                    {
						$$ = newExpNode(typeX);
                        $$->type = voidX;
                        $$->attr.name = "void";
                        $$->child[0] = $2;
                        $2->child[0] = $4;
                        $2->child[1] = $6;
                        $2->nodekind = statementX;
                        $2->kind.stmt = functionX;
                        $2->type = voidX;
						defEscopo($2->child[0], $2->attr.name);
						defEscopo($2->child[1], $2->attr.name);
					};
                    
params: param_lista
            {
                $$ = $1;
            }
        | VOID
            {
                $$ = $1;
            };

param_lista: param_lista VIR param
            {
                YYSTYPE t = $1;
                if (t != NULL){
                    while (t->sibling != NULL) 
                        t = t->sibling;
					t->sibling = $3;
					$$ = $1;
                }
                else
                {
                    $$ = $3;
                }
            }
        | param
            {
                $$ = $1;
            };
            
param: INT id
        {
            $$ = newExpNode(typeX);
			$2->nodekind = statementX;
            $2->kind.stmt = paramX;
            $$->type = integerX;
			$2->type = integerX; 	
            $$->attr.name = "inteiro";
            $$->child[0] = $2;    
        }
    | VOID id
        {
            $$ = newExpNode(typeX);
			$2->nodekind = statementX;
            $2->kind.stmt = paramX;
            $$->type = voidX;
			$2->type = voidX; 	
            $$->attr.name = "void";
            $$->child[0] = $2;
        }
    | INT id ACOL FCOL
        {
            $$ = newExpNode(typeX);
			$2->nodekind = statementX;
            $2->kind.stmt = paramX;
            $$->type = arrayX;
			$2->type = arrayX;
            $$->attr.name = "vetor inteiro";
            $$->child[0] = $2;
        }
    | VOID id ACOL FCOL
        {
            $$ = newExpNode(typeX);
			$2->nodekind = statementX;
            $2->kind.stmt = paramX;
            $$->type = arrayX;
            $$->attr.name = "vetor void";
            $$->child[0] = $2;
			$2->type = arrayX; 
        };
            
composto_decl: ACHA local_declaracoes statement_lista FCHA
                {
                    YYSTYPE t = $2;
					if (t != NULL){
						while (t->sibling != NULL) 
                            t = t->sibling;
						t->sibling = $3;
						$$ = $2;
					}
					else{
						$$ = $3;
					}
                };

local_declaracoes: local_declaracoes var_declaracao
                    {
                        YYSTYPE t = $1;
						if (t != NULL){
							while (t->sibling != NULL) 
                                t = t->sibling;
							t->sibling = $2;
							$$ = $1;
						}
						else{
							$$ = $2;
						}
                    }
                | /*VAZIO*/
                    {
                        $$ = NULL;
                    };

statement_lista: statement_lista statement
                    {
                        YYSTYPE t = $1;
						if (t != NULL){
							while (t->sibling != NULL) 
                                t = t->sibling;
							t->sibling = $2;
							$$ = $1;
						}
						else{
							$$ = $2;
						}
                    }
                | /*VAZIO*/
                    {
                        $$ = NULL;
                    };

statement: expressao_decl 
            {
                $$ = $1;
            }
        | composto_decl
            {
                $$ = $1;
            }
        | selecao_decl
            {
                $$ = $1;
            }
        | iteracao_decl
            {
                $$ = $1;
            }
        | retorno_decl
            {
                $$ = $1;
            };

expressao_decl: expressao PVIR
                    {
                        $$ = $1;
                    }
                | PVIR
                    ;

selecao_decl: IF AP expressao FP statement
                {
                    $$ = newStmtNode(ifX);
					$$->child[0] = $3;
					$$->child[1] = $5;
                }
            | IF AP expressao FP statement ELSE statement
                {
                    $$ = newStmtNode(ifX);
					$$->child[0] = $3;
					$$->child[1] = $5;
                    $$->child[2] = $7;
                };

iteracao_decl: WHILE AP expressao FP statement
                {
                    $$ = newStmtNode(whileX);
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                };

retorno_decl: RETURN PVIR
                {
                    $$ = newStmtNode(returnX);
                    $$->type = voidX;
                }
            | RETURN expressao PVIR
                {
                    $$ = newStmtNode(returnX);
                    $$->child[0] = $2;
                };

expressao: var ATRIB expressao
            {
                $$ = newStmtNode(assignX);
				$$->child[0] = $1;
				$$->child[1] = $3;
            }
        | simples_expressao
            {
                $$ = $1;
            };

var: id
        {
            $$ = $1;
            $$->type = integerX;
        }
    | id ACOL expressao FCOL
        {
            $$ = $1;
   		    $$->child[0] = $3;
            $$->kind.exp = vectorX;
   		    $$->type = arrayX;
        };

simples_expressao: soma_expressao relacional soma_expressao
                    {
                        $$ = $2;
                        $$->child[0] = $1;
				        $$->child[1] = $3;
                    }
                | soma_expressao
                    {
                        $$ = $1;
                    };

relacional: MENE
            {
                $$ = newExpNode(operationX);
          		$$->attr.op = MENE;
          		$$->type = booleanX;
            }
        | MEN
            {
                $$ = newExpNode(operationX);
          		$$->attr.op = MEN;
          		$$->type = booleanX;
            }
        | MAI
            {
                $$ = newExpNode(operationX);
          		$$->attr.op = MAI;
          		$$->type = booleanX;
            }
        | MAIE
            {
                $$ = newExpNode(operationX);
          		$$->attr.op = MAIE;
          		$$->type = booleanX;
            }
        | IGUAL
            {
               $$ = newExpNode(operationX);
          		$$->attr.op = IGUAL;
          		$$->type = booleanX; 
            }
        | DIF
            {
                $$ = newExpNode(operationX);
          		$$->attr.op = DIF;
          		$$->type = booleanX;
            };

soma_expressao: soma_expressao soma termo
                    {
                        $$ = $2;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                    }
            | termo
                    {
                        $$ = $1;
                    };

soma: SOM
        {
            $$ = newExpNode(operationX);
          	$$->attr.op = SOM;
        }
    | SUBT
        {
            $$ = newExpNode(operationX);
          	$$->attr.op = SUBT;
        };

termo: termo mult fator
        {
            $$ = $2;
            $$->child[0] = $1;
            $$->child[1] = $3;
        }
    | fator
        {
            $$ = $1;
        };

mult: MULT
        {
            $$ = newExpNode(operationX);
          	$$->attr.op = MULT;
        }
    | DIVI
        {
            $$ = newExpNode(operationX);
          	$$->attr.op = DIVI;
        };

fator: AP expressao FP
        {
            $$ = $2;
        }
    | var
        {
            $$ = $1;
        }
    | ativacao
        {
            $$ = $1;
        }
    | num
        {
            $$ = $1;
        };

ativacao: id AP args FP
            {
                $$ = $1;
                $$->child[0] = $3;
                $$->nodekind = statementX;
                $$->kind.stmt = callX;
            };

args: arg_lista
        {
            $$ = $1;
        }
    | /*VAZIO*/
        {
            $$ = NULL;
        };

arg_lista: arg_lista VIR expressao
            {
            YYSTYPE t = $1;
				if (t != NULL){
					while (t->sibling != NULL) 
                        t = t->sibling;
					t->sibling = $3;
					$$ = $1;
				}
				else{
					$$ = $3;
				}
            }
        | expressao
            {
                $$ = $1;
            };

id: ID
        {
            $$ = newExpNode(idX);
            $$->attr.name = copyString(yytext);
		};

num: NUM
        {
			$$ = newExpNode(constantX);
            $$->attr.val = atoi(yytext);
			$$->type = integerX;
		};

%%

void yyerror(){ // Escreve o erro no console
	extern char* yytext;
	printf("ERRO SINTATICO: %s LINHA: %d.\n", yytext, numlinha);
	Error = TRUE;
}

TreeNode * parse(void){ // Retorna a arvore criada
	yyparse();
    FILE *f_out = fopen("outParser.output", "w+");
    printTree(savedTree, f_out);
    fclose(f_out);
	return savedTree;
}

static int yylex(void){ // Pega os tokens
	TokenType t = getToken();
	return t;
}