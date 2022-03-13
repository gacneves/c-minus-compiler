#include "globals.h"
#include "util.h"
#include "parse.h"
#include "analyze.h"
#include "cgen.h"
#include "assembly.h"
#include "binary.h"

FILE *yyin;
FILE *source;

TreeNode *syntaxTree;
int numlinha = 1;
int iniciolinha = 1;

int Error = FALSE;

TokenType getToken();
void Scanner();

int main(int argc, char * argv[])
{
    if(argc != 3){
        printf("Uso: %s <nomeDoArquivo.extensao> <BIOS, SO ou ARQ>\n", argv[0]);
        return 0;
    }
    if(strcmp(argv[2], "BIOS") && strcmp(argv[2], "SO") && strcmp(argv[2], "ARQ")){
        printf("As unicas opcoes de modulos sao BIOS, SO ou ARQ\n");
        return 0;
    }
    source = fopen(argv[1], "r");
    if(source == NULL){
        printf("Arquivo de entrada nao encontrado\n");
        return 0;
    }
    yyin = source;

    printf("\nCompilador C-");
    printf("\n------------------------------------------------------\n");

    printf("\nRealizando Scanner...\n");
    Scanner();

    if(!Error){
        printf("\nScanner concluido.\n");

        rewind(yyin);
        rewind(source);
        numlinha = 1;
        iniciolinha = 1;

        printf("\nGerando Arvore Sintatica...\n");
        syntaxTree = parse();
    
        if (!Error){
            printf("\nArvore Sintatica gerada com sucesso, salva em OutParser.\n");

            printf("\nConstruindo Tabela de Simbolos...\n");
            buildSymtab(syntaxTree);
            if (!Error){
                printf("\nTabela de Simbolos construida com sucesso, salva em OutAnalyze.\n");
                printf("\nChecando semantica...\n");
                typeCheck(syntaxTree);

                if(!Error){
                    QuadList quadList;
                    InstructionList assemblyList;
                    char * tp = argv[2]; 

                    printf("\nCodigo semanticamente correto.\n");

                    printf("\nGerando Codigo Intermediario...\n");
                    quadList = codeGen(syntaxTree, tp);
                    printf("\nLista de Quadruplas gerada, salva em OutQuadList.\n");

                    printf("\nGerando Codigo Assembly...\n");
                    assemblyList = assemblyGen(quadList, tp);
                    printf("\nLista de Instrucoes Assembly geradas, salva em OutAssemblyList.\n");

                    printf("\nGerando Codigo Executavel...\n");
                    binaryGen(assemblyList, tp);
                    printf("\nCodigo Executavel gerado, salvo em OutBinaryList.\n");

                    printf("\n------------------------------------------------------\n");
                    printf("Compilação concluída com sucesso.\n\n");
                }
            }
        }
    }

    fclose(source);

    return 0;
}