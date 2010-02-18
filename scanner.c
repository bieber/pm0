#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dfa.h"
#include "codes.h"

#define USAGE "Usage: scanner [-q] infile"

struct tokenList{
  char* lexeme;
  token symbol;
  int printLexeme; //Flag to tell whether we should print the lexeme in output
  struct tokenList* next;
};

int transition(DFA* this, char input);
struct tokenList* newNode(char* lexeme, int symbol, int print);
void deleteList(struct tokenList* root);

int main(int argc, char* argv[]){

  FILE* fin;
  int verbose = 1;
  int fileArg = 1;
  int charIn;
  int i;
  struct tokenList* tokenTable=NULL;
  struct tokenList* lastToken;
  struct tokenList* currToken;
  
  DFA machine;

  //Checking command-line arguments
  if(argc < 2 || argc > 3){
    printf("%s\n", USAGE);
    return 1;
  }

  //Turning off the verbose flag, if necessary
  if(argc == 3){
    fileArg = 2;
    if(strcmp(argv[1], "-q") == 0)
      verbose = 0;
  }

  machine.transition = &transition;

  fin = fopen(argv[fileArg], "r");
  lastToken=0;

  //Inserting a newline before any output gets sent
  printf("\n");

  while(!feof(fin)){

    //Initializing and running the machine
    initDFA(&machine);
    runDFA(&machine, fin);
    
    //In case of error, skip output
    if(!machine.accept)
      continue;

    //If we get nulsym, skip output
    if(machine.retVal.numeric == nulsym)
      continue;

    //Otherwise, add this token to the list
    if(!tokenTable){
      tokenTable = newNode(machine.retVal.string, machine.retVal.numeric, 
                           machine.retVal.retString);
      lastToken = tokenTable;
    }else{
      lastToken->next = newNode(machine.retVal.string, machine.retVal.numeric,
                                machine.retVal.retString);
      lastToken = lastToken->next;
    }
    
  }

  fclose(fin);


  //Printing the input file, if verbose is set
  if(verbose){
    printf("\nSource Program:\n");
    fin=fopen(argv[fileArg], "r");
    while((charIn = fgetc(fin)) != EOF)
      printf("%c", (char)charIn);
    fclose(fin);
  }
  
  //Printing the lexeme table, if verbose is set
  if(verbose && tokenTable){
    printf("\nLexeme Table:\n");
    printf("%s        ", "Lexeme");
    printf("%s\n", "Token");
    printf("-----------------\n");
    
    //Printing all the lexemes
    currToken = tokenTable;
    while(currToken){
      printf("%s ", currToken->lexeme);
      for(i = 0; i < 12 - strlen(currToken->lexeme); i++)
        printf(" ");
      printf("%d\n", (int)(currToken->symbol));
      currToken=currToken->next;
    }
    
  }
  
  //Freeing memory
  deleteList(tokenTable);

  return 0;
}

int transition(DFA* this, char input){
  
  switch(this->state){
  case 0:  //Start state
    if(isspace(input)){
      this->accept = 1;
      this->retVal.numeric = nulsym;
      this->retVal.retString = 0;
      this->halt = 1;
    }else if(isalpha(input)){

      //From this point, the default is to return an identifier, so we
      //set up the machine's state accordingly
      this->accept = 1;
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->rewind = 1;

      if(input == 's'){
        return 3;
      }else if(input == 'f'){
        return 13;
      }else if(input == 't'){
        return 17;
      }else if(input == 'm'){
        return 38;
      }else if(input == 'w'){
        return 40;
      }else{
        return 1;
      }
    }else if(isdigit(input)){
      return 2;
    }else{
      printf("Invalid Symbols\n");
      this->rewind = 0;
      rejectDFA(this);
    }
    break;

  case 1:  //Handling generic identifiers
    if(strlen(this->retVal.string) > MAX_IDENT_LENGTH){
      printf("Error: Identifier too long\n");
      this->accept=0;
      this->halt = 1;
    }

    if(isalnum(input)){
      return 1;
    }else{
      this->halt = 1;
    }
    break;
  
  case 2: // Handling digits
    if(isdigit(input)){
      if(strlen(this->retVal.string) > MAX_NUMBER_LENGTH){
          printf("Error: Invalid numerical length\n");
          this->rewind = 0;
          rejectDFA(this);
      }
      return 2;
    }else if(isalpha(input)){
      printf("Error: Invalid identifier\n");
      this->rewind=0;
      rejectDFA(this);
    }else{
      this->accept = 1;
      this->halt = 1;
      this->retVal.retString = 1;
      this->retVal.numeric = numbersym;
      this->rewind = 1;
    }
    break;
  
  case 3:
    if(isalnum(input)){
      if(input == 'y')
        return 4;
      else if(input == 'n')
        return 8;
      else if(input == 'i'){
        this->accept = 1;
        return 4;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 4:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = sisym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
  
  case 5:
    if(isalnum(input)){
      if(input == 'a')
        return 6;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 6:
    if(isalnum(input)){
      if(input == 'w'){
        this->accept = 1;
        return 1;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 7:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = syawsym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
  
  case 8:
    if(isalnum(input)){
      if(input == 'g')
        return 9;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 9:
    if(isalnum(input)){
      if(input == 'a')
        return 8;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 10:
    if(input == '\''){
      return 11;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 11:
    if(isalnum(input)){
      if(input == 'i'){
        this->accept = 1;
        return 12;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 12:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = sngaisym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
  
  case 13:
    if(isalnum(input)){
      if(input == 'p')
        return 14;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 14:
    if(isalnum(input)){
      if(input == 'e')
        return 15;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 15:
    if(input == '\''){
      this->accept = 1;
      return 16;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 16:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = fpesym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
    
  case 17:
    if(isalnum(input)){
      if(input == 'x')
        return 18;
      else if(input == 's')
        return 27;
      else if(input == 'e')
        return 32;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 18:
    if(isalnum(input)){
      if(input == 'o'){
        this->accept = 1;
        return 19;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 19:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = txosym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
  
  case 20:
    if(isalnum(input)){
      if(input == 'e')
        return 21;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 21:
    if(isalnum(input)){
      if(input == 'f')
        return 22;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 22:
    if(isalnum(input)){
      if(input == 'y')
        return 23;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 23:
    if(isalnum(input)){
      if(input == 'y')
        return 24;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 24:
    if(isalnum(input)){
      if(input == 'a')
        return 25;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 25:
    if(isalnum(input)){
      if(input == 'w'){
        this->accept = 1;
        return 26;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 26:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = txokefyawsym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
  
  case 27:
    if(isalnum(input)){
      if(input == 'a')
        return 28;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 28:
    if(isalnum(input)){
      if(input == 'k')
        return 29;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 29:
    if(isalnum(input)){
      if(input == 'r')
        return 30;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 30:
    if(isalnum(input)){
      if(input == 'r'){
        this->accept = 1;
        return 31;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 31:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = tsakrrsym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
  
  case 32:
    if(isalnum(input)){
      if(input == 'n')
        return 33;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 33:
    if(isalnum(input)){
      if(input == 'g')
        return 34;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 34:
    if(isalnum(input)){
      if(input == 'k')
        return 35;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 35:
    if(isalnum(input)){
      if(input == 'r')
        return 36;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 36:
    if(isalnum(input)){
      if(input == 'r'){
        this->accept = 1;
        return 37;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 37:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = tengkrrsym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
    
  case 38:
    if(isalnum(input)){
      if(input == 'i')
        return 39;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 39:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = misym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
    
  case 40:
    if(isalnum(input)){
      if(input == 'r')
        return 41;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 41:
    if(isalnum(input)){
      if(input == 'r')
        return 42;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 42:
    if(isalnum(input)){
      if(input == 'r')
        return 43;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 43:
    if(isalnum(input)){
      if(input == 'p')
        return 44;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 44:
    if(isalnum(input)){
      if(input == 'a'){
        this->accept = 1;
        return 45;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 45:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = wrrpasym;
      this->retVal.retString = 0;
      this->rewind = 1;
    }
    break;
  }
  
  
    
  return -1;
}

struct tokenList* newNode(char* lexeme, int symbol, int print){
  struct tokenList* retVal=(struct tokenList*)malloc(sizeof(struct tokenList));
  retVal->lexeme = strdup(lexeme);
  retVal->symbol = symbol;
  retVal->printLexeme = print;

  return retVal;
}
void deleteList(struct tokenList* root){
  struct tokenList* next;
  while(root){
    free(root->lexeme);
    next = root->next;
    free(root);
    root = next;
  }
}
