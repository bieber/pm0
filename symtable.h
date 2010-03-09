#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "codes.h"

#define SYMTABLE_SIZE 100

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
  int value;

} symTableEntry;

typedef struct symTableList{

  symTableEntry* data;
  struct symTableList* next;

}symTableList;

void insertSymbol(symTableList** table, symTableEntry* symbol);
void findSymbol(symTableList** table, char* symbol, int scope);
symTableList** newTable();
int hash(char* symbol);

#endif
