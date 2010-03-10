#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "codes.h"

#define SYMTABLE_SIZE 15

//The symbol type enum
typedef enum{
  VAR,
  CONST,
  FUNC
} symType;

//The struct to store in the table
typedef struct{
  
  symType type;
  char name[MAX_IDENT_LENGTH + 1];
  int scope;
  int offset;
  int value;

} symTableEntry;

typedef struct symTableList{

  symTableEntry* data;
  struct symTableList* next;

}symTableList;

void insertSymbol(symTableList** table, symTableEntry* symbol);
symTableEntry* findSymbol(symTableList** table, symType type,  char* symbol, 
                          int scope);
symTableList** newTable();
symTableEntry* newSymbol(symType type, char* name, int scope, int offset,
                         int value);
int hash(char* symbol);

void printStats(symTableList** table);

#endif
