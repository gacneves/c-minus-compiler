#ifndef _ASSEMBLY_H_
#define _ASSEMBLY_H_

#include "cgen.h"

typedef enum{
    ADD, SUB, MUL, DIV, NOT, AND, OR, XOR, SLT, SGT, JR, IN, OUT, ADDI, SUBI, BEQ, BNEQ, LW, SW, SR, SL, J, JAL, NOP, HLT, FINALIZE, PREEMPON, PREEMPOFF, MEMCTRL, HDTOINST, HDTOREG, REGTOHD, LCDWRITE, SETPROGID
}InstructionKind;

typedef enum{   
    $zero, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $a0, $a1, $a2, $a3, $a4, $ad, $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, $c0, $c1, $c2, $id, $bf, $sp, $v0, $md, $ra
}Register;
// $zero: possui sempre o valor 0
// $t: registradores temporarios
// $a: registradores de argumentos
// $id: registradores que identifica o processo que esta executando
// $bf: buffer de troca de registrador com HD, e vice-versa
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

InstructionList assemblyGen(QuadList head, char * tp); // Gerador de codigo Assembly

int searchInstLine_Label(int labelNo);      // Procura numero da linha de instrucao referente ao label para uso na geracao de codigo binario

#endif