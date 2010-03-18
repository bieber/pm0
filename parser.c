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
void throwError(errorCode code);

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
  
/*void program(){
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
}*/

/***********************/
/*  Grammar Functions  */
/***********************/

/***************************/
/*  program ::= block "."  */
/***************************/
void program(){
  block();
  
  if(currentToken != periodsym)
    printf("Period expected.\n");
}

/***********************************************************/
/*  block ::= const-declaration var-declaration statement  */
/***********************************************************/
void block(){
  if(currentToken == constsym){
    /***************************************************************/
    /*  const-declaration ::= [ "const" ident "=" number           */
    /*                                {"," ident "=" number} ";"]  */
    /***************************************************************/
    do{
      readToken();
      
      if(currentToken != identsym)
        printf("const must be followed by identifier.\n");
    
      readToken();
      
      if(currentToken != eqsym){
        if(currentToken == becomessym)
          printf("Use = instead of :=.\n");
        else
          printf("Identifier must be followed by =.\n");
      }
      
      readToken();
    
      if(currentToken != numbersym)
        printf("= must be followed by a number.\n");
    
      readToken();
    }while(currentToken == commasym);
    
    readToken();
    
    if(currentToken != semicolonsym)
      printf("Semicolon missing after const-declaration.\n");
  }
  
  if(currentToken == intsym){
    /*******************************************************/
    /*  var-declaration ::= ["int" ident {"," ident} ";"]  */
    /*******************************************************/
    do{
      readToken();
      
      if(currentToken != identsym)
        printf("var must be followed by identifier.\n");
        
      readToken();
    }while(currentToken == commasym);
    
    if(currentToken != semicolonsym)
      printf("Semicolon missing after var-declaration.\n");
      
    readToken();
  }
  
  while(currentToken == procsym){
    /**********************************************************************/
    /*  proc-declaration ::= {"procedure" ident ";" block ";"} statement  */
    /**********************************************************************/
    readToken();
    
    if(currentToken != identsym)
      printf("procedure must be followed by identifier.\n");
    
    readToken();
    
    if(currentToken != semicolonsym)
      printf("Semicolon missing in procedure before block.\n");
      
    readToken();
    
    block();
    
    if(currentToken != semicolonsym)
      printf("Semicolon mission in procedure after block.\n");
    
    readToken();
  }
  
  statement();
  
  return;
}

/******************************************************************************/
/* statement ::= [ ident ":=" expression                                      */
/*               | "syaw" ident                                               */
/*               | "snga'i" statement {";" statement} "fpe'"                  */
/*               | "txo" condition "tsakrr" statement ["txokefyaw" statement] */
/*               | "tengkrr" condition "si" statement                         */
/*               | "mi" ident                                                 */
/*               | "wrrpa" ident                                              */
/*               | e ]                                                        */
/******************************************************************************/
void statement(){
  if(currentToken == identsym){
    readToken();
    
    if(currentToken != becomessym)
      printf("Error, from STATEMENT, not becomessym\n");
      
    readToken();
    
    expression();
  }
  else if(currentToken == syawsym){ // 'callsym'
    readToken();
    
    if(currentToken != identsym)
      printf("syaw must be followed by an identifier.\n");
   
    readToken();
  }
  else if(currentToken == sngaisym){ // 'beginsym'
    readToken();
    
    statement();
    
    while(currentToken == semicolonsym){
      readToken();
      
      statement();
    }
    
    if(currentToken != fpesym) // 'endsym'
      printf("Incorrect symbol following statement\n");
    
    readToken();
  }
  else if(currentToken == txosym){ // 'ifsym'
    readToken();
    
    condition();
    
    if(currentToken != tsakrrsym) // 'thensym'
      printf("tsakrr expected.\n");
    
    readToken();
    
    statement();
  }
  else if(currentToken == tengkrrsym){ // 'whilesym'
    readToken();
    
    condition();
    
    if(currentToken != sisym) // 'dosym'
      printf("si expected.\n");
    
    readToken();
      
    statement();
  }
  
  return;
}

/***************************************************/
/*  condition ::= "odd" expression                 */
/*                 | expression rel-op expression  */
/***************************************************/
void condition(){
  if(currentToken == oddsym){
    readToken();
    
    expression();
  }
  else{
    expression();
    
    /*******************************************/
    /*  rel-op ::= "="|"!="|"<"|"<="|">"|">="  */
    /*******************************************/
    if(currentToken != eqsym 
        || currentToken != neqsym 
        || currentToken != lessym
        || currentToken != leqsym
        || currentToken != gtrsym
        || currentToken != geqsym)
      printf("Relational operator expected\n");
    
    readToken();
    
    expression();
  }
  
  return;
}

/******************************************************/
/*  expression ::= [ "+"|"-" ] term {("+"|"-") term}  */
/******************************************************/
void expression(){
  if(currentToken == plussym || currentToken == minussym)
    readToken();
    
  term();
  
  while(currentToken == plussym || currentToken == slashsym){
    readToken();
    
    term();
  }
  
  return;
}

/****************************************/
/*  term ::= factor {("*"|"/") factor}  */
/****************************************/
void term(){
  factor();
  
  while(currentToken == multsym || currentToken == slashsym){
    readToken();
    
    factor();
  }
  
  return;
}

/****************************************************/
/*  factor ::= ident | number | "(" expression ")"  */
/****************************************************/
void factor(){
  if(currentToken == identsym)
    readToken();
  else if(currentToken == numbersym)
    readToken();
  else if(currentToken == lparentsym){
    readToken();
    
    expression();
    
    if(currentToken != rparentsym)
      printf("Right parenthesis missing.\n");
  }
  else
    printf("Error, from FACTOR, generic error\n");
  
  return;
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

void throwError(errorCode code){
  
}
