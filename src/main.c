#include "globals.h"
#include "util.h"
#include "parse.h"
#include "analyze.h"
#include "cgen.h"
#include "assembly.h"
#include "binary.h"

FILE * yyin;

int Error = FALSE;

TreeNode *syntaxTree;
int numlinha = 1;
int iniciolinha = 1;

TokenType getToken();
void Scanner();

int compilationProcess(FILE * f){
    yyin = f;

    printf("\nRealizando Scanner...\n");
    Scanner();
    if(Error) return 0;
    printf("\nScanner concluido.\n");

    rewind(yyin);
    rewind(f);
    numlinha = 1;
    iniciolinha = 1;

    printf("\nGerando Arvore Sintatica...\n");
    syntaxTree = parse();
    if(Error) return 0;
    printf("\nArvore Sintatica gerada com sucesso, salva em OutParser.\n");

    printf("\nConstruindo Tabela de Simbolos...\n");
    buildSymtab(syntaxTree);
    if(Error) return 0;
    printf("\nTabela de Simbolos construida com sucesso, salva em OutAnalyze.\n");

    printf("\nChecando semantica...\n");
    typeCheck(syntaxTree);
    if(Error) return 0;
    printf("\nCodigo semanticamente correto.\n");

    QuadList quadList;
    InstructionList assemblyList;
    char * tp = "BIOS"; 
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
                
    return 1;
}

int main()
{
    char filename[100];
    FILE * bios_file, * os_file, * proc_file;
    int ret;
    
    printf("\nC- Compiler for MIPS based processor");
    printf("\n------------------------------------------------------");
    printf("\nFor more information about the hardware specifications, see README");
    printf("\n\nAuthor: Gabriel Angelo Cabral Neves");
    printf("\n------------------------------------------------------\n");

    do{
        printf("\nEnter BIOS file: ");
        do{
            scanf("%s", filename);
            bios_file = fopen(filename, "r");
            if(bios_file == NULL){
                printf("\nFile not found! Please enter a valid one");
                printf("\nRe-enter BIOS file: ");
            }
        }while(bios_file == NULL);

        printf("\nSuccessfully opened BIOS file!");

        printf("\nStarting compilation process for BIOS...");

        ret = compilationProcess(bios_file);
    }while(!ret);
    fclose(bios_file);

    return 0;
}