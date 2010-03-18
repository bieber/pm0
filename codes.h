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
  syawsym,
  constsym, 
  intsym,
  procsym,
  wrrpasym,
  misym,
  txokefyawsym
} token;

// Declaration of Error Codes
typedef enum {
  EQ_NOT_BECOMES = 1,
  NUM_FOLLOW_EQ,
  EQ_FOLLOW_ID,
  ID_FOLLOW_CONST,
  ID_FOLLOW_VAR,
  ID_FOLLOW_PROC,
  SEMICOL_COMMA_MISS,
  WRONG_SYM_AFTER_PROC,
  STATEMENT_EXPEC,
  WRONG_SYM_AFTER_STATE,
  PERIOD_EXPEC,
  SEMICOL_BW_STATE_MISS,
  UNDEC_ID,
  CANNOT_ASSIGN_TO_CONST_OR_PROC,
  ASSIGN_EXPEC,
  ID_FOLLOW_SYAW,
  CONST_OR_VAR_CALL_USELESS,
  TSAKRR_EXPEC,
  SEMICOL_OR_RBRACK_EXPEC,
  SI_EXPEC,
  WRONG_SYM_FOLLOWING_STATE,
  REL_OP_EXPEC,
  EXP_CANNOT_CONTAIN_PROC_IDENT,
  RPAREN_MISS,
  SYMBOL_CANNOT_PRECEDE_THIS_EXP,
  NUMBER_TOO_LARGE
} errorCode;

#endif
