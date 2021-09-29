#include "globals.h"
#include "util.h"

void printToken(TokenType token, const char* tokenString, FILE *listing)
{ 
  switch (token){ // Print de cada token e seu tipo
	case IF: 
    case ELSE:
    case INT: 
    case RETURN:
    case VOID: 
    case WHILE: fprintf(listing, "Palavra Reservada: %s\n",tokenString); break;
    case ATRIB: fprintf(listing,"Simbolo: =\n"); break;
    case MEN: fprintf(listing,"Simbolo: <\n"); break;
    case IGUAL: fprintf(listing,"Simbolo: ==\n"); break;
    case MAI: fprintf(listing,"Simbolo: >\n"); break;
    case MENE: fprintf(listing, "Simbolo: <=\n"); break;
    case MAIE: fprintf(listing, "Simbolo: >=\n"); break;
    case DIF: fprintf(listing, "Simbolo: !=\n"); break;
    case ACOL: fprintf(listing, "Simbolo: [\n"); break;
    case FCOL: fprintf(listing, "Simbolo: ]\n"); break;
    case ACHA: fprintf(listing, "Simbolo: {\n"); break;
    case FCHA: fprintf(listing, "Simbolo: }\n"); break;
    case AP: fprintf(listing,"Simbolo: (\n"); break;
    case FP: fprintf(listing,"Simbolo: )\n"); break;
    case PVIR: fprintf(listing,"Simbolo: ;\n"); break;
    case VIR: fprintf(listing,"Simbolo: ,\n"); break;
    case SOM: fprintf(listing,"Simbolo: +\n"); break;
    case SUBT: fprintf(listing,"Simbolo: -\n"); break;
    case MULT: fprintf(listing,"Simbolo: *\n"); break;
    case DIVI: fprintf(listing,"Simbolo: /\n"); break;
    case ENDF: fprintf(listing,"Palavra Reservada: EOF\n"); break;
    case NUM:
      fprintf(listing, "Numero: %s\n",tokenString);
      break;
    case ID:
      fprintf(listing, "ID: %s\n",tokenString);
      break;
    case ERRO:
      fprintf(listing, "ERRO: %s\n",tokenString);
      break;
    default: 
      fprintf(listing,"Token desconhecido: %d\n",token);
  }
}

void defEscopo(TreeNode* t, char* scope){ // Set dos escopos
	int i;
	while(t != NULL){
		for(i = 0; i < MAXCHILDREN; ++i){
			t->attr.scope = scope;
			defEscopo(t->child[i], scope);
		}
		t = t->sibling; 
	}
}

TreeNode * newStmtNode(StatementKind kind){ // Cria no do tipo statement
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if (t==NULL)
		fprintf(stderr,"Erro de memoria %d\n",numlinha);
	else { // Seta as informacoes do no
		for (i=0;i<MAXCHILDREN;i++) 
			t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = statementX;
		t->kind.stmt = kind;
		t->lineno = numlinha;
		t->attr.scope = "global";
	}
	return t;
}

TreeNode * newExpNode(ExpressionIdentifier kind){ // Cria no do tipo expression
	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if (t==NULL)
		fprintf(stderr,"Erro de memoria %d\n",numlinha);
	else { // Seta as informacoes do no
		for (i=0;i<MAXCHILDREN;i++) 
			t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = expressionX;
		t->kind.exp = kind;
		t->lineno = numlinha;
		t->type = VOID;
		t->attr.scope = "global";
	}
	return t;
}

char * copyString(char * s){ // Copia string
	int n;
	char * t;
	if (s==NULL) 
		return NULL;
	n = strlen(s)+1;
	t = malloc(n);
	if (t==NULL)
		fprintf(stderr,"Erro de memoria %d\n",numlinha);
	else 
		strcpy(t,s);
	return t;
}

static int indentno = 0; // Quantidade de tabs
static int primeiraLinha = 0; // Se for inicio do escopo

// Tabulacao
#define INDENT indentno+=1 
#define UNINDENT indentno-=1

static void printSpaces(FILE **f_out){ // Print dos tabs
	int i;
	for (i=0;i<indentno;i++)
		fprintf(*f_out,"\t");
}

void printTree(TreeNode * tree, FILE * f_out){ // Print da arvore gerada (recursivo)
	int i;
	if (primeiraLinha == 0)
		primeiraLinha = 1;
	else
		INDENT;
	while (tree != NULL) {
		printSpaces(&f_out);
		if (tree->nodekind==statementX)
			{ 
				switch (tree->kind.stmt) {
				case ifX:
				fprintf(f_out,"If\n");
				break;
				case assignX:
				fprintf(f_out,"Atribuicao\n");
				break;
				case whileX:
				fprintf(f_out,"While\n");
				break;
				case variableX:
				fprintf(f_out,"Variavel: %s\n", tree->attr.name);
				break;
				case functionX:
				fprintf(f_out,"Funcao: %s\n", tree->attr.name);
				break;
				case callX:
				fprintf(f_out,"Chamada de Funcao: %s \n", tree->attr.name);
				break;        
				case returnX:
				fprintf(f_out, "Return\n");
				break;
				case paramX:
				fprintf(f_out, "Parametro: %s\n", tree->attr.name);
				break;     

				default:
				fprintf(f_out,"ExpNode kind desconhecido\n");
				break;
			}
		}
		else if (tree->nodekind==expressionX)
			{ switch (tree->kind.exp) {
				case operationX:
				fprintf(f_out,"Operacao: ");
				printToken(tree->attr.op, "\0", f_out);
				break;
				case constantX:
				fprintf(f_out,"Constante: %d\n",tree->attr.val);
				break;
				case idX:
				fprintf(f_out,"ID: %s\n",tree->attr.name);
				break;
				case vectorX:
				fprintf(f_out,"Vetor: %s\n",tree->attr.name);
				break;
				case typeX:
				fprintf(f_out,"Tipo: %s\n",tree->attr.name);
				break;

				default:
				fprintf(f_out,"ExpNode kind desconhecido\n");
				break;
			}
		}
		else fprintf(f_out,"Node kind desconhecido\n");
		for (i=0;i<MAXCHILDREN;i++)
			printTree(tree->child[i],f_out);
		tree = tree->sibling;
	}
	UNINDENT;
}