#include "symtable.h"

#include <stdlib.h>
#include <string.h>

void insertSymbol(symTableList* table, symTableEntry* symbol){
  int index = ((int)(*symbol->name) +
               (int)(symbol->name[strlen(symbol->name)-1])) % SYMTABLE_SIZE;
  
  if(!table[index]){
    table[index] = (symTableList*)malloc(sizeof(symTableList));
    table[index]->data = symbol;
    table[index]->next = NULL;
  }else{
    
  }
  
}

void findSymbol(symTableList* table, char* symbol, int scope){

}
