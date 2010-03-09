#include "symtable.h"

#include <stdlib.h>
#include <string.h>

void insertSymbol(symTableList** table, symTableEntry* symbol){
  symTableList* newChain;
  int index = hash(symbol->name);
  
  if(!table[index]){
    table[index] = (symTableList*)malloc(sizeof(symTableList));
    table[index]->data = symbol;
    table[index]->next = NULL;
  }else{
    newChain = (symTableList*)malloc(sizeof(symTableList));
    newChain->data = symbol;
    newChain->next = table[index];
    table[index] = newChain;
  }
  
}

void findSymbol(symTableList** table, char* symbol, int scope){
  
}

int hash(char* symbol){
  return ((int)(*symbol) + (int)(symbol[strlen(symbol)-1])) % SYMTABLE_SIZE;
}

symTableList** newTable(){
  int i;
  symTableList** table = (symTableList**)malloc(sizeof(symTableList*)
                                                * SYMTABLE_SIZE);
  
  //Initializing the table entries to NULL
  for(i = 0; i < SYMTABLE_SIZE; table[i++] = NULL);

  return table;
}
