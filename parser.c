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
int scope = 0;

//The code variables
int pc = 0;
instruction code[MAX_CODE_LENGTH];

//----------------//
/*================*/

//Input function
void readToken();

//Code generation functions
void genCode(opcode op, int l, int m);
void backPatch(int location, opcode op, int l, int m);
int genLabel();
int reserveCode();
void printCode();

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

  //Generating code
  printCode();

  return 0;
  
}
  
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

  int numVars = 0;
  scope++;
  char tempSymbol[MAX_IDENT_LENGTH + 1];

  if(currentToken == constsym){
    /***************************************************************/
    /*  const-declaration ::= [ "const" ident "=" number           */
    /*                                {"," ident "=" number} ";"]  */
    /***************************************************************/
    do{
      readToken();
      
      if(currentToken != identsym){
        printf("const must be followed by identifier.\n");
      }else{
        strcpy(tempSymbol, tokenVal.string);
      }
    
      readToken();
      
      if(currentToken != eqsym){
        if(currentToken == becomessym)
          printf("Use = instead of :=.\n");
        else
          printf("Identifier must be followed by =.\n");
      }
      
      readToken();
    
      if(currentToken != numbersym){
        printf("= must be followed by a number.\n");
      }else{
        insertSymbol(symTable, newSymbol(CONST, tempSymbol, scope, 0, 
                                         tokenVal.numeric));
      }
    
      readToken();
    }while(currentToken == commasym);
    
    if(currentToken != semicolonsym)
      printf("Semicolon missing after const-declaration.\n");

    readToken();
  }
  
  if(currentToken == intsym){
    /*******************************************************/
    /*  var-declaration ::= ["int" ident {"," ident} ";"]  */
    /*******************************************************/
    do{
      readToken();
      
      if(currentToken != identsym){
        printf("var must be followed by identifier.\n");
      }else{
        insertSymbol(symTable, newSymbol(VAR, tokenVal.string, 
                                         scope, BASE_OFFSET + numVars++, 0));
      }
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
  
  genCode(INC, 0, BASE_OFFSET + numVars);

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
  symTableEntry* symbol;

  if(currentToken == identsym){
    symbol = findSymbol(symTable, VAR, tokenVal.string, scope);
    if(!symbol)
      throwError(UNDEC_ID);

    readToken();
    
    if(currentToken != becomessym)
      printf("Error, from STATEMENT, not becomessym\n");
      
    readToken();
    
    expression();

    genCode(STO, 0, symbol->offset);
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
        && currentToken != neqsym 
        && currentToken != lessym
        && currentToken != leqsym
        && currentToken != gtrsym
        && currentToken != geqsym)
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
  int minus = 0; //A flag to negate the expression if we need to
  token operator;

  if(currentToken == minussym)
    minus = 1;

  if(currentToken == plussym || currentToken == minussym)
    readToken();
    
  term();
  
  while(currentToken == plussym || currentToken == slashsym){
    operator = currentToken;
    
    readToken();
    
    term();

    if(operator == plussym)
      genCode(OPR, 0, ADD);
    else if(operator == minussym)
      genCode(OPR, 0, SUB);
  }

  if(minus)
    genCode(OPR, 0, NEG);

  return;
}

/****************************************/
/*  term ::= factor {("*"|"/") factor}  */
/****************************************/
void term(){
  token tempToken;
  factor();
  
  while(currentToken == multsym || currentToken == slashsym){
    tempToken = currentToken;

    readToken();
    
    factor();

    if(tempToken == multsym)
      genCode(OPR, 0, MUL);
    else if(tempToken == slashsym)
      genCode(OPR, 0, DIV);
  }
  
  return;
}

/****************************************************/
/*  factor ::= ident | number | "(" expression ")"  */
/****************************************************/
void factor(){
  symTableEntry* symbol;
  if(currentToken == identsym){
    //First looking for a constant that matches
    symbol = findSymbol(symTable, CONST, tokenVal.string, scope);
    if(symbol){
      genCode(LIT, 0, symbol->value);
    }else{
      //If that fails, looking for a variable
      symbol = findSymbol(symTable, VAR, tokenVal.string, scope);
      if(symbol){
        genCode(LOD, 0, symbol->offset);
      }else{
        throwError(UNDEC_ID);
      }
    }
    readToken();
  }else if(currentToken == numbersym){
    genCode(LIT, 0, tokenVal.numeric);
    readToken();
  }else if(currentToken == lparentsym){
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

void genCode(opcode op, int l, int m){

  //Checking pc size for overflow
  if(pc == MAX_CODE_LENGTH){
    printf("ERROR: Code too long\n");
    return;
  }

  //Inserting code and incrementing pc
  code[pc].op = (int)op;
  code[pc].l = l;
  code[pc].m = m;
  pc++;

}

void backPatch(int location, opcode op, int l, int m){
  
  if(location >= MAX_CODE_LENGTH){
    printf("ERROR: Invalid code index\n");
    return;
  }

  code[location].op = (int)op;
  code[location].l = l;
  code[location].m = m;

}

int genLabel(){

  return pc;

}

int reserveCode(){
  
  //Checking to make sure we have room
  if(pc >= MAX_CODE_LENGTH){
    printf("ERROR: Code too long\n");
    return -1;
  }

  return pc++;

}

void printCode(){
  
  int i;
  for(i = 0; i < pc; i++)
    printf("%d %d %d\n", (int)code[i].op, code[i].l, code[i].m);
}
