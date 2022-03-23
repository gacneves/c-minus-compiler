#include "globals.h"
#include "binary.h"
#include "symtab.h"

BinaryList binListHead;                                      // Inicio da lista de instrucoes binarias
int opCode;
char * dstModule;
char * compilingName;

void decimalToBinaryPrint(int i, int number, int bitNo, FILE * binFile){   // Funcao que converte decimal em binario para print das instrucoes
    char * binary = malloc(bitNo * sizeof(char));                   // Char para representar o binario de tamanho bitsNo
    for(int i = bitNo - 1; i >= 0; i--, number = number / 2){       // Converte o numero decimal para binario, inserindo do fim da string ao comeco, dividindo o numero a ser convertido
        int bit = number % 2;                                       // Decide o nivel logico do bit atual
        binary[i] = bit + '0';                                      // Insere da direita a esquerda da sequencia de caracteres
    }
    binary[bitNo] = '\0';

    if(opCode == 1)
        fprintf(binFile,"%s[%d] = 32'b%s", dstModule, i, binary);
    else
        fprintf(binFile,"_%s", binary);
    free(binary);                                                   
}

void binaryInsert(int opCode, int rd, int rs, int rt, int IMM, Type type){      // Insere uma instrucao binaria
    BinInstruction newInst;
    newInst.opCode = opCode;
    newInst.rd = rd;
    newInst.rs = rs;
    newInst.rt = rt;
    newInst.IMM = IMM;
    newInst.type = type;
    BinaryList newBinary = (BinaryList) malloc(sizeof(struct BinaryListRec));
    newBinary->bin = newInst;
    newBinary->next = NULL;
    if(binListHead == NULL) 
        binListHead = newBinary;
    else{
        BinaryList aux = binListHead;
        while(aux->next != NULL)  aux = aux->next;
        aux->next = newBinary;
    }
}

void binaryGen_I_Type(Instruction inst){                                                    // Trata as instrucoes do tipo I
    if(inst.instKind == BEQ || inst.instKind == BNEQ){                                      // Se a instrucao usar branch para pular para algum endereco
        int instLine = searchInstLine_Label(atoi(&inst.label[1]));                          // Branch sempre para labels, logo deve-se procurar a linha de instrucao referente a ele
        binaryInsert(inst.instKind, inst.rd, inst.rs, inst.rt, instLine, inst.type);        // Insere a instrucao com as respectivas informacoes
    }
    else                                                                                    // Caso contrario
        binaryInsert(inst.instKind, inst.rd, inst.rs, inst.rt, inst.immediate, inst.type);  // Insere a instrucao com as respectivas informacoes
}

void binaryGen_J_Type(Instruction inst){                                                // Trata as instrucao do tipo J
    int instLine;                                                                       // Variavel para armazenar o numero da linha da instrucao referente a funcao ou label
    if(inst.instKind == J){                                                             // Se for a instrucao J
        if(strcmp(inst.label,"main") == 0)                                              // Unica instrucao J que leva o endereco de uma funcao, eh a segunda instrucao referente a main
            instLine = searchInstLine_Fun(inst.label);                                  // Procura o numero da linha de instrucao referente a main
        else                                                                            // Se nao for o J do main, eh um J de label
            instLine = searchInstLine_Label(atoi(&inst.label[1]));                      // Procura o numero da instrucao do label no labelMap do assembly
        binaryInsert(inst.instKind, inst.rd, inst.rs, inst.rt, instLine, inst.type);    // Insere a instrucao com as respectivas informacoes
    }
    else{                                                                               // Se for a instrucao JAL
        instLine = searchInstLine_Fun(inst.label);                                      // Procura o numero da linha de instrucao referente a funcao
        binaryInsert(inst.instKind, inst.rd, inst.rs, inst.rt, instLine, inst.type);    // Insere a instrucao com as respectivas informacoes
    }
}

void printBinary(FILE * binFile){                                               // Funcao que printa as instrucao binarias
    int i;

    if(strcmp(compilingName, "BIOS") == 0) i = 0;
    else{
        if(strcmp(compilingName, "Operating System") == 0) i = OPERATING_SYSTEM_SECTOR;
        else i = (nextAvailableTrack * SECTOR_SIZE) + NUMBER_OF_REGS_IN_CONTEXT_EXCHANGE;
    }
    opCode = 1;
    BinaryList b = binListHead;
    while(b != NULL){
        switch(b->bin.type){
            case R:
                decimalToBinaryPrint(i,b->bin.opCode, 6, binFile);
                opCode = 0;
                decimalToBinaryPrint(i,b->bin.rd, 5, binFile);
                decimalToBinaryPrint(i,b->bin.rs, 5, binFile);
                decimalToBinaryPrint(i,b->bin.rt, 5, binFile);
                decimalToBinaryPrint(i,0, 11, binFile);
                break;
            case I:
                decimalToBinaryPrint(i,b->bin.opCode, 6, binFile);
                opCode = 0;
                decimalToBinaryPrint(i,b->bin.rd, 5, binFile);
                decimalToBinaryPrint(i,b->bin.rs, 5, binFile);
                decimalToBinaryPrint(i,b->bin.IMM, 16, binFile);
                break;
            case JP:
                decimalToBinaryPrint(i,b->bin.opCode, 6, binFile);
                opCode = 0;
                decimalToBinaryPrint(i,b->bin.IMM, 26, binFile);
                break;
            case O:
                decimalToBinaryPrint(i,b->bin.opCode, 6, binFile);
                opCode = 0;
                decimalToBinaryPrint(i,0, 26, binFile);
                break;
        }
        fprintf(binFile,";\n");
        b = b->next;
        i+=1;
        opCode = 1;
    }
}

void binaryGen(InstructionList instListHead, char * setDst, char * path){                                                                               // Gerador de codigo binario
    InstructionList i = instListHead;
    binListHead = NULL;
    FILE * codefile;

    compilingName = setDst;
    if(strcmp(setDst, "BIOS") == 0){
        codefile = fopen(path, "w+");
        dstModule = "BIOS";
    }
    else{
        codefile = fopen(path, "a");
        dstModule = "HD";
        char * comment = malloc(STRING_SIZE * sizeof(char));
        strcpy(comment, "// ");
        strcat(comment, setDst);
        if(strcmp(setDst, "Process") == 0){
            strcat(comment, " ");
            char * number = malloc(STRING_SIZE * sizeof(char));
            sprintf(number, "%d", id[proc_no - 1]);
            strcat(comment, number);
        }
        strcat(comment, "\n");
        fputs(comment, codefile);
    }

    while(i != NULL){
        if(i->inst.lineKind == Inst){
            switch(i->inst.type){
                case R:
                    binaryInsert(i->inst.instKind, i->inst.rd, i->inst.rs, i->inst.rt, i->inst.immediate, i->inst.type);
                    break;
                
                case I:
                    binaryGen_I_Type(i->inst);
                    break;
                
                case JP:
                    binaryGen_J_Type(i->inst);
                    break;
                
                case O:
                    binaryInsert(i->inst.instKind, i->inst.rd, i->inst.rs, i->inst.rt, i->inst.immediate, i->inst.type);
                    break;

                default:
                    break;
            }
        }
        i = i->next;
    }

    printBinary(codefile);
    if(strcmp(setDst, "BIOS") != 0) fprintf(codefile, "\n");

    fclose(codefile);
}