#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#include "cgen.h"

typedef enum{
    ADD, SUB, MUL, DIV, NOT, AND, OR, XOR, SLT, SGT, JR, IN, OUT, ADDI, SUBI, BEQ, BNEQ, LW, SW, SR, SL, J, JAL, NOP, HLT 
}InstructionKind;

typedef enum{   
    $zero, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11, $t12, $t13, $t14, $t15, $t16, $a0, $a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $ad, $v0, $sp, $md, $ra
}Register;
// $zero: possui sempre o valor 0
// $t: registradores temporarios
// $a: registradores de argumentos
// $ad: registrador de endereco para variaveis vetores como argumentos
// $v0: regsitrador de retorno de funcao int
// $sp: registrador de topo de pilha
// $md: resto de divisao
// $ra: endereco de retorno de chamada de funcao

typedef enum{
    R, I, JP, O
}Type;
// JP = Tipo J

typedef enum{
    Inst, Lab
}LineKind;

typedef struct{
    InstructionKind instKind;
    Register rd, rs, rt;
    int immediate, lineNo;
    Type type;
    LineKind lineKind;
    char * label;
} Instruction;

typedef struct InstructionListRec{
    Instruction inst;
    struct InstructionListRec * next; 
} * InstructionList;

typedef struct RegisterListRec{
    Register r;
    struct RegisterListRec * prev;
    struct RegisterListRec * next;
} * UnusedRegisterList;

int * labelMap;                             // Num das linhas de instrucoes referentes aos labels

InstructionList assemblyGen(QuadList head); // Gerador de codigo Assembly

int searchInstLine_Label(int labelNo);      // Procura numero da linha de instrucao referente ao label para uso na geracao de codigo binario

#endif