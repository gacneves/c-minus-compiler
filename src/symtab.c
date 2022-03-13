#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define SIZE 997 // Tamanho da tabela (num primo)
#define SHIFT 4 // Peso dos caracteres para o hash

static int hash (char* name, char* scope){ // Calcula o indice onde deve ser adcionado na tabela
    int temp = 0;
    int i = 0;
    while (name[i] != '\0'){ 
        temp = ((temp << SHIFT) + name[i]) % SIZE;
        i++;
    }
    i = 0;
    while (scope[i] != '\0') // Considera tambem o escopo do token
    { 
        temp = ((temp << SHIFT) + scope[i]) % SIZE;
        i++;
    }
    return temp;
}

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

static BucketList hashTable[SIZE];

void st_insert( char * name, int lineno, char* scope, char* typeID, char* typeData, int paramQt, int size){ 
    int h = hash(name, scope);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(scope,l->scope) != 0)) 
        l = l->next;
    if (l == NULL){ // Insere se não estiver presente na tabela
        l = (BucketList) malloc(sizeof(struct BucketListRec));
        l->name = name;
        l->lines = (LineList) malloc(sizeof(struct LineListRec));
        l->lines->lineno = lineno;
        l->lines->next = NULL;
        l->scope = scope;
        l->typeID = typeID;
        l->typeData = typeData;
        l->paramQt = paramQt;
        l->loc = -1;
        l->size = size;
        l->instLine = NULL;
        l->next = hashTable[h];
        hashTable[h] = l; 
    }
    else{ // Senão insere apenas a linha em que aparece
        LineList t = l->lines;
        while (t->next != NULL && t->lineno != lineno) 
            t = t->next;
        if(t->lineno != lineno){
            t->next = (LineList) malloc(sizeof(struct LineListRec));
            t->next->lineno = lineno;
            t->next->next = NULL;  
        }
    }
} 

int st_lookup (char* name, char* scope){ // Procura na tabela e retorna se esta presente
    int h = hash(name, scope);    
    BucketList l =  hashTable[h];

    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(scope,l->scope) != 0))
        l = l->next;
    if (l == NULL) 
        return 0;
    else 
        return 1;
}

char* st_lookup_tp (char* name, char* scope){ // Procura na tabela e retorna o tipo do dado
    int h = hash(name, scope);    
    BucketList l =  hashTable[h];
  
    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(scope,l->scope) != 0))
        l = l->next;

    if (l == NULL) 
        return "NULL";
    else 
        return l->typeData;
}

int st_lookup_paramQt(char *name, char *scope){ // Procura na tabela e retorna a quantidade de parametros da funcao
    int h = hash(name, scope);	
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(scope,l->scope) != 0)){
        l = l->next;
  }
  if (l == NULL) 
      return -1;
  else 
      return l->paramQt;
}

int st_lookup_size(char *name, char *scope){ // Procura na tabela e retorna o tamanho do vetor
    int h = hash(name, scope);	
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(scope,l->scope) != 0)){
        l = l->next;
  }
    if (l == NULL) 
        return -1;
    else 
        return l->size;
}


void printSymTab(){ // Printa a tabela se símbolos 
    FILE * listing = fopen("output_files/outAnalyze.output","w+");
    int i;
    fprintf(listing,"  Nome                     Escopo          TipoID         TipoDado         Número Linha\n");
    fprintf(listing,"--------                ------------    ------------    ------------    --------------------\n");
    for (i=0;i<SIZE;++i){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){ 
                LineList t = l->lines;
                fprintf(listing,"%-25s  ",l->name);
                fprintf(listing,"%-14s  ",l->scope);
                fprintf(listing,"%-14s  ",l->typeID);
                fprintf(listing,"%-14s  ",l->typeData); 
                while (t != NULL){ 
                    fprintf(listing,"%-4d ",t->lineno);
                    t = t->next;
                }
            fprintf(listing,"\n");
            l = l->next;
            }
        }
    }
    fclose(listing);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Atualizacao para informacos de organizacao de memoria

void insertMemoryData(char * name, char * scope, int location, int * instLine){
    int h = hash(name, scope);
    BucketList l =  hashTable[h];

    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(scope,l->scope) != 0))
        l = l->next;
    l->loc = location;
    l->instLine = instLine;
}

int st_lookup_local(char *name, char *scope){ // Procura na tabela e retorna o local na memoria
    int h = hash(name, scope);	
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(scope,l->scope) != 0)){
        l = l->next;
  }
    if (l == NULL) 
        return -1;
    else 
        return l->loc;
}

int searchInstLine_Fun(char * name){
    int h = hash(name, "global");
    BucketList l =  hashTable[h];

    while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp("global",l->scope) != 0))
        l = l->next;

    return * l->instLine;
}

void printMemInfo(FILE * memoryFile){
    fprintf(memoryFile,"  Local             Nome          Escopo         TipoDado         Tamanho        Linha da instrucao\n");
    fprintf(memoryFile,"---------       ------------    ----------      ----------      -----------     --------------------\n");
    // Print das infos da variaveis globais e locais
    for (int i=0;i<SIZE;++i){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){
                if(l->loc >= 0){
                    if(strcmp(l->scope,"global") != 0)
                        fprintf(memoryFile,"$sp+%-10d  ",l->loc);
                    else 
                        fprintf(memoryFile,"%-14d  ",l->loc);
                    fprintf(memoryFile,"%-14s  ",l->name);
                    fprintf(memoryFile,"%-14s  ",l->scope);
                    fprintf(memoryFile,"%-14s  ",l->typeData);
                    fprintf(memoryFile,"%-14d  ",l->size); 
                    if(l->instLine != NULL) fprintf(memoryFile,"%-14d  ",*l->instLine); 
                fprintf(memoryFile,"\n");
                }
                l = l->next;
            }
        }
    }

    // Print da info das funcoes
    fprintf(memoryFile,"\n----------------------------------------------------------------------------------------------------\n\n");
    for (int i=0;i<SIZE;++i){ 
        if (hashTable[i] != NULL){ 
            BucketList l = hashTable[i];
            while (l != NULL){
                if(strcmp(l->typeID,"funcao") == 0){
                    fprintf(memoryFile,"---------       ");
                    fprintf(memoryFile,"%-14s  ",l->name);
                    fprintf(memoryFile,"%-14s  ",l->scope);
                    fprintf(memoryFile,"%-14s  ",l->typeData);
                    fprintf(memoryFile,"----------      "); 
                    fprintf(memoryFile,"%-14d  ",*l->instLine); 
                fprintf(memoryFile,"\n");
                }
                l = l->next;
            }
        }
    }
}