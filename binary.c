#include "globals.h"
#include "binary.h"
#include "symtab.h"

BinaryList binListHead = NULL;                                      // Inicio da lista de instrucoes binarias

void decimalToBinaryPrint(int number, int bitNo, FILE * binFile){   // Funcao que converte decimal em binario para print das instrucoes
    char * binary = malloc(bitNo * sizeof(char));                   // Char para representar o binario de tamanho bitsNo
    for(int i = bitNo - 1; i >= 0; i--, number = number / 2){       // Converte o numero decimal para binario, inserindo do fim da string ao comeco, dividindo o numero a ser convertido
        int bit = number % 2;                                       // Decide o nivel logico do bit atual
        binary[i] = bit + '0';                                      // Insere da direita a esquerda da sequencia de caracteres
    }
    binary[bitNo] = '\0';

    fprintf(binFile,"%s",binary);
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
    BinaryList b = binListHead;
    while(b != NULL){
        switch(b->bin.type){
            case R:
                decimalToBinaryPrint(b->bin.opCode, 5, binFile);
                decimalToBinaryPrint(b->bin.rd, 5, binFile);
                decimalToBinaryPrint(b->bin.rs, 5, binFile);
                decimalToBinaryPrint(b->bin.rt, 5, binFile);
                decimalToBinaryPrint(0, 12, binFile);
                break;
            case I:
                decimalToBinaryPrint(b->bin.opCode, 5, binFile);
                decimalToBinaryPrint(b->bin.rd, 5, binFile);
                decimalToBinaryPrint(b->bin.rs, 5, binFile);
                decimalToBinaryPrint(b->bin.IMM, 17, binFile);
                break;
            case JP:
                decimalToBinaryPrint(b->bin.opCode, 5, binFile);
                decimalToBinaryPrint(b->bin.IMM, 27, binFile);
                break;
            case O:
                decimalToBinaryPrint(b->bin.opCode, 5, binFile);
                decimalToBinaryPrint(0, 27, binFile);
                break;
        }
        fprintf(binFile,"\n");
        b = b->next;
    }
}

void binaryGen(InstructionList instListHead){                                                                               // Gerador de codigo binario
    InstructionList i = instListHead;

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

    FILE * codefile = fopen("outBinary.output", "w+");
    printBinary(codefile);
    fclose(codefile);
}