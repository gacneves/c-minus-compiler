#ifndef _SYMTAB_H_
#define _SYMTAB_H_

// Insere na tabela hash na primeira vez
void st_insert( char * name, int lineno, char * scope, char * typeID, char * typeData, int paramQt, int size);

// Procura pelo nome na tabela
int st_lookup ( char * name, char * scope );

// Print da tabela no arquivo de saida
void printSymTab();

// Procura o tipo de dado na tabela
char* st_lookup_tp(char * name, char * scope);

// Procura a quantidade de parametros da funcao declarada
int st_lookup_paramQt(char * name, char * scope);

// Procura o tamanho de um vetor declarado
int st_lookup_size(char *name, char *scope);

// Procura o tamanho de um vetor declarado
int st_lookup_local(char *name, char *scope);

// Insere informacoes sobre a memoria no hash
void insertMemoryData(char * name, char * scope, int location, int * instLine);

// Procura a funcao na tabela e retorna o numero da linha de instrucao referente a ela
int searchInstLine_Fun(char * name);

// Print apenas das informacoes de memoria
void printMemInfo();

// Estrutura para armazenar a linhas em que o token aparece
typedef struct LineListRec{  
    int lineno;
    struct LineListRec * next;  
} * LineList;

// Estrutura para armazenar as informações nas posições da tabela hash
typedef struct BucketListRec{ 
    char* name;
    LineList lines;
    char* scope;
    char* typeID;
    char* typeData;
    int paramQt;
    int loc;
    int size;
    int * instLine;
    struct BucketListRec * next;     
} * BucketList;

#define SIZE 997 // Tamanho da tabela (num primo)
#define SHIFT 4 // Peso dos caracteres para o hash

BucketList hashTable[SIZE];

#endif
