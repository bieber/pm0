#ifndef CODES_H
#define CODES_H

//Machine constants
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500

//Scanner constants
#define MAX_IDENT_LENGTH 11
#define MAX_NUMBER_LENGTH 5

//Opcodes
typedef enum{
  LIT = 1,
  OPR = 2,
  LOD = 3,
  STO = 4,
  CAL = 5,
  INC = 6,
  JMP = 7,
  JPC = 8,
  SOI = 9,
  SIO = 10
} opcode;

//Operator codes
typedef enum{
  RET = 0,
  NEG = 1,
  ADD = 2,
  SUB = 3,
  MUL = 4,
  DIV = 5,
  ODD = 6,
  MOD = 7,
  EQL = 8,
  NEQ = 9,
  LSS = 10,
  LEQ = 11,
  GTR = 12,
  GEQ = 13
} operator;

//Instruction definition
typedef struct instruction{
  int op;
  int l;
  int m;
} instruction;

//Declaration of Token Types
typedef enum {
  nulsym = 1,
  identsym,
  numbersym,
  plussym,
  minussym,
  multsym,
  slashsym,
  oddsym,
  eqsym,
  neqsym,
  lessym, 
  leqsym,
  gtrsym,
  geqsym,
  lparentsym,
  rparentsym, 
  commasym,
  semicolonsym,
  periodsym,
  becomessym,
  sngaisym, 
  fpesym,
  txosym,
  tsakrrsym,
  tengkrrsym,
  sisym,
  swaysym,
  constsym, 
  intsym,
  procsym,
  wrrpasym,
  misym,
  txokefyawsym
} token_type;

#endif
