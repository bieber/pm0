#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codes.h"
#include "symtable.h"

#define USAGE "Usage: parser [-q] infile"

/*===============*/
//---Globals-----//

// I/O vars
FILE* fin;
int verbose = 1;

//Parsing status vars
token currentToken;
union{
  int numeric;
  char string[MAX_IDENT_LENGTH + 1];
} tokenVal;

//The symbol table
symTableList** symTable;

//----------------//
/*================*/

//Input function
void readToken();

//Non-terminal parsing functions
void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();

int main(int argc, char* argv[]){

  //Checking command-line arguments
  if(argc > 3){
    printf("%s\n", USAGE);
    return 1;
  }else if(argc < 2){
    fin = stdin;
  }else if(argc == 2){
    if(strcmp(argv[1], "-q") == 0){
      verbose = 0;
      fin = stdin;
    }else{
      fin = fopen(argv[1], "r");
      if(!fin){
        printf("%s\n", USAGE);
        return 1;
      }
    }
  }else if(argc == 2){
    if(strcmp("-q", argv[1]) != 0){
      printf("%s\n", USAGE);
      return 1;
    }
    verbose = 0;
    fin = fopen(argv[2], "r");
  }

  //Initializing the symbol table
  symTable = newTable();

  //Fetching the first token
  readToken();
  
  //Parsing the program
  program();

  return 0;
  
}
  
void program(){
  block();  
}

void block(){
  while(currentToken != nulsym){
    printf("%d\n", currentToken);
    if(currentToken == identsym)
      printf("%s\n", tokenVal.string);
    if(currentToken == numbersym)
      printf("%d\n", tokenVal.numeric);
    readToken();
  }
}

void statement(){

}

void condition(){

}
  
void expression(){

}
  
void term(){

  factor();

}
  
void factor(){

}

void readToken(){
  
  //Making sure we still have input to read
  if(feof(fin)){
    currentToken = nulsym;
    return;
  }

  //Reading the new token in
  fscanf(fin, "%d ", (int*)(&currentToken));

  //Checking to see if we need to read anything else in
  if(currentToken == identsym){
    fscanf(fin, "%s ", tokenVal.string);
  }

  if(currentToken == numbersym){
    fscanf(fin, "%d ", &tokenVal.numeric);
  }

}
