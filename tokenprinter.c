#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codes.h"

/*==== Globals */

token currentToken;
union{
  int numeric;
  char string[MAX_IDENT_LENGTH + 1];
} tokenVal;

/*==============*/

typedef struct tokenList{
  
  token symbol;
  char string[MAX_IDENT_LENGTH + 1];
  int numeric;
  int varNum;

  struct tokenList* next;

} tokenList;

typedef struct symList{
  
  char name[MAX_IDENT_LENGTH + 1];
  int index;
  struct symList* next;

} symList;

int indexFromName(symList* list, char* name){
  symList* current;
  for(current = list; current; current = current->next){
    if(strcmp(name, current->name) == 0)
       return current->index;
  }

  return 0;

}

void readToken(){
  
  //Making sure we still have input to read
  if(feof(stdin)){
    currentToken = nulsym;
    return;
  }

  //Reading the new token in
  fscanf(stdin, "%d ", (int*)(&currentToken));

  //Checking to see if we need to read anything else in
  if(currentToken == identsym){
    fscanf(stdin, "%s ", tokenVal.string);
  }

  if(currentToken == numbersym){
    fscanf(stdin, "%d ", &tokenVal.numeric);
  }

}

int main(int argc, char* argv[]){

  int maxIndex = 0;
  int i;

  tokenList* head = NULL;
  tokenList* current;
  symList* symbols = NULL;
  symList* temp;

  readToken();

  do{
    
    //Making the new link in the token list
    if(!head){
      head = (tokenList*)malloc(sizeof(tokenList));
      current = head;
      current->next = NULL;
    }else{
      current->next = (tokenList*)malloc(sizeof(tokenList));
      current->next->next = NULL;
      current = current->next;
    }

    current->symbol = currentToken;
    if(currentToken == identsym)
      strcpy(current->string, tokenVal.string);
    if(currentToken == numbersym)
      current->numeric = tokenVal.numeric;

    readToken();
  }while(currentToken != nulsym);

  //Printing the first time and loading the symbols
  for(current = head; current; current = current->next){
    printf("%d ", current->symbol);
    if(current->symbol == identsym){
      i = indexFromName(symbols, current->string);
      if(i > 0){
        printf("%d ", i);
      }else{
        temp = (symList*)malloc(sizeof(symList));
        temp->next = symbols;
        symbols = temp;
        temp->index = ++maxIndex;
        strcpy(temp->name, current->string);
        printf("%d ", maxIndex);
      }
    }

    if(current->symbol == numbersym){
      printf("%d ", current->numeric);
    }
  }

  printf("\n\n");

  //Now printing the symbolic representation

  for(current = head; current; current = current->next){
    
  }

  //Deleting the token list
  current = head;
  while(current){
    head = current;
    current = current->next;
    free(head);
  }

  //Deleting the symbol list
  temp = symbols;
  while(temp){
    symbols = temp;
    temp = temp->next;
    free(temp);
  }

  return 0;

}
