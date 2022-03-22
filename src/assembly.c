#include "globals.h"
#include "assembly.h"
#include "cgen.h"
#include "symtab.h"

#define QUANTUM 20
#define BUFFER_REG 27

InstructionList instListHead = NULL;    // Inicio da lista de instrucoes Assembly
UnusedRegisterList RegListHead = NULL;  // Inicio da lista de registradores nao usados
UnusedRegisterList RegListTail = NULL;  // Fim da lista de registradores nao usados
Register argRegister;                   // Seleciona o registrador de argumento
int sPos;                               // Local do stack na memoria
int globalVarLocation = 0;              // Onde colocar variaveis globais na memoria
int lineNo = 0;                         // Numero da linha da instrucao Assembly
char * currentScope;                    // Escopo atual alterado uma vez que a quadrupla FUN aparece
int argCounter;                         // Conta quantos argumentos ainda faltam para se guardar na memoria
int checkNotSet;                        // Flag para checagem apos comparacao logica de LET e GET (se $t for 0, entao a comparacao eh verdadeira)
char * inputType;                       // Contem se a entrada é a BIOS, SO ou um programa

int isNumber(char * str){                   // Verifica se uma string contem apenas digitos
    for(int i = 0; i < strlen(str); i++)
        if(isdigit(str[i]) == 0) return 0;

    return 1;
}

int searchInstLine_Label(int labelNo){      // Retorna o numero da instrucao referente ao label para uso na geracao do codigo binario
    return labelMap[labelNo];
}

Register initArgRegister(){
    return (strcmp(inputType,"ARQ")!=0) ? $c0 : $a0;
}

Register tempRegister(char * temp){                 // Retorna o registrador temporario $t(0-7)/$s(0-7) dado a string que o representa
    if(strcmp(temp,"$t0") == 0) return $t0;
    else if(strcmp(temp,"$t1") == 0) return $t1;
    else if(strcmp(temp,"$t2") == 0) return $t2;
    else if(strcmp(temp,"$t3") == 0) return $t3;
    else if(strcmp(temp,"$t4") == 0) return $t4;
    else if(strcmp(temp,"$t5") == 0) return $t5;
    else if(strcmp(temp,"$t6") == 0) return $t6;
    else if(strcmp(temp,"$t7") == 0) return $t7;
    else if(strcmp(temp,"$s0") == 0) return $s0;
    else if(strcmp(temp,"$s1") == 0) return $s1;
    else if(strcmp(temp,"$s2") == 0) return $s2;
    else if(strcmp(temp,"$s3") == 0) return $s3;
    else if(strcmp(temp,"$s4") == 0) return $s4;
    else if(strcmp(temp,"$s5") == 0) return $s5;
    else if(strcmp(temp,"$s6") == 0) return $s6;
    else if(strcmp(temp,"$s7") == 0) return $s7;
    else return -1;
}

char * registerToString(Register r){        // Transforma registradores em string novamente apenas para print do codigo Assembly
    char * reg = malloc(6*(sizeof(char)));
    switch(r){
        case $zero:
            strcpy(reg,"$zero");
            break;
        case $t0:
            strcpy(reg,"$t0");
            break;
        case $t1:
            strcpy(reg,"$t1");
            break;
        case $t2:
            strcpy(reg,"$t2");
            break;
        case $t3:
            strcpy(reg,"$t3");
            break;
        case $t4:
            strcpy(reg,"$t4");
            break;
        case $t5:
            strcpy(reg,"$t5");
            break;
        case $t6:
            strcpy(reg,"$t6");
            break;
        case $t7:
            strcpy(reg,"$t7");
            break;
        case $s0:
            strcpy(reg,"$s0");
            break;
        case $s1:
            strcpy(reg,"$s1");
            break;
        case $s2:
            strcpy(reg,"$s2");
            break;
        case $s3:
            strcpy(reg,"$s3");
            break;
        case $s4:
            strcpy(reg,"$s4");
            break;
        case $s5:
            strcpy(reg,"$s5");
            break;
        case $s6:
            strcpy(reg,"$s6");
            break;
        case $s7:
            strcpy(reg,"$s7");
            break;
        case $a0:
            strcpy(reg,"$a0");
            break;
        case $a1:
            strcpy(reg,"$a1");
            break;
        case $a2:
            strcpy(reg,"$a2");
            break;
        case $a3:
            strcpy(reg,"$a3");
            break;
        case $a4:
            strcpy(reg,"$a4");
            break;
        case $c0:
            strcpy(reg,"$c0");
            break;
        case $c1:
            strcpy(reg,"$c1");
            break;
        case $c2:
            strcpy(reg,"$c2");
            break;
        case $id:
            strcpy(reg,"$id");
            break;
        case $pc:
            strcpy(reg,"$pc");
            break;
        case $ad:
            strcpy(reg,"$ad");
            break;
        case $v0:
            strcpy(reg,"$v0");
            break;
        case $sp:
            strcpy(reg,"$sp");
            break;
        case $md:
            strcpy(reg,"$md");
            break;
        case $ra:
            strcpy(reg,"$ra");
            break;
        default:
            break;
    }
    return reg;
}

void unusedRegInsert(Register r){
    UnusedRegisterList newUnusedReg = (UnusedRegisterList) malloc(sizeof(struct RegisterListRec));
    newUnusedReg->r = r;
    newUnusedReg->next = NULL;
    if(RegListHead == NULL){
        newUnusedReg->prev = NULL;
        RegListHead = newUnusedReg;
        RegListTail = RegListHead;
    }
    else{
        RegListTail->next = newUnusedReg;
        newUnusedReg->prev = RegListTail;
        RegListTail = newUnusedReg;
    }
}

void unusedRegRemove(Register r){
    UnusedRegisterList unusedReg = RegListHead;
    while(unusedReg != NULL && unusedReg->r != r) unusedReg = unusedReg->next;
    if(unusedReg == NULL) ;
    else if(unusedReg == RegListHead){              // Se for o primeiro elemento
        if(unusedReg == RegListTail){               // Se for o unico elemento da lista
            RegListHead = NULL;
            RegListTail = NULL;
        }
        else{                                       // Se nao for o unico elemento da lista
            RegListHead = RegListHead->next;
            RegListHead->prev = NULL;
        }    
    }
    else if(unusedReg == RegListTail){              // Se for o ultimo elemento da lista
        RegListTail = RegListTail->prev;
        RegListTail->next = NULL;
    }
    else{                                           // Se for um elemento do meio da lista
        unusedReg->prev->next = unusedReg->next;
        unusedReg->next->prev = unusedReg->prev;
    }
    free(unusedReg);
}

void assemblyInsert(InstructionKind instKind, Register rd, Register rs, Register rt, int immediate, int lineNo, Type tp, LineKind lineKind, char * lab){ // Insere uma instrucao ou label Assembly
    Instruction newInst;
    newInst.instKind = instKind;
    newInst.rd = rd;
    newInst.rs = rs;
    newInst.rt = rt;
    newInst.immediate = immediate;
    newInst.lineNo = lineNo;
    newInst.type = tp;
    newInst.lineKind = lineKind;
    newInst.label = malloc((strlen(lab)+1) * sizeof(char));
    strcpy(newInst.label,lab);
    InstructionList newAssembly = (InstructionList) malloc(sizeof(struct InstructionListRec));
    newAssembly->inst = newInst;
    newAssembly->next = NULL;
    if(instListHead == NULL) 
        instListHead = newAssembly;
    else{
        InstructionList a = instListHead;
        while(a->next != NULL)  a = a->next;
        a->next = newAssembly;
    }
}

void assemblyGen_FUN(Quad q){                                   // Tratamento da quadrupla FUN
    int * instLine = malloc(sizeof(int));                       // Aloca-se um inteiro para representar o num da linha de instrucao do inicio da funcao
    *instLine = lineNo;
    insertMemoryData(q.arg2, "global", -1, instLine);           // Insere na tabela para uso posterior do codigo binario
    assemblyInsert(0, 0, 0, 0, 0, lineNo, 0, Lab, q.arg2);      // Converte em um label que define o inicio da funcao
    sPos = 0;                                                   // Inicia o topo da pilha como 0
    currentScope = q.arg2;                                      // Armazena-se escopo atual
    argRegister = initArgRegister();                            // Inicializa o registrador de argumento como $a0 (o primeiro a ser passado)
    argCounter = st_lookup_paramQt(q.arg2, "global");           // Guarda-se quantidade de argumentos da funcao
}

void assemblyGen_ARG(Quad q){                                               // Tratamento da quadrupla ARG
    insertMemoryData(q.arg2, q.arg3, sPos, NULL);                           // Insere na area local da memoria o numero inteiro
    assemblyInsert(SW, argRegister, $sp, 0, sPos, lineNo, I, Inst, "");     // Instrucao de SW do argumento
    sPos++;                                                                 // Soma-se o topo da pilha
    lineNo++;                                                               // Soma-se o numero da linha de instrucao
    argRegister++;                                                          // Soma-se o registrador $a de argumento para selecao do proximo
    argCounter--;                                                           // Desconta-se a quantidade de argumentos restantes
    if(argCounter == 0)                                                     // Se for o ultimo argumento
        argRegister = initArgRegister();                                    // Atualiza-se o registrador de argumento para o primeiro novamente para uso posterior
}

void assemblyGen_PARAM(Quad q){                                                             // Tratamento da quadrupla PARAM
    assemblyInsert(ADD, argRegister, tempRegister(q.arg1), $zero, 0, lineNo, R, Inst, "");  // Passa o conteudo do registrador temp para o registrador de argumento
    lineNo++;                                                                               // Soma-se o numero da linha de instrucao
    argRegister++;                                                                          // Soma-se o registrador $a de argumento para selecionar o proximo
    unusedRegRemove(tempRegister(q.arg1));                                                  // Remove-se o reg temp da lista de nao usado
}

void assemblyGen_ALLOC(Quad q){                                     // Tratamento da quadrupla ALLOC                                                                                                                 // Se a variavel a ser alocada nao for um vetor
    if(strcmp(q.arg2,"global") == 0){                               // Se o escopo da variavel for global
        insertMemoryData(q.arg1, q.arg2, globalVarLocation, NULL);  // Aloca-se um espaço na memoria na area global
        globalVarLocation += st_lookup_size(q.arg1, q.arg2);        // Soma-se a proxima posicao a ser inserida na area global
    }
    else{                                                           // Se for uma variavel local
        insertMemoryData(q.arg1, q.arg2, sPos, NULL);               // Aloca-se um espaco na memoria na area local
        sPos += st_lookup_size(q.arg1, q.arg2);                     // Soma-se o topo da pilha
    }
}

void assemblyGen_CALL(Quad q){                                                              // Tratamento da quadrupla CALL
    if(strcmp(q.arg2,"input") == 0){                                                        // Se for a funcao input
        assemblyInsert(IN, tempRegister(q.arg1), 0, 0, 0, lineNo, R, Inst, "");             // Insere-se a instrucao IN salvando o valor no registrador temp
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        unusedRegInsert(tempRegister(q.arg1));                                              // Insere-se na lista de regs nao usados o temp que se armazena o retorno da funcao IN
    }
    else if(strcmp(q.arg2,"output") == 0){                                                  // Se for a funcao output
        assemblyInsert(OUT, argRegister-1, 0, 0, 0, lineNo, R, Inst, "");                   // Insere-se a instrucao OUT passando o argumento a ser impresso na saida
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        argRegister = initArgRegister();                                                    // Reinicializa o registrador de argumento como $a0 (o primeiro a ser passado) para uso posterior
    }
    else if(strcmp(q.arg2,"preempON") == 0){                                                // Se for a funcao preempON
        assemblyInsert(PREEMPON, 0, 0, 0, 0, lineNo, O, Inst, "");                          // Insere-se a instrução PREEMPON, para iniciar a contagem do timer de quantum
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
    }
    else if(strcmp(q.arg2,"preempOFF") == 0){                                               // Se for a funcao preempOFF
        assemblyInsert(PREEMPOFF, 0, 0, 0, 0, lineNo, O, Inst, "");                         // Insere-se a instrução PREEMPOFF, para finalizar a contagem do timer de quantum
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
    }
    else if(strcmp(q.arg2,"memCtrl") == 0){                                                 // Se for a funcao memCtrl
        assemblyInsert(MEMCTRL, 0, 0, 0, 0, lineNo, O, Inst, "");                           // Insere-se a instrução MEMCTRL, para mudar a fonte das instrucoes da BIOS para a mem de inst
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
    }
    else if(strcmp(q.arg2,"hdToInst") == 0){                                                // Se for a funcao hdToInst
        assemblyInsert(HDTOINST, $c0, $c1, $c2, 0, lineNo, R, Inst, "");                    // Insere-se a instrução HDTOINST, para escrever do HD na memória de instruções
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        argRegister = initArgRegister();                                                    // Reinicializa o registrador de argumento como $a0 (o primeiro a ser passado) para uso posterior
    }
    else if(strcmp(q.arg2,"hdToReg") == 0){                                                 // Se for a funcao hdToReg
        assemblyInsert(HDTOREG, $c0, $c1, $c2, 0, lineNo, R, Inst, "");                     // Insere-se a instrução HDTOREG, para escrever do HD no banco de registradores
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        argRegister = initArgRegister();                                                    // Reinicializa o registrador de argumento como $a0 (o primeiro a ser passado) para uso posterior
    }
    else if(strcmp(q.arg2,"regToHd") == 0){                                                 // Se for a funcao regToHd
        assemblyInsert(REGTOHD, $c0, $c1, $c2, 0, lineNo, R, Inst, "");                     // Insere-se a instrução REGTOHD, para escrever do banco de registradores no HD
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        argRegister = initArgRegister();                                                    // Reinicializa o registrador de argumento como $a0 (o primeiro a ser passado) para uso posterior
    }
    else if(strcmp(q.arg2,"storeVar") == 0){                                                // Se for a funcao storeVar
        assemblyInsert(ADD, $pc, $zero, $c0, 0, lineNo, R, Inst, "");                       // Insere-se a instrução ADDI, para escrever no registrador $a0 o numero do registrador de buffer $bf
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        assemblyInsert(ADDI, $c0, $zero, 0, BUFFER_REG, lineNo, I, Inst, "");                       // Insere-se a instrução ADDI, para escrever no registrador $a0 o numero do registrador de buffer $bf
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        assemblyInsert(REGTOHD, $c0, $c1, $c2, 0, lineNo, R, Inst, "");                     // Insere-se a instrução REGTOHD, para escrever do banco de registradores no HD
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        argRegister = initArgRegister();                                                    // Reinicializa o registrador de argumento como $a0 (o primeiro a ser passado) para uso posterior
    }
    else if(strcmp(q.arg2,"setPC") == 0){                                                   // Se for a funcao setPC
        assemblyInsert(JR, $pc, 0, 0, 0, lineNo, R, Inst, "");                              // Insere-se a instrução ADD, para escrever no registrador $bf de buffer de troca de dados entre o HD, a variavel desejada
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
    }
        else if(strcmp(q.arg2,"setRunningId") == 0){                                        // Se for a funcao setRunningId
        assemblyInsert(ADD, $id, $zero, $c0, 0, lineNo, R, Inst, "");                       // Insere-se a instrução ADD, para escrever no registrador $id o identificador do processo que esta rodando
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        argRegister = initArgRegister();                                                    // Reinicializa o registrador de argumento como $a0 (o primeiro a ser passado) para uso posterior
    }
    else if(strcmp(q.arg2,"halt") == 0){                                                    // Se for a funcao halt
        assemblyInsert(HLT, 0, 0, 0, 0, lineNo, O, Inst, "");                               // Insere-se a instrucao HLT que para o funcionamento do processador
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
    }
    else{                                                                                   // Se nao for a funcao input ou output
        // Empilha registradores que ainda nao foram utilizados
        UnusedRegisterList unusedRegister = RegListHead;                                    // Variavel para iterar sobre a lista de registradores nao usados
        while(unusedRegister != NULL){                                                      // Verifica se ha registradores que nao foram usados
            assemblyInsert(SW, unusedRegister->r, $sp, 0, sPos, lineNo, I, Inst, "");       // Insere-se na pilha aqueles que nao foram usados
            lineNo++;                                                                       // Soma-se o numero da linha de instrucao
            sPos++;                                                                         // Soma-se o topo da pilha
            unusedRegister = unusedRegister->next;                                          // Avanca para o proximo registrador da lista
        }

        if(strcmp(currentScope, "main") != 0){                                              // Se a chamada ocorrer dentro de um escopo que nao seja o main (recursao), empilha-se o $ra
            assemblyInsert(SW, $ra, $sp, 0, sPos, lineNo, I, Inst, "");                     // Insere-se a instrucao SW salvando o endereco de retorno salvo em $ra da chamada anterior
            sPos++;                                                                         // Soma-se o topo da pilha
            lineNo++;                                                                       // Soma-se o numero da linha de instrucao
        }

        assemblyInsert(ADDI, $sp, $sp, 0, sPos, lineNo, I, Inst, "");                       // Soma-se o inicio da proxima funcao na memoria, atribuindo o topo da pilha a $sp
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        argRegister = initArgRegister();                                                    // Reinicializa o registrador de argumento como $a0 (o primeiro a ser passado) para uso posterior
        assemblyInsert(JAL, 0, 0, 0, 0, lineNo, JP, Inst, q.arg2);                          // Insere-se a instrucao JAL que pula para funcao e guarda no registrador $ra o endereco de retorno
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        assemblyInsert(SUBI, $sp, $sp, 0, sPos, lineNo, I, Inst, "");                       // Insere-se a instrucao SUBI que volta o registrador $sp pro escopo anterior
        lineNo++;                                                                           // Soma-se o numero da linha de instrucao
        if(strcmp(currentScope, "main") != 0){                                              // Se a chamada ocorrer dentro de um escopo que nao seja o main (recursao) 
            sPos--;                                                                         // Desempilha o endereco de retorno da pilha
            assemblyInsert(LW, $ra, $sp, 0, sPos, lineNo, I, Inst, "");                     // Volta-se o endereco de retorno da chamada atual para o registrador $ra
            lineNo++;                                                                       // Soma-se o numero da linha de instrucao
        }

        // Desempilha registradores que ainda nao foram utilizados
        unusedRegister = RegListTail;                                                       // Comecando do fim da fila
        while(unusedRegister != NULL){                                                      // Enquanto ainda houver registradores para serem desempilhados
            sPos--;                                                                         // Desempilha o registrador da pilha
            assemblyInsert(LW, unusedRegister->r, $sp, 0, sPos, lineNo, I, Inst, "");       // Carreaga-se da pilha os registradores
            lineNo++;                                                                       // Soma-se o numero da linha de instrucao
            unusedRegister = unusedRegister->prev;                                          // Retrocede para o registrador anterior da lsita
        }

        if(strcmp(st_lookup_tp(q.arg2,"global"),"inteiro") == 0){                           // Se a funcao tiver retorno int, salva-se o conteudo de $v0 no registrador temp necessario
            assemblyInsert(ADD, tempRegister(q.arg1), $v0, $zero, 0, lineNo, R, Inst, "");  // Insere-se a instrucao ADD que faz a passagem do valor retornado ao temp
            lineNo++;                                                                       // Soma-se o numero da linha de instrucao
            unusedRegInsert(tempRegister(q.arg1));                                          // Insere na lista de regs nao usados o temp que se armazena o retorno da funcao
        }
    }
}

void assemblyGen_RET(Quad q){                                                       // Tratamento da quadrupla RET
    assemblyInsert(ADD, $v0, tempRegister(q.arg1), $zero, 0, lineNo, R, Inst, "");  // Insere-se a instrucao ADD, que copia o conteudo do temp para o registrador $v0
    lineNo++;                                                                       // Soma-se o numero da linha de instrucao
    assemblyInsert(JR, $ra, 0, 0, 0, lineNo, R, Inst, "");                          // Insere-se a instrucao JR, que pula para o endereco presente em $ra, o endereco de retorno da funcao
    lineNo++;                                                                       // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg1));                                          // Remove-se o reg temp da lista de nao usado
}

void assemblyGen_END(Quad q){                                   // Tratamento da quadrupla END
    if(strcmp(q.arg1,"main") != 0){                             // Se nao for a funcao main, adicionamos um retorno ao fim da mesma
        assemblyInsert(JR, $ra, 0, 0, 0, lineNo, R, Inst, "");  // Insere-se a instrucao JR para o pulo ao endereco em $ra
        lineNo++;                                               // Soma-se o numero da linha de instrucao
    }
}

void assemblyGen_LOAD(Quad q){                                                                                                      // Tratamento da quadrupla LOAD
    if(isNumber(q.arg2)){                                                                                                           // Se o inteiro a se der load vier de uma constante
        assemblyInsert(ADDI, tempRegister(q.arg1), $zero, 0, atoi(q.arg2), lineNo, I, Inst, "");                                    // Insere-se a instrucao ADDI, salvando no registrador $temp a constante
        lineNo++;                                                                                                                   // Soma-se o numero da linha de instrucao
    }                                                           
    else{                                                                                                                           // Se o inteiro vier de uma variavel salva na memoria
        int location;                                                                                                               // Local da variavel na memoria
        if(st_lookup(q.arg2,"global") == 1){                                                                                        // Se o escopo da variavel for global
            location = st_lookup_local(q.arg2,"global");                                                                            // Pega o local da variavel na memoria na area global
            if(strcmp(q.arg3,"-") != 0){                                                                                            // Se for um vetor e precisar acessar algum indice
                assemblyInsert(LW, tempRegister(q.arg1), tempRegister(q.arg3), 0, location, lineNo, I, Inst, "");                   // Usa-se o LW da posicao em location mais o indice do vetor
                unusedRegRemove(tempRegister(q.arg3));                                                                              // Remove-se o reg temp do indice da lista de nao usado
            }
            else                                                                                                                    // Se nao for um vetor
                assemblyInsert(LW, tempRegister(q.arg1), $zero, 0, location, lineNo, I, Inst, "");                                  // Usa-se o LW da posicao em location
            lineNo++;                                                                                                               // Soma-se o numero da linha de instrucao
        }               
        else{                                                                                                                       // Se for uma variavel local
            location = st_lookup_local(q.arg2,currentScope);                                                                        // Pega o local da variavel na memoria na area local
            if(strcmp(q.arg3,"-") != 0){                                                                                            // Se for um vetor e precisar acessar algum indice
                if(strcmp(st_lookup_tp(q.arg2,currentScope),"vetor arg") == 0){                                                     // Se for um vetor como argumento, ele possui o endereco base de um vetor de outro escopo
                    assemblyInsert(LW, tempRegister(q.arg1), $sp, 0, location, lineNo, I, Inst, "");                                // Insere-se a instrucao LW para carregar o endereco base desse vetor
                    lineNo++;                                                                                                       // Soma-se o numero da linha de instrucao
                    assemblyInsert(ADD, tempRegister(q.arg1), tempRegister(q.arg1), tempRegister(q.arg3), 0, lineNo, R, Inst, "");  // Insere-se a instrucao ADD para pegar o endereco base do vetor + indice
                    lineNo++;                                                                                                       // Soma-se o numero da linha de instrucao    
                    assemblyInsert(LW, tempRegister(q.arg1), tempRegister(q.arg1), 0, 0, lineNo, I, Inst, "");                      // Insere-se a instrucao LW para carregar o conteudo do vetor naquele indice
                }       
                else{                                                                                                               // Se for um vetor alocado na area local da funcao
                    assemblyInsert(ADD, tempRegister(q.arg3), tempRegister(q.arg3), $sp, 0, lineNo, R, Inst, "");                   // Insere-se a instrucao ADD para pegar o endereco do vetor no indice em relacao a pilha
                    lineNo++;                                                                                                       // Soma-se o numero da linha de instrucao    
                    assemblyInsert(LW, tempRegister(q.arg1), tempRegister(q.arg3), 0, location, lineNo, I, Inst, "");               // Insere-se a instrucao LW para dar load de um valor em uma vetor no indice salvo na memoria
                }
                unusedRegRemove(tempRegister(q.arg3));                                                                              // Remove-se o reg temp do indice da lista de nao usado
            }       
            else                                                                                                                    // Se nao for um vetor
                assemblyInsert(LW, tempRegister(q.arg1), $sp, 0, location, lineNo, I, Inst, "");                                    // Insere-se a instrucao LW para carregar o dado daquele endereco
            lineNo++;                                                                                                               // Soma-se o numero da linha de instrucao
        }   
    }
    unusedRegInsert(tempRegister(q.arg1));                                                                                          // Insere na lista de regs nao usados ao se dar load em um temp
}

void assemblyGen_STORE(Quad q){                                                                                     // Tratamento da quadrupla STORE
    int location;                                                                                                   // Local da variavel na memoria
    if(st_lookup(q.arg1,"global") == 1){                                                                            // Se o escopo da variavel for global
        location = st_lookup_local(q.arg1,"global");                                                                // Pega o local da variavel na memoria na area global
        if(strcmp(q.arg3,"-") != 0){                                                                                // Se for um vetor e precisar acessar algum indice
            assemblyInsert(SW, tempRegister(q.arg2), tempRegister(q.arg3), 0, location, lineNo, I, Inst, "");       // Usa-se o SW da posicao em location mais o indice do vetor
            unusedRegRemove(tempRegister(q.arg3));                                                                  // Remove-se o reg temp de indice da lista de nao usado
        }
        else                                                                                                        // Se nao for um vetor
            assemblyInsert(SW, tempRegister(q.arg2), $zero, 0, location, lineNo, I, Inst, "");                      // Usa-se o SW da posicao em location
        lineNo++;                                                                                                   // Soma-se o numero da linha de instrucao
    }
    else{                                                                                                           // Se for uma variavel local                                  
        location = st_lookup_local(q.arg1,currentScope);                                                            // Pega o local da variavel na memoria na area local
        if(strcmp(q.arg3,"-") != 0){                                                                                // Se for um vetor e precisar acessar algum indice
            if(strcmp(st_lookup_tp(q.arg1,currentScope),"vetor arg") == 0){                                         // Se for um vetor como argumento, ele possui o endereco base de um vetor de outro escopo 
                assemblyInsert(LW, $ad, $sp, 0, location, lineNo, I, Inst, "");                                     // Insere-se a instrucao LW para carregar o endereco base desse vetor no registrador $ad
                lineNo++;                                                                                           // Soma-se o numero da linha de instrucao
                assemblyInsert(ADD, $ad, $ad, tempRegister(q.arg3), 0, lineNo, R, Inst, "");                        // Insere-se a instrucao ADD para pegar o endereco base do vetor + indice
                lineNo++;                                                                                           // Soma-se o numero da linha de instrucao    
                assemblyInsert(SW, tempRegister(q.arg2), $ad, 0, 0, lineNo, I, Inst, "");                           // Insere-se a instrucao SW para guardar o conteudo do vetor naquele indice
            }       
            else{                                                                                                   // Se for um vetor alocado na area local da funcao
                assemblyInsert(ADD, tempRegister(q.arg3), tempRegister(q.arg3), $sp, 0, lineNo, R, Inst, "");       // Insere-se a instrucao ADD para pegar o endereco indice deslocado apenas de $sp
                lineNo++;                                                                                           // Soma-se o numero da linha de instrucao
                assemblyInsert(SW, tempRegister(q.arg2), tempRegister(q.arg3), 0, location, lineNo, I, Inst, "");   // Insere-se a instrucao SW para dar store de um temp em um vetor no indice salvo na memoria a partir do endereco base + indice + $sp
            }
            unusedRegRemove(tempRegister(q.arg3));                                                                  // Remove-se o reg temp de indice da lista de nao usado
        }       
        else                                                                                                        // Se nao for um vetor
            assemblyInsert(SW, tempRegister(q.arg2), $sp, 0, location, lineNo, I, Inst, "");                        // Insese-se a instrucao SW para guardar um dado naquele endereco
        lineNo++;                                                                                                   // Soma-se o numero da linha de instrucao
    }
    unusedRegRemove(tempRegister(q.arg2));                                                                          // Remove-se o reg temp da lista de nao usado     
}

void assemblyGen_ASSIGN(Quad q){                                                                        // Tratamento da quadrupla ASSIGN
    assemblyInsert(ADD, tempRegister(q.arg1), tempRegister(q.arg2), $zero, 0, lineNo, R, Inst, "");     // Insere-se a instrucao ADD para copiar o conteudo de um registrador para o outro
    lineNo++;                                                                                           // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg2));                                                              // Remove-se o reg temp, que passou o inteiro para o outro reg destino, da lista de nao usado
}

void assemblyGen_LABEL(Quad q){                             // Tratamento da quarupla LABEL
    char * labNo = &q.arg1[1];                              // Pega o numero do label
    labelMap[atoi(labNo)] = lineNo;                         // Insere no vetor de labels a linha da instrucao que ele representa
    assemblyInsert(0, 0, 0, 0, 0, lineNo, 0, Lab, q.arg1);  // Converte em um label numerico para controle do programa
}

void assemblyGen_GOTO(Quad q){                                  // Tratamento da quadrupla GOTO
    assemblyInsert(J, 0, 0, 0, 0, lineNo, JP, Inst, q.arg1);    // Insere-se a instrucao de J para o label desejado
    lineNo++;                                                   // Soma-se o numero da linha de instrucao
}

void assemblyGen_IFF(Quad q){                                                               // Tratamento da quadrupla IFF
    if(checkNotSet == 0)                                                                    // Caso nao seja um IFF de um LET ou GET
        assemblyInsert(BEQ, tempRegister(q.arg1), $zero, 0, 0, lineNo, I, Inst, q.arg2);    // Insere-se a instrucao BEQ para pulo ao label caso o temp da quadrupla seja falso (temp == 0)
    else{                                                                                   // Caso seja de um LET ou GET
        assemblyInsert(BNEQ, tempRegister(q.arg1), $zero, 0, 0, lineNo, I, Inst, q.arg2);   // Insere-se a instrucao BNEQ para pulo ao label caso o temp da quadrupla seja falso (temp != 0)
        checkNotSet = 0;                                                                    // Reseta a flag para controle
    }
    lineNo++;                                                                               // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg1));                                                  // Remove-se o reg temp da lista de nao usado
}

void assemblygen_Arithmetic(Quad q){                                                                                        // Tratamento de quadruplas aritmeticas
    InstructionKind operation;                                                                                              // Operacao a ser realizada
    if(strcmp(q.op,"ADD") == 0) operation = ADD;                                                                            // Quadrupla ADD
    else if(strcmp(q.op,"SUB") == 0) operation = SUB;                                                                       // Quadrupla SUB
    else if(strcmp(q.op,"MUL") == 0) operation = MUL;                                                                       // Quadrupla MUL
    else operation = DIV;                                                                                                   // Quadrupla DIV
    assemblyInsert(operation, tempRegister(q.arg1), tempRegister(q.arg2), tempRegister(q.arg3), 0, lineNo, R, Inst, "");    // Insere-se a instrucao referente a quadrupla aritmetica
    lineNo++;                                                                                                               // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg2));                                                                                  // Remove-se o reg temp referente ao operando 1 da lista de nao usado
    unusedRegRemove(tempRegister(q.arg3));                                                                                  // Remove-se o reg temp referente ao operando 2 da lista de nao usado
    unusedRegInsert(tempRegister(q.arg1));                                                                                  // Insere-se o reg temp destino na lista de nao usado
}

void assemblyGen_LT(Quad q){                                                                                        // Tratamento da quadrupla LT
    assemblyInsert(SLT, tempRegister(q.arg1), tempRegister(q.arg2), tempRegister(q.arg3), 0, lineNo, R, Inst, "");  // Insere-se a instrucao SLT (rd = 1 se rs < rt, ou rd = 0 se rs >= rt)
    lineNo++;                                                                                                       // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg2));                                                                          // Remove-se o reg temp referente ao operando 1 da lista de nao usado
    unusedRegRemove(tempRegister(q.arg3));                                                                          // Remove-se o reg temp referente ao operando 2 da lista de nao usado
    unusedRegInsert(tempRegister(q.arg1));                                                                          // Insere-se o reg temp destino na lista de nao usado
}

void assemblyGen_LET(Quad q){                                                                                       // Tratamento da quadrupla LET
    assemblyInsert(SGT, tempRegister(q.arg1), tempRegister(q.arg2), tempRegister(q.arg3), 0, lineNo, R, Inst, "");  // Insere-se a instrucao SGT (rd = 1 se rs > rt, ou rd = 0 se rs <= rt)
    lineNo++;                                                                                                       // Soma-se o numero da linha de instrucao
    checkNotSet = 1;                                                                                                // Seta a flag para que na quad IFF cheque se o temp é 0 (0 == TRUE)
    unusedRegRemove(tempRegister(q.arg2));                                                                          // Remove-se o reg temp referente ao operando 1 da lista de nao usado
    unusedRegRemove(tempRegister(q.arg3));                                                                          // Remove-se o reg temp referente ao operando 2 da lista de nao usado
    unusedRegInsert(tempRegister(q.arg1));                                                                          // Insere-se o reg temp destino na lista de nao usado
}

void assemblyGen_GT(Quad q){                                                                                        // Tratamento da quadrupla GT
    assemblyInsert(SGT, tempRegister(q.arg1), tempRegister(q.arg2), tempRegister(q.arg3), 0, lineNo, R, Inst, "");  // Insere-se a instrucao SGT (rd = 1 se rs > rt, ou rd = 0 se rs <= rt)
    lineNo++;                                                                                                       // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg2));                                                                          // Remove-se o reg temp referente ao operando 1 da lista de nao usado
    unusedRegRemove(tempRegister(q.arg3));                                                                          // Remove-se o reg temp referente ao operando 2 da lista de nao usado
    unusedRegInsert(tempRegister(q.arg1));                                                                          // Insere-se o reg temp destino na lista de nao usado
}

void assemblyGen_GET(Quad q){                                                                                       // Tratamento da quadrupla GET
    assemblyInsert(SLT, tempRegister(q.arg1), tempRegister(q.arg2), tempRegister(q.arg3), 0, lineNo, R, Inst, "");  // Insere-se a instrucao SLT (rd = 1 se rs > rt, ou rd = 0 se rs <= rt)
    lineNo++;                                                                                                       // Soma-se o numero da linha de instrucao
    checkNotSet = 1;                                                                                                // Seta a flag para que na quad IFF checa se o temp é 0 (0 == TRUE)
    unusedRegRemove(tempRegister(q.arg2));                                                                          // Remove-se o reg temp referente ao operando 1 da lista de nao usado
    unusedRegRemove(tempRegister(q.arg3));                                                                          // Remove-se o reg temp referente ao operando 2 da lista de nao usado
    unusedRegInsert(tempRegister(q.arg1));                                                                          // Insere-se o reg temp destino na lista de nao usado
}

void assemblyGen_EQ(Quad q, Quad branchInfo){                                                                   // Tratamento da quadrupla EQ
    assemblyInsert(BNEQ, tempRegister(q.arg2), tempRegister(q.arg3), 0, 0, lineNo, I, Inst, branchInfo.arg2);   // Insere-se a instrucao BNEQ, pulando para o respectivo label da quad IFF se os temps da quad EQ nao forem iguais  
    lineNo++;                                                                                                   // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg2));                                                                      // Remove-se o reg temp referente ao operando 1 da lista de nao usado
    unusedRegRemove(tempRegister(q.arg3));                                                                      // Remove-se o reg temp referente ao operando 2 da lista de nao usado
}

void assemblyGen_NEQ(Quad q, Quad branchInfo){                                                                  // Tratamento da quadrupla NEQ
    assemblyInsert(BEQ, tempRegister(q.arg2), tempRegister(q.arg3), 0, 0, lineNo, I, Inst, branchInfo.arg2);    // Insere-se a instrucao BEQ, pulando para o respectivo label se os temp da quad NEQ forem iguais  
    lineNo++;                                                                                                   // Soma-se o numero da linha de instrucao
    unusedRegRemove(tempRegister(q.arg2));                                                                      // Remove-se o reg temp referente ao operando 1 da lista de nao usado
    unusedRegRemove(tempRegister(q.arg3));                                                                      // Remove-se o reg temp referente ao operando 2 da lista de nao usado
}

void assemblyGen_HLT(Quad q){                               // Tratamento da quadrupla HLT
    assemblyInsert(HLT, 0, 0, 0, 0, lineNo, O, Inst, "");   // Insere-se a instrucao HLT que para o funcionamento do processador
    lineNo++;                                               // Soma-se o numero da linha de instrucao
}

void assemblyGen_FINALIZE(Quad q){                              // Tratamento da quadrupla FINALIZE
    assemblyInsert(FINALIZE, 0, 0, 0, 0, lineNo, O, Inst, "");  // Insere-se a instrucao FINALIZE que sinaliza o termino de um programa
    lineNo++;                                                   // Soma-se o numero da linha de instrucao
}

void printAssembly(FILE * assemblyFile){ // Funcao para print do codigo Assembly gerado
    InstructionList i = instListHead;
    while(i != NULL){
        switch(i->inst.lineKind){
            case Inst:
                switch(i->inst.instKind){
                    case ADD:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "ADD", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case SUB:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "SUB", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case MUL:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "MUL", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case DIV:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "DIV", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case NOT:
                        fprintf(assemblyFile, "%d %s %s %s\n", i->inst.lineNo, "NOT", registerToString(i->inst.rd), registerToString(i->inst.rs));
                        break;
                    case AND:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "AND", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case OR:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "OR", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case XOR:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "XOR", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case SLT:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "SLT", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case SGT:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "SGT", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case JR:
                        fprintf(assemblyFile, "%d %s %s\n", i->inst.lineNo, "JR", registerToString(i->inst.rd));
                        break;
                    case IN:
                        fprintf(assemblyFile, "%d %s %s\n", i->inst.lineNo, "IN", registerToString(i->inst.rd));
                        break;
                    case OUT:
                        fprintf(assemblyFile, "%d %s %s\n", i->inst.lineNo, "OUT", registerToString(i->inst.rd));
                        break;
                    case ADDI:
                        fprintf(assemblyFile, "%d %s %s, %s, %d\n", i->inst.lineNo, "ADDI", registerToString(i->inst.rd), registerToString(i->inst.rs), i->inst.immediate);
                        break;
                    case SUBI:
                        fprintf(assemblyFile, "%d %s %s, %s, %d\n", i->inst.lineNo, "SUBI", registerToString(i->inst.rd), registerToString(i->inst.rs), i->inst.immediate);
                        break;
                    case BEQ:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "BEQ", registerToString(i->inst.rd), registerToString(i->inst.rs), i->inst.label);
                        break;
                    case BNEQ:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "BNEQ", registerToString(i->inst.rd), registerToString(i->inst.rs), i->inst.label);
                        break;
                    case LW:
                        fprintf(assemblyFile, "%d %s %s, %s, %d\n", i->inst.lineNo, "LW", registerToString(i->inst.rd), registerToString(i->inst.rs), i->inst.immediate);
                        break;
                    case SW:
                        fprintf(assemblyFile, "%d %s %s, %s, %d\n", i->inst.lineNo, "SW", registerToString(i->inst.rd), registerToString(i->inst.rs), i->inst.immediate);
                        break;
                    case SR:
                        fprintf(assemblyFile, "%d %s %s, %s\n", i->inst.lineNo, "SR", registerToString(i->inst.rd), registerToString(i->inst.rs));
                        break;
                    case SL:
                        fprintf(assemblyFile, "%d %s %s, %s\n", i->inst.lineNo, "SL", registerToString(i->inst.rd), registerToString(i->inst.rs));
                        break;
                    case J:
                        fprintf(assemblyFile, "%d %s %s\n", i->inst.lineNo, "J", i->inst.label);
                        break;
                    case JAL:
                        fprintf(assemblyFile, "%d %s %s\n", i->inst.lineNo, "JAL", i->inst.label);
                        break;
                    case NOP:
                        fprintf(assemblyFile, "%d %s\n", i->inst.lineNo, "NOP");
                        break;
                    case HLT:
                        fprintf(assemblyFile, "%d %s\n", i->inst.lineNo, "HLT");
                        break;
                    case FINALIZE:
                        fprintf(assemblyFile, "%d %s\n", i->inst.lineNo, "FINALIZE");
                        break;
                    case PREEMPON:
                        fprintf(assemblyFile, "%d %s\n", i->inst.lineNo, "PREEMPON");
                        break;
                    case PREEMPOFF:
                        fprintf(assemblyFile, "%d %s\n", i->inst.lineNo, "PREEMPOFF");
                        break;
                    case MEMCTRL:
                        fprintf(assemblyFile, "%d %s\n", i->inst.lineNo, "MEMCTRL");
                        break;
                    case HDTOINST:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "HDTOINST", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case HDTOREG:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "HDTOREG", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    case REGTOHD:
                        fprintf(assemblyFile, "%d %s %s, %s, %s\n", i->inst.lineNo, "REGTOHD", registerToString(i->inst.rd), registerToString(i->inst.rs), registerToString(i->inst.rt));
                        break;
                    default:
                        break;
                }
                break;

            case Lab:
                fprintf(assemblyFile, ".%s\n", i->inst.label);
                break;

            default:
                break;
        }
        i = i->next;
    }
}

void printLabelInfo(FILE * memoryFile){                                         // Printa no arquivo de memoria informacoes sobre os labels
    for(int i=0; i<nlabel; i++){
        fprintf(memoryFile,"---------       ");
        fprintf(memoryFile,"L%-13d  ", i);
        fprintf(memoryFile,"----------      label           ----------      ");
        fprintf(memoryFile,"%d\n", labelMap[i]);
    }
}

InstructionList assemblyGen(QuadList head, char * tp){
    inputType = tp;
    labelMap = malloc(nlabel * sizeof(int));                                    // Inicializa map dos labels
    sPos = globalSize;                                                          // Inicializa o inicio da pilha
    
    assemblyInsert(ADDI, $sp, $zero, 0, sPos, lineNo, I, Inst, "");             // Instrucao para armazenar inicio da pilha em $sp
    lineNo++;                                                                   // Soma-se o numero da linha de instrucao
    assemblyInsert(J, 0, 0, 0, 0, lineNo, JP, Inst, "main");                    // Instrucao para pular para a funcao main
    lineNo++;                                                                   // Soma-se o numero da linha de instrucao
    
    QuadList q = head;                                                          // Lista de quadruplas geradas para sua iteracao
    while(q != NULL){                                                           // Enquanto houver alguma quadrupla, chama sua respectiva funcao para geracao da instrucao Assembly
        if(strcmp(q->quad.op,"FUN") == 0) assemblyGen_FUN(q->quad);
        else if(strcmp(q->quad.op,"ARG") == 0) assemblyGen_ARG(q->quad);
        else if(strcmp(q->quad.op,"PARAM") == 0) assemblyGen_PARAM(q->quad);
        else if(strcmp(q->quad.op,"ALLOC") == 0) assemblyGen_ALLOC(q->quad);
        else if(strcmp(q->quad.op,"CALL") == 0) assemblyGen_CALL(q->quad);
        else if(strcmp(q->quad.op,"RET") == 0) assemblyGen_RET(q->quad);
        else if(strcmp(q->quad.op,"END") == 0) assemblyGen_END(q->quad);
        else if(strcmp(q->quad.op,"LOAD") == 0) assemblyGen_LOAD(q->quad);
        else if(strcmp(q->quad.op,"STORE") == 0) assemblyGen_STORE(q->quad);
        else if(strcmp(q->quad.op,"ASSIGN") == 0) assemblyGen_ASSIGN(q->quad);
        else if(strcmp(q->quad.op,"LABEL") == 0) assemblyGen_LABEL(q->quad);
        else if(strcmp(q->quad.op,"GOTO") == 0) assemblyGen_GOTO(q->quad);
        else if(strcmp(q->quad.op,"IFF") == 0) assemblyGen_IFF(q->quad);
        else if(strcmp(q->quad.op,"ADD") == 0) assemblygen_Arithmetic(q->quad);
        else if(strcmp(q->quad.op,"SUB") == 0) assemblygen_Arithmetic(q->quad);
        else if(strcmp(q->quad.op,"MUL") == 0) assemblygen_Arithmetic(q->quad);
        else if(strcmp(q->quad.op,"DIV") == 0) assemblygen_Arithmetic(q->quad);
        else if(strcmp(q->quad.op,"LT") == 0) assemblyGen_LT(q->quad);
        else if(strcmp(q->quad.op,"LET") == 0) assemblyGen_LET(q->quad);
        else if(strcmp(q->quad.op,"GT") == 0) assemblyGen_GT(q->quad);
        else if(strcmp(q->quad.op,"GET") == 0) assemblyGen_GET(q->quad);
        else if(strcmp(q->quad.op,"EQ") == 0){                                  // Se for um EQ passa também a quadrupla seguinte de IFF
            assemblyGen_EQ(q->quad, q->next->quad);
            q = q->next;
        }
        else if(strcmp(q->quad.op,"NEQ") == 0){                                 // Se for um NEQ passa também a quadrupla seguinte de IFF
            assemblyGen_NEQ(q->quad, q->next->quad);
            q = q->next;
        }
        else if(strcmp(q->quad.op,"HLT") == 0) assemblyGen_HLT(q->quad);
        else if(strcmp(q->quad.op,"FINALIZE") == 0) assemblyGen_FINALIZE(q->quad);
        q = q->next;
    }

    FILE * codefile = fopen("debug/outAssembly.output", "w+");
    printAssembly(codefile);
    fclose(codefile);

    FILE * memoryFile = fopen("debug/outMemory.output", "w+");
    printMemInfo(memoryFile);
    printLabelInfo(memoryFile);
    fclose(memoryFile);

    return instListHead;
}