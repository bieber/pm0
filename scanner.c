// Robert Bieber & Paul Gatterdam
// COP3402 - Scanner
// 2-22-10

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

  //Initializing manual members
  machine.transition = &transition;
  machine.line = 1;
  machine.column = 1;

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
    fin = fopen(argv[fileArg], "r");
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
      for(i = 0; i < (MAX_IDENT_LENGTH + 1) - strlen(currToken->lexeme); i++)
        printf(" ");
      printf("%d\n", (int)(currToken->symbol));
      currToken=currToken->next;
    }
    
  }

  //Printing the final output line, regardless of verbose flag
  currToken = tokenTable;
  while(currToken){
    printf("%d ", (int)currToken->symbol);
    if(currToken->printLexeme)
      printf("%s ", currToken->lexeme);
    currToken = currToken->next;
  }
  printf("\n");
  
  //Freeing memory
  deleteList(tokenTable);

  return 0;
}
 
/*
===== Mini State Map =====
| 0: Entry State
| 1: Identifier Loop
| 2: Number Loop
| 10-19: Comment checking
| 20-29: Checking for :=
| 30-39: Checking for <=
| 40-49: Checking for >=
| 100-199: Reserved words beginning with s
| 200-299: Reserved words beginning with f
| 300-399: Reserved words beginning with t
| 400-499: Reserved words beginning with m
| 500-599: Reserved words beginning with w
| 600-699: Reserved words beginning with p
| 700-799: Reserved words beginning with i
| 800-899: Reserved words beginning with o
| 900-999: Reserved words beginning with c
==========================
*/

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
        return 100;
      }else if(input == 'f'){
        return 200;
      }else if(input == 't'){
        return 300;
      }else if(input == 'm'){
        return 400;
      }else if(input == 'w'){
        return 500;
      }else if(input == 'p'){
        return 600;
      }else if(input == 'i'){
        return 700;
      }else if(input == 'o'){
        return 800;
      }else if(input == 'c'){
        return 900;
      }else{
        return 1;
      }
    }else if(isdigit(input)){
      this->accept = 1;
      this->retVal.numeric = numbersym;
      this->retVal.retString = 0;
      return 2;
  //---------Cases for symbols---------------//    
    }else{
      if(input == '+'){ // Found a plus
        this->retVal.numeric = plussym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt=1;
      }else if(input == '-'){ // Found a minus
        this->retVal.numeric = minussym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt=1;
      }else if(input == '*'){ // Found a multiply
        this->retVal.numeric = multsym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt=1;
      }else if(input == '/'){ // Go to state 10, check for / or /*
        this->retVal.numeric = slashsym;
        this->retVal.retString = 0;
        this->rewind = 0;
        return 10;
      }else if(input == '('){ // Found an opening parenthesis
        this->retVal.numeric = lparentsym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt = 1;
      }else if(input == ')'){ // Found a closing parenthesis
        this->retVal.numeric = rparentsym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt = 1;
      }else if(input == '='){ // Found an equals
        this->retVal.numeric = eqsym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt = 1;
      }else if(input == ':'){ // Go to state 20, checking for := (becomes)
        return 20;
      }else if(input == ','){ // Found a comma
        this->retVal.numeric = commasym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt = 1;
      }else if(input == '.'){ // Found a period
        this->retVal.numeric = periodsym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt = 1;
      }else if(input == '<'){ // Go to state 30, checking for < or <=
        return 30;
      }else if(input == '>'){ // Go to state 40, checking for > or >=
        return 40;
      }else if(input == ';'){ // Found a semicolon
        this->retVal.numeric = semicolonsym;
        this->retVal.retString = 0;
        this->accept = 1;
        this->rewind = 0;
        this->halt = 1;
      }else{
        printf("Error: Invalid Symbol %c at (%d, %d)\n", input, this->line,
               this->column);
      this->rewind = 0;
      rejectDFA(this);
      }
    }
    break;

  case 1:  //Handling generic identifiers
    if(isalnum(input)){
      return 1;
    }else{
      if(strlen(this->retVal.string) > MAX_IDENT_LENGTH){
       printf("Error: Identifier too long %s at (%d, %d)\n", 
              this->retVal.string, this->line, this->column);
       this->accept = 0;
       this->halt = 1;
      }
      else{
        this->retVal.numeric = identsym;
        this->retVal.retString = 1;
        this->accept = 1;
        this->rewind = 1;
        this->halt = 1;
      }
    }
    break;
  
  case 2: // Handling digits
    if(isdigit(input)){
      if(strlen(this->retVal.string) > MAX_NUMBER_LENGTH){
        printf("Error: Invalid numerical length at (%d, %d)\n",
               this->line, this->column);
        this->rewind = 0;
        rejectDFA(this);
      }
      else
        return 2;
    }else if(isalpha(input)){
      printf("Error: Invalid identifier %s at (%d, %d)\n", this->retVal.string,
             this->line, this->column);
      this->rewind = 0;
      rejectDFA(this);
    }else{
      this->retVal.numeric = numbersym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  //------------ Handling possibility of comments
  case 10:
    if(input == '*'){
      this->retVal.numeric = nulsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 0;
      return 11; //The next step in comment processing
    }else{
      //If it wasn't a star, we just got a /
      this->retVal.numeric = slashsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
 
  case 11:
    if(input == '*'){
      return 12;
    }else{
      return 11;
    }
    break;
    
  case 12:
    if(input == '/'){
      this->halt = 1;
    }else if(input == '*'){
      return 12;
    }else{
      return 11;
    }
    break;
//-------- Checking for :=
  case 20:
    if(input == '='){
      this->retVal.numeric = becomessym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 0;
      this->halt = 1;
    }else{
      printf("Error: Invalid Symbol %c at (%d, %d)\n", input, this->line,
             this->column);
      this->rewind=0;
      rejectDFA(this);
    }
    break;
 
//--------------- Checking for <=
  case 30:
    if(input == '='){
      this->retVal.numeric = leqsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 0;
      this->halt = 1;
    }else{
      this->retVal.numeric = lessym;
      this->retVal.retString = 0;
      this->rewind = 1;
      this->accept = 1;
      this->halt = 1;
    }
    break;
 
//---------------- Checking for >=
  case 40:
    if(input == '='){
      this->retVal.numeric = geqsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 0;
      this->halt = 1;
    }else{
      this->retVal.numeric = gtrsym;
      this->retVal.retString = 0;
      this->rewind = 1;
      this->accept = 1;
      this->halt = 1;
    }
    break;
  
  case 100: // Found s, looking for "si", "syaw", "snga'i" or identifier
    if(isalnum(input)){
      if(input == 'i'){
        this->accept = 1;
        return 101;
      }else if(input == 'y')
        return 102;
      else if(input == 'n')
        return 105;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 101: // Found i, finishing "si" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = sisym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 102: // Found y, looking for "syaw" or identifier
    if(isalnum(input)){
      if(input == 'a')
        return 103;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 103: // Found a, looking for "syaw" or identifier
    if(isalnum(input)){
      if(input == 'w'){
        this->accept = 1;
        return 104;
      }else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 104: // Found w, finishing "syaw" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = syawsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 105: // Found n, looking for "snga'i" or identifier
    if(isalnum(input)){
      if(input == 'g')
        return 106;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 106: // Found g, looking for "snga'i" or identifier
    if(isalnum(input)){
      if(input == 'a')
        return 107;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 107: // Found a, loooking for "snga'i" or identifier
    if(input == '\''){
      return 108;
    }else if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 108: // Found ', looking for "snga'i" or identifier
    if(isalnum(input)){
      if(input == 'i'){
        this->accept = 1;
        return 109;
      }else{
        printf("Error: Invalid Identifier %s at (%d, %d)\n",
               this->retVal.string, this->line, this->column);
        rejectDFA(this);
      }
    }else{
        if(this->retVal.string[strlen(this->retVal.string)-1] == '\n')
          this->retVal.string[strlen(this->retVal.string)-1] = '\0';
        printf("Error: Invalid Identifier %s at (%d, %d)\n", 
               this->retVal.string, this->line, this->column);
        rejectDFA(this);
    }
    break;
  
  case 109: // Found i, finishing "snga'i" or looking for identifier
    if(isalnum(input)){
      printf("Error: Invalid Identifier %s at (%d, %d)\n",
             this->retVal.string, this->line, this->column);
      rejectDFA(this);
    }else{
      this->retVal.numeric = sngaisym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 200: // Found f, looking for "fpe'" or identifier
    if(isalnum(input)){
      if(input == 'p')
        return 201;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 201: // Found p, looking for "fpe'" or identifier
    if(isalnum(input)){
      if(input == 'e')
        return 202;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 202: // Found e, looking for "fpe'" or identifier
    if(input == '\''){
      this->accept = 1;
      return 203;
    }else if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 203: // Found ', finishing "fpe'" or looking for identifier
    if(isalnum(input)){
      printf("Error: Invalid Identifier %s at (%d, %d)\n", this->retVal.string,
             this->line, this->column);
      rejectDFA(this);
    }else{
      this->retVal.numeric = fpesym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 300: // Found t, looking for "txo", "txokefyaw", "tsakrr", "tengkrr" or
            //identifier
    if(isalnum(input)){
      if(input == 'x')
        return 301;
      else if(input == 's')
        return 309;
      else if(input == 'e')
        return 314;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 301: // Found x, looking for "txo", "txokefyaw", or identifier
    if(isalnum(input)){
      if(input == 'o'){
        this->accept = 1;
        return 302;
      }else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 302: // Found o, finishing "txo" or looking for "txokefyaw" or identifier
    if(isalnum(input)){
      if(input == 'k')
        return 303;
      else
        return 1;
    }else{
      this->retVal.numeric = txosym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 303: // Found k, looking for "txokefyaw" or identifier
    if(isalnum(input)){
      if(input == 'e')
        return 304;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 304: // Found e, looking for "txokefyaw" or identifier
    if(isalnum(input)){
      if(input == 'f')
        return 305;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 305: // Found f, looking for "txokefyaw" or identifier
    if(isalnum(input)){
      if(input == 'y')
        return 306;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 306: // Found y, looking for "txokefyaw" or identifier
    if(isalnum(input)){
      if(input == 'a')
        return 307;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 307: // Found a, looking for "txokefyaw" or identifier
    if(isalnum(input)){
      if(input == 'w'){
        this->accept = 1;
        return 308;
      }else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 308: // Found w, finishing "txokefyaw" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = txokefyawsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 309: // Found s, looking for "tsakrr" or identifier
    if(isalnum(input)){
      if(input == 'a')
        return 310;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 310: // Found a, looking for "tsakrr" or identifier
    if(isalnum(input)){
      if(input == 'k')
        return 311;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 311: // Found k, looking for "tsakrr" or identifier
    if(isalnum(input)){
      if(input == 'r')
        return 312;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 312: // Found r, looking for "tsakrr" or identifier
    if(isalnum(input)){
      if(input == 'r'){
        this->accept = 1;
        return 313;
      }else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 313: // Found r, finishing "tsakrr" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = tsakrrsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 314: // Found e, looking for "tengkrr" or identifier
    if(isalnum(input)){
      if(input == 'n')
        return 315;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 315: // Found n, looking for "tengkrr" or identifier
    if(isalnum(input)){
      if(input == 'g')
        return 316;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 316: // Found g, looking for "tengkrr" or identifier
    if(isalnum(input)){
      if(input == 'k')
        return 317;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 317: // Found k, looking for "tengkrr" or identifier
    if(isalnum(input)){
      if(input == 'r')
        return 318;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 318: // Found r, looking for "tengkrr" or identifier
    if(isalnum(input)){
      if(input == 'r'){
        this->accept = 1;
        return 319;
      }else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 319: // Found r, finishing "tengkrr" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = tengkrrsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 400: // Found m, looking for "mi" or identifier
    if(isalnum(input)){
      if(input == 'i')
        return 401;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 401: // Found i, finishing "mi" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = misym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 500: // Found w, looking for "wrrpa" or identifier
    if(isalnum(input)){
      if(input == 'r')
        return 501;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 501: // Found r, looking for "wrrpa" or identifier
    if(isalnum(input)){
      if(input == 'r')
        return 502;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 502: // Found r, looking for "wrrpa" or identifier
    if(isalnum(input)){
      if(input == 'p')
        return 503;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 503: // Found p, looking for "wrrpa" or identifier
    if(isalnum(input)){
      if(input == 'a'){
        this->accept = 1;
        return 504;
      }else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 504: // Found a, finishing "wrrpa" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = wrrpasym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 600: // Found p, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'r')
        return 601;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 601: // Found r, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'o')
        return 602;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 602: // Found o, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'c')
        return 603;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 603: // Found c, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'e')
        return 604;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 604: // Found e, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'd')
        return 605;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 605: // Found d, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'u')
        return 606;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 606: // Found u, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'r')
        return 607;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 607: // Found r, looking for "procedure" or identifier
    if(isalnum(input)){
      if(input == 'e')
        return 608;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 608: // Found e, finishing "procedure" or looking for identifier
    if(isalnum(input)){
      this->accept = 1;
      return 1;
    }else{
      this->retVal.numeric = procsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 700: // Found i, looking for "int" or identifier
    if(isalnum(input)){
      if(input == 'n')
        return 701;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 701: // Found n, looking for "int" or identifier
    if(isalnum(input)){
      if(input == 't')
        return 702;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 702: // Found t, finishing "int" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = intsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
  
  case 800: // Found o, looking for "odd" or identifier
    if(isalnum(input)){
      if(input == 'd')
        return 801;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 801: // Found d, looking for "odd" or identifier
    if(isalnum(input)){
      if(input == 'd')
        return 802;
      else
        return 1;
    }else{
      this->retVal.numeric = identsym;
      this->retVal.retString = 1;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;
    
  case 802: // Found d, finishing "odd" or looking for identifier
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = oddsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->rewind = 1;
      this->halt = 1;
    }
    break;

  case 900: //Found a 'c', looking for "const"
    if(input == 'o'){
      return 901;
    }else if(isalnum(input)){
      return 1;
    }else{
      this->accept = 1;
      this->halt = 1;
      this->retVal.retString = 1;
    }
    break;  

  case 901:
    if(input == 'n'){
      return 902;
    }else if(isalnum(input)){
      return 1;
    }else{
      this->accept = 1;
      this->halt = 1;
      this->retVal.retString = 1;
    }
    break;

  case 902:
    if(input == 's'){
      return 903;
    }else if(isalnum(input)){
      return 1;
    }else{
      this->accept = 1;
      this->halt = 1;
      this->retVal.retString = 1;
    }
    break;

  case 903:
    if(input == 't'){
      return 904;
    }else if(isalnum(input)){
      return 1;
    }else{
      this->accept = 1;
      this->halt = 1;
      this->retVal.retString = 1;
    }
    break;
    
  case 904:
    if(isalnum(input)){
      return 1;
    }else{
      this->retVal.numeric = constsym;
      this->retVal.retString = 0;
      this->accept = 1;
      this->halt = 1;
    }
   
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
