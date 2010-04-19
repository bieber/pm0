#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
#include "codes.h"
#include "symtable.h"
 
#define USAGE "Usage: parser [-q] infile"
 
/*===============*/
//---Globals-----//
 
// I/O vars
FILE* fin;
int verbose = 1;
int tokenNum = 0;
 
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

//A disjoint set to keep track of the parenthood of scopes
int scopeParent[MAX_CODE_LENGTH];
 
//----------------//
/*================*/
 
//Input function
void readToken();

//Symbol table helper functions
//Each one will store results to the pointers provided, and return true or
//false to indicate success or failure
int findVar(char* symbol, int scope, int* l, int* m);
int findConst(char* symbol, int scope, int* m);
int findFunc(char* symbol, int scope, int* l, int* m);
 
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
 
  int i;

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

  //Initializing the disjoint set for scopes
  for(i = 0; i < MAX_CODE_LENGTH; i++)
    scopeParent[i] = 0;

  //Fetching the first token
  readToken();
  
  //Parsing the program
  program();
 
  //Generating code
  printCode();

  //END:

  deleteTable(symTable);

  return 0;
  
}
  
/***********************/
/* Grammar Functions   */
/***********************/
 
/***************************/
/* program ::= block "."   */
/***************************/
void program(){
  block();
  
  if(currentToken != periodsym){
    if(currentToken != nulsym)
      throwError(WRONG_SYM_AFTER_STATE);
    else
      throwError(PERIOD_EXPEC);
  }
}
 
/***********************************************************/
/* block ::= const-declaration var-declaration statement   */
/***********************************************************/
void block(){

  int jmpLoc = reserveCode();

  int thisScope;
  int numVars = 0;
  char tempSymbol[MAX_IDENT_LENGTH + 1];
 
  if(currentToken == constsym){
    /***************************************************************/
    /* const-declaration ::= [ "const" ident "=" number            */
    /*                               {"," ident "=" number} ";"]   */
    /***************************************************************/
    do{
      readToken();
      
      if(currentToken != identsym){
        throwError(ID_FOLLOW_CONST_VAR_PROC);
      }else{
        strcpy(tempSymbol, tokenVal.string);
      }
 
      readToken();
      
      if(currentToken != eqsym){
        if(currentToken == becomessym)
          throwError(EQ_NOT_BECOMES);
        else
          throwError(EQ_FOLLOW_ID);
      }
      
      readToken();
    
      if(currentToken != numbersym){
        throwError(NUM_FOLLOW_EQ);
      }else{
        insertSymbol(symTable, newSymbol(CONST, tempSymbol, scope, 0,
                                         tokenVal.numeric));
      }
    
      readToken();
    }while(currentToken == commasym);
    
    if(currentToken != semicolonsym)
      throwError(SEMICOL_COMMA_MISS);
 
    readToken();
  }
  
  if(currentToken == intsym){
    /*******************************************************/
    /* var-declaration ::= ["int" ident {"," ident} ";"] */
    /*******************************************************/
    do{
      readToken();
      
      if(currentToken != identsym){
        throwError(ID_FOLLOW_CONST_VAR_PROC);
      }else{
        insertSymbol(symTable, newSymbol(VAR, tokenVal.string,
                                         scope, BASE_OFFSET + numVars++, 0));
      }
        
      readToken();
    }while(currentToken == commasym);
    
    if(currentToken != semicolonsym)
      throwError(SEMICOL_COMMA_MISS);
      
    readToken();
  }
  
  //Saving the scope before moving into procedure declarations
  thisScope = scope;

  while(currentToken == procsym){
    /**********************************************************************/
    /* proc-declaration ::= {"procedure" ident ";" block ";"} statement   */
    /**********************************************************************/

    readToken();
    
    if(currentToken != identsym)
      throwError(ID_FOLLOW_CONST_VAR_PROC);

    //Storing the function name in the symbol table
    insertSymbol(symTable, newSymbol(FUNC, tokenVal.string, thisScope, 
                                     genLabel(), 0));

    
    readToken();
    
    if(currentToken != semicolonsym)
      throwError(SEMICOL_COMMA_MISS);
      
    readToken();

    //Incrementing the scope for our new function and saving its parent
    //in the disjoint set
    scope++;
    scopeParent[scope] = thisScope;

    block();
    
    if(currentToken != semicolonsym)
      throwError(WRONG_SYM_AFTER_PROC);
    
    readToken();
  }

  //Restoring the scope
  scope = thisScope;

  backPatch(jmpLoc, JMP, 0, genLabel());
  genCode(INC, 0, BASE_OFFSET + numVars);
 
  statement();
  
  genCode(OPR, 0, RET);
 
  return;
}
 
/******************************************************************************/
/* statement ::= [ ident ":=" expression                                      */
/*               | "syaw" <ident>                                             */
/*               | "snga'i" statement {";" statement} "fpe'"                  */
/*               | "txo" condition "tsakrr" statement ["txokefyaw" statement] */
/*               | "tengkrr" condition "si" statement                         */
/*               | "mi" <ident>                                               */
/*               | "wrrpa" <ident>                                            */
/*               | e ]                                                        */
/******************************************************************************/
void statement(){
  int l;
  int m;
  symTableEntry* symbol;
  int tempLabels[2];
 
  if(currentToken == identsym){
    symbol = findSymbol(symTable, VAR, tokenVal.string, scope);
    if(!findVar(tokenVal.string, scope, &l, &m)){
      if(findConst(tokenVal.string, scope, &m))
        throwError(CANNOT_ASSIGN_TO_CONST_OR_PROC);
      else
        throwError(UNDEC_ID);
    }
    
    readToken();
    
    if(currentToken != becomessym)
      throwError(ASSIGN_EXPEC);
    
    readToken();
    
    expression();
 
    genCode(STO, l, m);
  }
  else if(currentToken == syawsym){
    readToken();
    
    if(currentToken != identsym)
      throwError(ID_FOLLOW_SYAW);

    if(!findFunc(tokenVal.string, scope, &l, &m))
      throwError(UNDEC_ID);

    genCode(CAL, l, m);
    
    readToken();
  }
  else if(currentToken == sngaisym){ // 'beginsym'
    readToken();
    
    statement();
    
    while(currentToken == semicolonsym){
      readToken();
      
      statement();
    }
      
    if(currentToken != fpesym && 
        (currentToken != identsym && currentToken != sngaisym
        && currentToken != txosym && currentToken != tengkrrsym)) // 'endsym'
      //throwError(WRONG_SYM_AFTER_STATE);
      throwError(SEMICOL_OR_RBRACK_EXPEC);
    else if(currentToken == identsym || currentToken == sngaisym
        || currentToken == txosym || currentToken == tengkrrsym)
      throwError(SEMICOL_BW_STATE_MISS);
    
    readToken();
  }
  else if(currentToken == txosym){ // 'ifsym'
    tempLabels[1] = 0;

    readToken();
    
    condition();
    
    tempLabels[0] = reserveCode(); //Conditional jump will go here
 
    if(currentToken != tsakrrsym) // 'thensym'
      throwError(TSAKRR_EXPEC);
    
    readToken();
    
    statement();

    if(currentToken == txokefyawsym){
      tempLabels[1] = reserveCode();
      readToken();
    }
 
    backPatch(tempLabels[0], JPC, 0, genLabel());
    
    if(tempLabels[1]){
      statement();
      backPatch(tempLabels[1], JMP, 0, genLabel());
    }

  }
  else if(currentToken == tengkrrsym){ // 'whilesym'
 
    readToken();
    
    tempLabels[0] = genLabel(); //Jump back up to here at the end of the loop
 
    condition();
 
    tempLabels[1] = reserveCode(); //Stick the conditional jump here
 
    if(currentToken != sisym) // 'dosym'
      throwError(SI_EXPEC);
    
    readToken();
      
    statement();
    
    genCode(JMP, 0, tempLabels[0]);
    backPatch(tempLabels[1], JPC, 0, genLabel());
  }
  else if(currentToken == misym){ // Input
    genCode(SIO, 0, 2);
    
    readToken();
    
    if(currentToken == identsym){
      if(findVar(tokenVal.string, scope, &l, &m))
        genCode(STO, l, m);
      else if(findConst(tokenVal.string, scope, &m)
              || findFunc(tokenVal.string, scope, &l, &m))
        throwError(CANNOT_STORE_IN_CONST_OR_PROC);
      else
        throwError(UNDEC_ID);
      
      readToken();
    }
  }
  else if(currentToken == wrrpasym){ // Output
    readToken();
    
    if(currentToken == identsym){
      if(findVar(tokenVal.string, scope, &l, &m))
        genCode(LOD, l, m);
      else if(findConst(tokenVal.string, scope, &m))
        genCode(LIT, 0, m);
      else
        throwError(UNDEC_ID);
      
      readToken();
    }
    
    genCode(SOI, 0, 1);
  }
  else if(currentToken == fpesym){
    return;
  }
  else{
    if(currentToken == periodsym)
      throwError(RBRACK_EXPEC_AT_END);
    else
      throwError(STATEMENT_EXPEC);
  }
  
  return;
}
 
/***************************************************/
/* condition ::= "odd" expression                  */
/*               | expression rel-op expression    */
/***************************************************/
void condition(){
  token operation;
 
  if(currentToken == oddsym){
    readToken();
    
    expression();

    genCode(OPR, 0, ODD);
  }
  else{
    expression();
    
    /*******************************************/
    /* rel-op ::= "="|"!="|"<"|"<="|">"|">="   */
    /*******************************************/
    if(currentToken != eqsym
       && currentToken != neqsym
       && currentToken != lessym
       && currentToken != leqsym
       && currentToken != gtrsym
       && currentToken != geqsym){

      if(currentToken == becomessym)
        throwError(EQ_NOT_BECOMES);
 
      throwError(REL_OP_EXPEC);
 
    }else{
      operation = currentToken;
    }
    
    readToken();
    
    expression();
 
    if(operation == eqsym)
      genCode(OPR, 0, EQL);
    else if(operation == neqsym)
      genCode(OPR, 0, NEQ);
    else if(operation == lessym)
      genCode(OPR, 0, LSS);
    else if(operation == leqsym)
      genCode(OPR, 0, LEQ);
    else if(operation == gtrsym)
      genCode(OPR, 0, GTR);
    else if(operation == geqsym)
      genCode(OPR, 0, GEQ);
 
  }
  
  return;
}
 
/******************************************************/
/* expression ::= [ "+"|"-" ] term {("+"|"-") term}   */
/******************************************************/
void expression(){
  int minus = 0; //A flag to negate the expression if we need to
  token operator;
 
  if(currentToken != plussym && currentToken != minussym
     && currentToken != lparentsym && currentToken != identsym
     && currentToken != numbersym)
    throwError(SYMBOL_CANNOT_BEGIN_THIS_EXP);

  if(currentToken == minussym)
    minus = 1;
 
  if(currentToken == plussym || currentToken == minussym)
    readToken();
  
  if(currentToken == procsym)
    throwError(EXP_CANNOT_CONTAIN_PROC_IDENT);
  
  term();
  
  if(minus)
    genCode(OPR, 0, NEG);

  while(currentToken == plussym || currentToken == minussym){
    operator = currentToken;
    
    readToken();
    
    if(currentToken == procsym)
      throwError(EXP_CANNOT_CONTAIN_PROC_IDENT);
    
    term();
 
    if(operator == plussym)
      genCode(OPR, 0, ADD);
    else if(operator == minussym)
      genCode(OPR, 0, SUB);
  }
 
  return;
}
 
/****************************************/
/* term ::= factor {("*"|"/") factor}   */
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
/* factor ::= ident | number | "(" expression ")"   */
/****************************************************/
void factor(){
  int l;
  int m;
  symTableEntry* symbol;
  if(currentToken == identsym){
    //First looking for a constant that matches
    symbol = findSymbol(symTable, CONST, tokenVal.string, scope);
    if(findConst(tokenVal.string, scope, &m)){
      genCode(LIT, 0, m);
    }else if(findVar(tokenVal.string, scope, &l, &m)){
      //If that fails, looking for a variable
        genCode(LOD, l, m);
    }else{
        throwError(UNDEC_ID);
    }
    readToken();
  }else if(currentToken == numbersym){
    genCode(LIT, 0, tokenVal.numeric);
    readToken();
  }else if(currentToken == lparentsym){
    readToken();
    
    expression();
    
    if(currentToken != rparentsym)
      throwError(RPAREN_MISS);
    else
      readToken();
  }
  else
    throwError(PREC_FACTOR_CANNOT_BEGIN_SYM);
  
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

  tokenNum++;
 
}
 
void throwError(errorCode code){
  printf("Error #%d at token %d: ", code, tokenNum);

  switch(code){
  case(EQ_NOT_BECOMES): // Used
    printf("Use = instead of :=.\n");
    break;
  case(NUM_FOLLOW_EQ): // Used
    printf("= must be followed by a number.\n");
    break;
  case(EQ_FOLLOW_ID): // Used
    printf("Identifier must be followed by a number.\n");
    break;
  case(ID_FOLLOW_CONST_VAR_PROC): // Used
    printf("const, var, procedure must be followed by identifier.\n");
    break;
  case(SEMICOL_COMMA_MISS): // Used
    printf("Semicolon or comma missing.\n");
    break;
  case(WRONG_SYM_AFTER_PROC): // Used
    printf("Incorrect symbol after procedure declaration.\n");
    break;
  case(STATEMENT_EXPEC): // Used
    printf("Statement expected.\n");
    break;
  case(WRONG_SYM_AFTER_STATE): // Used
    printf("Incorrect symbol after statement part in block.\n");
    break;
  case(PERIOD_EXPEC): // Used
    printf("Period expected.\n");
    break;
  case(SEMICOL_BW_STATE_MISS): // Used
    printf("Semicolon between statements missing.\n");
    break;
  case(UNDEC_ID): // Used
    printf("Undeclared identifier.\n");
    break;
  case(CANNOT_ASSIGN_TO_CONST_OR_PROC): // Used
    printf("Assignment to constant or procedure is not allowed.\n");
    break;
  case(ASSIGN_EXPEC): // Used
    printf("Assignment operator expected.\n");
    break;
  case(ID_FOLLOW_SYAW): // Used
    printf("syaw must be followed by an identifier.\n");
    break;
  case(CONST_OR_VAR_CALL_USELESS): // NO
    printf("Call of a constant or variable is meaningless.\n");
    break;
  case(TSAKRR_EXPEC): // Used
    printf("tsakrr expected.\n");
    break;
  case(SEMICOL_OR_RBRACK_EXPEC): // Used
    printf("Semicolon or } expected.\n");
    break;
  case(SI_EXPEC): // Used
    printf("si expected.\n");
    break;
  case(WRONG_SYM_FOLLOWING_STATE): // Used parallel to SEMICOL_OR_RBRACK_EXPEC
    printf("Incorrect symbol following statement.\n");
    break;
  case(REL_OP_EXPEC): // Used
    printf("Relational operator expected.\n");
    break;
  case(EXP_CANNOT_CONTAIN_PROC_IDENT): // Used
    printf("Expression must not contain a procedure identifier.\n");
    break;
  case(RPAREN_MISS): // Used
    printf("Right parenthesis missing.\n");
    break;
  case(PREC_FACTOR_CANNOT_BEGIN_SYM): // Used
    printf("The preceding factor cannot begin with this symbol.\n");
    break;
  case(SYMBOL_CANNOT_BEGIN_THIS_EXP):
    printf("An expression cannot begin with this symbol.\n");
    break;
  case(NUMBER_TOO_LARGE): // NO?
    printf("This number is too large.\n");
    break;
  case(RBRACK_EXPEC_AT_END):
    printf("} expected at the end of the program.\n");
    break;
  case(CANNOT_STORE_IN_CONST_OR_PROC):
    printf("Cannot store a value in a constant or procedure.\n");
    break;
  default:
    printf("Improper error code.\n");
    break;
  }
  deleteTable(symTable);
  abort();
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
 
int findVar(char* symbol, int scope, int* l, int* m){
  
  int level = 0;
  symTableEntry* result = NULL;
  
  while(scope >= 0){
    result = findSymbol(symTable, VAR, symbol, scope);
    
    if(result){
      *l = level;
      *m = result->offset;
      return 1;
    }else{
      if(scope == 0)
        return 0;
      scope = scopeParent[scope];
      level++;
    }
  }  

  return 0;

}

int findConst(char* symbol, int scope, int* m){
  
  symTableEntry* result = NULL;

  while(scope >= 0){
    result = findSymbol(symTable, CONST, symbol, scope);

    if(result){
      *m = result->value;
      return 1;
    }else{
      if(scope == 0)
        return 0;
      scope = scopeParent[scope];
    }

  }

  return 0;

}

int findFunc(char* symbol, int scope, int* l, int* m){

  symTableEntry* result = NULL;

  result = findSymbol(symTable, FUNC, symbol, scope);

  if(result){
    *m = result->offset;
    *l = 0;
    return 1;
  }

  result = findSymbol(symTable, FUNC, symbol, scopeParent[scope]);

  if(result){
    *m = result->offset;
    *l = 1;
    return 1;
  }

  return 0;

}
