#include "symtable.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

symTableEntry* findSymbol(symTableList** table, symType type, char* symbol,
                          int scope ){
  
  //Finding the matching symbol, or null
  symTableList* current;
  for(current = table[hash(symbol)]; current; current = current->next)
    if((current->data->scope == scope || current->data->type == FUNC)
       && strcmp(current->data->name, symbol) == 0
       && current->data->type == type)
      break;

  if(current)
    return current->data;
  else
    return NULL;

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

symTableEntry* newSymbol(symType type, char* name, int scope, int offset,
                         int value){

  symTableEntry* symbol = (symTableEntry*)malloc(sizeof(symTableEntry));
  symbol->type = type;
  strcpy(symbol->name, name);
  symbol->scope = scope;
  symbol->offset = offset;
  symbol->value = value;

  return symbol;

}

void printStats(symTableList** table){
  int i;
  symTableList* current;
  for(i=0; i<SYMTABLE_SIZE; i++){
    printf("|");
    for(current = table[i]; current; current=current->next)
      printf("*");
    printf("\n");
  }
}

void deleteTable(symTableList** table){
  
  symTableList* current;
  symTableList* temp;
  int i;
  for(i = 0; i < SYMTABLE_SIZE; i++){
    current = table[i];
    while(current){
      temp = current->next;
      free(current);
      current = temp;
    }
  }

}
