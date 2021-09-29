%option noyywrap
%option nounput

%{	
	#include "globals.h"
	#include "parser.tab.h"

    extern int numlinha;
    extern int iniciolinha;
    extern FILE *yyin;
%}

digito [0-9]
letra [a-zA-Z]
ID {letra}+
NUM {digito}+
BRANCO [ \t\r]+
QUEBRA \n

%%
{NUM}{ID} return ERRO;
{ID}{NUM} return ERRO;

"if" return IF;
"else" return ELSE;
"int" return INT;
"return" return RETURN;
"void" return VOID;
"while" return WHILE;

"+" return SOM;
"-" return SUBT;
"*" return MULT;
"/" return DIVI;
"<" return MEN;
"<=" return MENE;
">" return MAI;
">=" return MAIE;
"==" return IGUAL;
"!=" return DIF;
"=" return ATRIB;
";" return PVIR;
"," return VIR;
"(" return AP;
")" return FP;
"[" return ACOL;
"]" return FCOL;
"{" return ACHA;
"}" return FCHA;
"/*" { 
	char c;
	iniciolinha = numlinha;
	do{ 
		c = input();
		if (c == 0) {
			numlinha = iniciolinha;
            return ERRO2;
		}
		if (c == '\n')
			numlinha++;
		if (c == '*') {
			if(input() == '/')
				break;
		}
		if(c == '/'){
			if(input() == '*'){
				return ERRO;
			}
		}		
	}while (1);
	}

{ID} return ID; 
{NUM} return NUM; 
{QUEBRA} numlinha++;
{BRANCO} 

<<EOF>> return ENDF;

. return ERRO;

%%

TokenType getToken(){ // Pega os tokens
	int token;
	
	token = yylex();

	return token;
}

void Scanner(){ // Funcao scanner para gerar arquivo com tokens
    int token;
    FILE *f_out = fopen("outScanner.output","w+");
	do{
        token = yylex();
        switch(token){
            case ID:
                fprintf(f_out, "ID "); 
                break;
            case NUM:
                fprintf(f_out, "NUM "); 
                break;
            case IF:
                fprintf(f_out, "IF ");
                break;
            case ELSE:
                fprintf(f_out, "ELSE ");
                break;
            case INT:
                fprintf(f_out, "INT ");
                break;
            case RETURN:
                fprintf(f_out, "RETURN ");
                break;
            case VOID:
                fprintf(f_out, "VOID ");
                break;
            case WHILE:
                fprintf(f_out, "WHILE ");
                break;
            case SOM:
                fprintf(f_out, "SOM ");
                break;
            case SUBT:
                fprintf(f_out, "SUBT ");
                break;
            case MULT:
                fprintf(f_out, "MULT ");
                break;
            case DIVI:
                fprintf(f_out, "DIVI ");
                break;
            case MEN:
                fprintf(f_out, "MEN ");
                break;
            case MENE:
                fprintf(f_out, "MENE ");
                break;
            case MAI:
                fprintf(f_out, "MAI ");
                break;
            case MAIE:
                fprintf(f_out, "MAIE ");
                break;
            case IGUAL:
                fprintf(f_out, "IGUAL ");
                break;
            case DIF:
                fprintf(f_out, "DIF ");
                break;
            case ATRIB:
                fprintf(f_out, "ATRIB ");
                break;
            case PVIR:
                fprintf(f_out, "PVIR \n");
                break;
            case VIR:
                fprintf(f_out, "VIR ");
                break;
            case AP:
                fprintf(f_out, "AP ");
                break;
            case FP:
                fprintf(f_out, "FP ");
                break;
            case ACOL:
                fprintf(f_out, "ACOL ");
                break;
            case FCOL:
                fprintf(f_out, "FCOL ");
                break;
            case ACHA:
                fprintf(f_out, "ACHA \n");
                break;
            case FCHA:
                fprintf(f_out, "FCHA \n");
                break;
            case ENDF:
                fprintf(f_out, "ENDF ");
                break;
            case ERRO:
                fprintf(f_out, "ERRO ");
                printf("ERRO LEXICO: %s LINHA: %d\n", yytext, numlinha); // ERRO Geral
                Error = TRUE;
                break;
            case ERRO2:
                printf("ERRO LEXICO: */ LINHA: %d\n", numlinha); // ERRO Caso específico: não encontra o fecha comentário
                break;
                Error = TRUE;
            default: break;
        }
    }while(token != ENDF);
}