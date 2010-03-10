#include "codes.h"
#include "symtable.c"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SYMBOL_TABLE_SIZE 

#define USAGE "Usage: parser [-q] infile\n"

void program(struct symbol* symbolList);
void block(struct symbol* symbolList);
void statement(struct symbol* symbolList);
void condition(struct symbol* symbolList);
void expression(struct symbol* symbolList);
void term(struct symbol* symbolList);
void factor(struct symbol* symbolList);

int main(int argc, char* argv[])
{
  FILE* fin;
  int verbose = 1;
  int fileArg = 1;
  struct tokenList* tokenTable
  struct symbol* symbolList;
  struct symbol* tempList;
  int L = 0, M = 0;
  
  //Checking command-line arguments
  if(argc < 2 || argc > 3){
    printf("%s\n", USAGE);
    return 1;
  }
  
  if(argc == 0){
    fin = STDIN;
  }
  else if(argc == 1){
    if(strcmp(argv[1], "-q") == 0)
      verbose = 0;
    fin = STDIN;
  }
  else if(argc == 2){
    fileArg = 1;
    fin = fopen(argv[fileArg], "r");
    tokenTable = system("./scanner -q fin");
  }
  //Turning off the verbose flag, if necessary
  else if(argc == 3){
    fileArg = 2;
    if(strcmp(argv[1], "-q") == 0)
      verbose = 0;
    
    fin = fopen(argv[fileArg], "r");
    tokenTable = system("./scanner -q fin");
  }
  
  tempList = symbolList;
  
  while(tokenTable)
  {
    tempList.kind = tokenTable.symbol;
    strcpy(tempList.name, tokenTable.lexeme);
    
    if(tempList.kind == numbersym)
      tempList.val = atoi(tokenTable.lexeme);
      
    if(tempList.kind == identsym){
      tempList.level = ;
      tempList.addr = ;
    }
    
    if(tempList.kind == procsym){
      tempList.level = ;
      tempList.addr = ;
    }
    
    tokenTable = tokenTable.next;
    tempList = tempList.next;
  }
  
}
  
  void program(struct symbol* symbolList){
    block(symbolList);
    
    if(symbolList.kind != periodsym)
      printf("Period expected.\n");
  }
  
  void block(struct symbol* symbolList){
    if(symbolList.kind == constsym){
      while(symbolList.kind != commasym){
        symbolList = symbolList.next;
        
        if(symbolList.kind != identsym)
          printf("const must be followed by identifier.\n");
      
        symbolList = symbolList.next;
        
        if(symbolList.kind != eqsym){
          if(symbolList.kind == becomessym)
            printf("Use = instead of :=.\n");
          else
            printf("Identifier must be followed by =.\n");
        }
        
        symbolList = symbolList.next;
      
        if(symbolList.kind != numbersym)
          printf("= must be followed by a number.\n");
      
        symbolList = symbolList.next;
      }
      
      symbolList = symbolList.next;
      
      if(symbolList.kind != semicolonsym)
        printf("Semicolon missing.\n");
    }
    
    if(symbolList.kind == intsym){
      while(symbolList.kind == commasym){
        symbolList = symbolList.next;
        
        if(symbolList.kind != identsym)
          printf("var must be followed by identifier\n");
          
        symbolList = symbolList.next;
      }
      
      if(symbolList.kind != semicolonsym)
        printf("Error, from BLOCK, not semicolonsym\n");
        
      symbolList = symbolList.next;
    }
    
    while(symbolList.kind == procsym){
      symbolList = symbolList.next;
      
      if(symbolList.kind != identsym)
        printf("procedure must be followed by identifier.\n");
      
      symbolList = symbolList.next;
      
      if(symbolList.kind != semicolonsym)
        printf("Error, from BLOCK, not semicolonsym\n");
        
      symbolList = symbolList.next;
      
      block(symbolList);
      
      if(symbolList.kind != semicolonsym)
        printf("Error, from BLOCK, not semicolonsym\n");
      
      symbolList = symbolList.next;
    }
    
    statement(symbolList);
    
    return;
  }
  
  void statement(struct symbol* symbolList){
    if(symbolList.kind == identsym){
      symbolList = symbolList.next;
      
      if(symbolList.kind != becomessym)
        printf("Error, from STATEMENT, not becomessym\n");
        
      symbolList = symbolList.next;
      
      expression(symbolList);
    }
    else if(symbolList.kind == syawsym){ // 'callsym'
      symbolList = symbolList.next;
      
      if(symbolList.kind != identsym)
        printf("syaw must be followed by an identifier.\n");
      
      symbolList = symbolList.next;
    }
    else if(symbolList.kind == sngaisym){ // 'beginsym'
      symbolList = symbolList.next;
      
      statement(symbolList);
      
      while(symbolList.kind == semicolonsym){
        symbolList = symbolList.next;
        
        statement(symbolList);
      }
      
      if(symbolList.kind != fpesym) // 'endsym'
        printf("Incorrect symbol following statement\n");
      
      symbolList = symbolList.next;
    }
    else if(symbolList.kind == ifsym){
      symbolList = symbolList.next;
      
      condition(symbolList);
      
      if(symbolList.kind != tsakrrsym) // 'thensym'
        printf("tsakrr expected.\n");
      
      symbolList = symbolList.next;
      
      statement(symbolList);
    }
    else if(symbolList.kind == tengkrrsym){ // 'whilesym'
      symbolList = symbolList.next;
      
      condition(symbolList);
      
      if(symbolList.kind != sisym) // 'dosym'
        printf("si expected.\n");
      
      symbolList = symbolList.next;
      
      statement(symbolList);
    }
    
    return;
  }
  
  void condition(struct symbol* symbolList){
    if(symbolList.kind == oddsym){
      symbolList = symbolList.next;
      
      expression(symbolList);
    }
    else
    {
      expression(symbolList);
      
      if(symbolList.kind != RELATION)
        printf("Relational operator expected\n");
      
      symbolList = symbolList.next;
      
      expression(symbolList);
    }
    
    return;
  }
  
  void expression(struct symbol* symbolList){
    if(symbolList.kind == plussym || symbolList.kind == minussym)
      symbolList = symbolList.next;
      
    term(symbolList);
    
    while(symbolList == plussym || symbolList == slashsym){
      symbolList = symbolList.next;
      
      term(symbolList);
    }
    
    return;
  }
  
  void term(struct symbol* symbolList){
    factor(symbolList);
    
    while(symbolList.kind == multsym || symbolList.kind == slashsym){
      symbolList = symbolList.next;
      
      factor(symbolList);
    }
    
    return;
  }
  
  void factor(struct symbol* symbolList){
    if(symbolList.kind == identsym)
      symbolList = symbolList.kind;
    else if(symbolList.kind == numbersym)
      symbolList = symbolList.kind;
    else if(symbolList.kind == lparensym){
      symbolList = symbolList.next;
      
      expression(symbolList);
      
      if(symbolList != rparensym)
        printf("Right parenthesis missing.\n");
    }
    else
      printf("Error, from FACTOR, generic error\n");
    
    return;
  }