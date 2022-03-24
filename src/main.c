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

int compilationProcess(FILE * f, char * tp){
    yyin = f;
    Error = FALSE;
    syntaxTree = NULL;

    // Check Debug folder existence
    printf("\nChecking debug folder existence...");
    if(dirExists("debug")) printf("\nFound debug folder");
    else{
        printf("\nDebug folder not found");
        printf("\nCreating debug directory...");
        if(makeDir("debug")) printf("\nCreated debug folder successfully!");
        else return 0;
    }

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
    printf("\nGerando Codigo Intermediario...");
    quadList = codeGen(syntaxTree, tp);
    printf("\nLista de Quadruplas gerada, salva em OutQuadList.");

    printf("\nGerando Codigo Assembly...");
    assemblyList = assemblyGen(quadList, tp);
    printf("\nLista de Instrucoes Assembly geradas, salva em OutAssemblyList.");

    printf("\nCompilation process information can be found in debug folder");

    // Check Output folder existence
    printf("\nChecking output folder existence...");
    if(dirExists("output")) printf("\nFound output folder");
    else{
        printf("\nOutput folder not found");
        printf("\nCreating output directory...");
        if(makeDir("output")) printf("\nCreated output folder successfully!");
        else return 0;
    }

    // Check output/tp folder existence
    char * path = malloc(STRING_SIZE * sizeof(char));
    strcpy(path, "output/");
    if(strcmp(tp, "BIOS") == 0)
        strcat(path, tp);
    else
        strcat(path, "HD");
    strcat(path, ".bin");
    printf("\nGerando Codigo Executavel...");
    binaryGen(assemblyList, tp, path);
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

        ret = compilationProcess(bios_file, "BIOS");
        fclose(bios_file);
        if(!ret){
            printf("\nBIOS compilation process failed. Please fix the error if you want to try again. cMinusCompiler will still be running");
            printf("\nRe-enter BIOS file: ");
        }
    }while(!ret);

    printf("\nBIOS compilation process finished successfully. Binary file saved in output/BIOS folder");

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

        /* Removing HD output file since the content is appended */
        remove("output/HD.bin");
        ret = compilationProcess(os_file, "Operating System");
        fclose(os_file);
        if(!ret){
            printf("\nOS compilation process failed. Please fix the error if you want to try again. cMinusCompiler will still be running");
            printf("\nRe-enter OS file: ");
        }
    }while(!ret);

    printf("\nOS compilation process finished successfully. Incrementing binary file in output/HD folder...");

    printf("\n------------------------------------------------------\n");

    printf("\nEnter the programs to store in HD (Max 15)\n");

    nextAvailableTrack = 1;
    proc_info = calloc(MAX_NO_PROGRAMS, sizeof(ProcInfo));
    proc_no = 0;
    do{
        printf("\nEnter Program file (0 to finish): ");
        do{
            do{
                scanf("%s", filename);
                if(strcmp(filename, "0") == 0) break;
                proc_file = fopen(filename, "r");
                if(proc_file == NULL){
                    printf("\nFile not found! Please enter a valid one");
                    printf("\nRe-enter Program file: ");
                }
            }while(proc_file == NULL);

            if(strcmp(filename, "0") == 0) break;
            printf("\nSuccessfully opened Program file!\n");
            
            printf("\nEnter program identifier: ");
            scanf("%d", &proc_info[proc_no].id);

            printf("\nStarting compilation process for given program...");

            ret = compilationProcess(proc_file, "Process");
            fclose(proc_file);
            if(!ret){
                printf("\nProgram compilation process failed. Please fix the error if you want to try again. cMinusCompiler will still be running");
                proc_info[proc_no].id = 0;
                proc_no -= 1;
                printf("\nRe-enter OS file: ");
            }
            else{
                proc_info[proc_no].size = program_size_count;
                proc_info[proc_no].location = nextAvailableTrack;
                proc_no += 1;
                nextAvailableTrack += 1;
            }
        }while(!ret);
        if(strcmp(filename, "0") != 0) printf("\n");
    }while(strcmp(filename, "0") != 0);

    printf("\nPrograms compilation process finished successfully! Incrementing binary file in output/HD folder...");

    printf("\n------------------------------------------------------\n");

    printf("\nCreating process table...");

    generateProcessTable("output/HD.bin");

    printf("\nProcess table created successfully! HD binary file is completed and can be found in output folder.");

    printf("\nFinish compilation process for all modules. Shutting it down.\n\n");
    
    return 0;
}