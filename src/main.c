#include "globals.h"
#include "util.h"
#include "parse.h"
#include "analyze.h"
#include "cgen.h"
#include "assembly.h"
#include "binary.h"

FILE * yyin;

int Error;

TreeNode *syntaxTree;
int numlinha = 1;
int iniciolinha = 1;

TokenType getToken();
void Scanner();

int compilationProcess(FILE * f){
    yyin = f;

    Error = FALSE;

    printf("\nRealizando Scanner...");
    Scanner();
    if(Error) return 0;
    printf("\nScanner concluido.");

    rewind(yyin);
    rewind(f);
    numlinha = 1;
    iniciolinha = 1;

    printf("\nGerando Arvore Sintatica...");
    syntaxTree = parse();
    if(Error) return 0;
    printf("\nArvore Sintatica gerada com sucesso, salva em OutParser.");

    printf("\nConstruindo Tabela de Simbolos...");
    buildSymtab(syntaxTree);
    if(Error) return 0;
    printf("\nTabela de Simbolos construida com sucesso, salva em OutAnalyze.");

    printf("\nChecando semantica...");
    typeCheck(syntaxTree);
    if(Error) return 0;
    printf("\nCodigo semanticamente correto.");

    QuadList quadList;
    InstructionList assemblyList;
    char * tp = "BIOS"; 
    printf("\nGerando Codigo Intermediario...");
    quadList = codeGen(syntaxTree, tp);
    printf("\nLista de Quadruplas gerada, salva em OutQuadList.");

    printf("\nGerando Codigo Assembly...");
    assemblyList = assemblyGen(quadList, tp);
    printf("\nLista de Instrucoes Assembly geradas, salva em OutAssemblyList.");

    printf("\nGerando Codigo Executavel...");
    binaryGen(assemblyList, tp);
    printf("\nCodigo Executavel gerado, salvo em OutBinaryList.");
                
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
    printf("\nAuthor: Gabriel Angelo Cabral Neves");
    printf("\n------------------------------------------------------\n");

    printf("\nEnter BIOS file: ");
    do{
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
        fclose(bios_file);
        if(!ret){
            printf("\nBIOS compilation process failed. Please fix the error if you want to try again. cMinusCompiler will still be running");
            printf("\nRe-enter BIOS file: ");
        }
    }while(!ret);

    printf("\nBIOS compilation process finished successfully. Binary file saved in output_files folder");

    printf("\n------------------------------------------------------\n");

    printf("\nEnter OS file: ");
    do{
        do{
            scanf("%s", filename);
            os_file = fopen(filename, "r");
            if(os_file == NULL){
                printf("\nFile not found! Please enter a valid one");
                printf("\nRe-enter OS file: ");
            }
        }while(os_file == NULL);

        printf("\nSuccessfully opened OS file!");

        printf("\nStarting compilation process for OS...");

        ret = compilationProcess(os_file);
        fclose(os_file);
        if(!ret){
            printf("\nOS compilation process failed. Please fix the error if you want to try again. cMinusCompiler will still be running");
            printf("\nRe-enter OS file: ");
        }
    }while(!ret);

    printf("\nOS compilation process finished successfully. Waiting for programs to save binary file for HD...");

    printf("\n------------------------------------------------------\n");

    printf("Enter the programs to save in HD");

    return 0;
}