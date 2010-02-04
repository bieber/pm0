#include <stdio.h>
#include <string.h>
#include "codes.h"

//MAX macro
#define MAX(x, y) (x>y?x:y)

//Usage
#define USAGE "Usage: vm [-q] infile\n"

//Machine constants
#define OVERFLOW  2 //Allocating a little extra room in arrays
#define MAX_LEXI_LEVELS 3

//Opcode strings
char* OPCODES[] =
{
  "",
  "lit",
  "opr",
  "lod",
  "sto",
  "cal",
  "inc",
  "jmp",
  "jpc",
  "soi",
  "sio"
};

int base(int levels, int bp, int stack[]);
void printState(int line, instruction op, int pc, int bp, int sp, int stack[],
                int arlist[]);
int execOp(int* sp, int* bp, int* pc, instruction ir, instruction code[], 
           int stack[], int arlist[], int* arcntr);
void printFile(instruction code[], int numInstructions);
int readFile(int argc, char* argv[], int* verbose, instruction code[]);

int main(int argc, char* argv[]){
  int running = 1;
  int verbose = 1;
  int numInstructions = 0;
  int lineRun;
  int arcntr = 0;

  //Memory
  int stack[(MAX_STACK_HEIGHT + OVERFLOW)];
  instruction code[MAX_CODE_LENGTH + OVERFLOW];
  int arlist[MAX_STACK_HEIGHT];

  //Registers
  int sp=0;
  int bp=1;
  int pc=0;
  instruction ir;

  //Initializing IR
  ir.op = 0;
  ir.l = 0;
  ir.m = 0;
  
  //Initializing stack
  stack[1] = 0;
  stack[2] = 0;
  stack[3] = 0;

  //Reading the input file
  numInstructions = readFile(argc, argv, &verbose, code);
  if(numInstructions < 0) //Exit with error if readFile returns invalid
    return 1;

  //Printing the program listing
  if(verbose)
    printFile(code, numInstructions);

  //Printing the initial state
  if(verbose)
    printState(-1, ir, pc, bp, sp, stack, arlist);
  
  //Execution loop
  while(running){  
    //Storing the current line for the stack trace
    lineRun = pc;
    
    //Fetch cycle
    ir = code[pc++];
    
    //Execution cycle
    if(!execOp(&sp, &bp, &pc, ir, code, stack, arlist, &arcntr))
      running = 0;
    
    //If the operation resulted in a stack overflow, complain and exit
    if(sp > MAX_STACK_HEIGHT){
      printf("STACK OVERFLOW\n");
      return 1;
    }

    //Printing the current status
    if(verbose)
      printState(lineRun, ir, pc, bp, sp, stack, arlist);
    
  }
  
  return 0;
}

int base(int levels, int bp, int stack[]){
  while(levels > 0){
    bp = stack[bp];
    levels--;
  }
  return bp;
}

void printState(int line, instruction op, int pc, int bp, int sp, int stack[], 
    int arlist[]){
  int i, k;
  
  //Printing header, if necessary
  if(line < 0)
    printf("                 %3s %3s %3s  Stack\n", "pc", "bp", "sp");

  //Printing the current line
  if(line < 0)
    printf("Initial Values   ");
  else
    printf("%3d %s %3d %3d  ", line, OPCODES[op.op], op.l, op.m);

  //Printing the register statuses
  printf("%3d %3d %3d  ", pc, bp, sp);

  //Printing the stack
  for(i=1, k=0; i<=MAX(sp, (bp+2)); i++){
    
    //Determining whether or not to insert a pipe before this element
    if(arlist[k] == i)
    {
      printf("| ");
      k++;
    }

    printf("%d ", stack[i]);
  }

  printf("\n");
}

int execOp(int* sp, int* bp, int* pc, instruction ir, instruction code[], 
           int stack[], int arlist[], int* arcntr){
  //One giant switch statement for all the opcodes
  switch((opcode)ir.op){
  case LIT:
    stack[++(*sp)] = ir.m;
    break;
    
  case OPR:  //Operators
    switch((operator)ir.m){
    case RET:
      if(*bp == 1) //Kill the simulation if we're at the base level
        return 0;
      *sp = *bp - 1;
      *pc = stack[*sp+3];
      *bp = stack[*sp+2];
      break;
      
    case NEG:
      stack[*sp] = -stack[*sp];
      break;
      
    case ADD:
      (*sp)--;
      stack[*sp] = stack[*sp] + stack[*sp+1];
      break;
      
    case SUB:
      (*sp)--;
      stack[*sp] = stack[*sp] - stack[*sp+1];
      break;
      
    case MUL:
      (*sp)--;
      stack[*sp] = stack[*sp] * stack[*sp+1];
      break;
      
    case DIV:
      (*sp)--;
      stack[*sp] = stack[*sp] / stack[*sp+1];
      break;
      
    case ODD:
      stack[*sp] = stack[*sp] % 2;
      break;
      
    case MOD:
      (*sp)--;
      stack[*sp] = stack[*sp] % stack[(*sp)+1];
      break;
    
    case EQL:
      (*sp)--;
      stack[*sp] = stack[*sp] == stack[*sp+1];
      break;
      
    case NEQ:
      (*sp)--;
      stack[*sp] = stack[*sp] != stack[*sp+1];
      break;
      
    case LSS:
      (*sp)--;
      stack[*sp] = stack[*sp] < stack[*sp+1];
      break;
      
    case LEQ:
      (*sp)--;
      stack[*sp] = stack[*sp] <= stack[*sp+1];
      break;
      
    case GTR:
      (*sp)--;
      stack[*sp] = stack[*sp] > stack[*sp+1];
      break;
      
    case GEQ:
      (*sp)--;
      stack[*sp] = stack[*sp] >= stack[*sp+1];
      break;
    }
    break;
    
  case LOD:
    stack[++*sp] = stack[base(ir.l, *bp, stack) + ir.m];
    break;
    
  case STO:
    stack[base(ir.l, *bp, stack) + ir.m] = stack[(*sp)--];
    break;
    
  case CAL:
    arlist[(*arcntr)++] = *sp + 1;
    stack[*sp + 1] = base(ir.l, *bp, stack);
    stack[*sp + 2] = *bp;
    stack[*sp + 3] = *pc;
    *bp = *sp + 1;
    *pc = ir.m;
    break;
    
  case INC:
    *sp = *sp + ir.m;
    break;
    
  case JMP:
    *pc = ir.m;
    break;
    
  case JPC:
    if(!stack[(*sp)--])
      *pc = ir.m;
    break;
    
  case SOI:
    printf("%d\n", stack[(*sp)--]);
    break;
    
  case SIO:
    scanf("%d", &stack[++(*sp)]);
    break;
  }
  
  return 1; //A non-zero return value keeps the machine running
}

void printFile(instruction code[], int numInstructions)
{
  int i;

  //Printing the header
  printf("\nLine   OP   L   M\n");
  
  //Printing the lines
  for(i=0; i < numInstructions; i++){
    printf("%4d   %s  %1d   %d\n", i, OPCODES[code[i].op],
           code[i].l, code[i].m);
  }

  printf("\n");
}

int readFile(int argc, char* argv[], int* verbose, instruction code[])
{
  FILE* fin;
  int pcounter = 0;

  //Checking command-line arguments
  if(argc < 2 || argc > 3){
    printf(USAGE);
    return -1;
  }

  if(argc == 3){
    if(strcmp(argv[1], "-q") == 0){
      *verbose = 0;
      fin = fopen(argv[2], "r");
    }else{
      printf(USAGE);
      return -1;
    }
  }else{
    fin = fopen(argv[1], "r");
  }

  if(!fin){
    printf("Error opening file\n");
    return -1;
  }

  //Reading the file
  while(!feof(fin)){
    fscanf(fin, "%d ", &code[pcounter].op);
    fscanf(fin, "%d ", &code[pcounter].l);
    fscanf(fin, "%d ", &code[pcounter].m);
    pcounter++;

    //Checking for code overflow
    if(pcounter >= MAX_CODE_LENGTH){
      printf("CODE OVERFLOW\n");
      return -1;
    }
  }

  fclose(fin);

  return pcounter;
}
