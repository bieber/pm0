#ifndef SYMTABLE_H
#define SYMTABLE_H

#define SYMTABLE_SIZE 100

//The struct to store in the table
typedef struct{
  
  char* name;
  int scope;

} symTableEntry;

typedef struct symTableList{

  symTableEntry* data;
  struct symTableList* next;

}symTableList;

void insertSymbol(symTableList* table, symTableEntry* symbol);
void findSymbol(symTableList* table, char* symbol, int scope);

#endif
