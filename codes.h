#ifndef CODES_H
#define CODES_H

//Machine constants
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500

//Scanner constants
#define MAX_IDENT_LENGTH 11
#define MAX_NUMBER_LENGTH 5

//Parser constants
#define BASE_OFFSET 3

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
  NUM_FOLLOW_EQ = 2,
  EQ_FOLLOW_ID = 3,
  ID_FOLLOW_CONST_VAR_PROC = 4,
  SEMICOL_COMMA_MISS = 5,
  WRONG_SYM_AFTER_PROC = 6,
  STATEMENT_EXPEC = 7,
  WRONG_SYM_AFTER_STATE = 8,
  PERIOD_EXPEC = 9,
  SEMICOL_BW_STATE_MISS = 10,
  UNDEC_ID = 11,
  CANNOT_ASSIGN_TO_CONST_OR_PROC = 12,
  ASSIGN_EXPEC = 13,
  ID_FOLLOW_SYAW = 14,
  CONST_OR_VAR_CALL_USELESS = 15,
  TSAKRR_EXPEC = 16,
  SEMICOL_OR_RBRACK_EXPEC = 17,
  SI_EXPEC = 18,
  WRONG_SYM_FOLLOWING_STATE = 19,
  REL_OP_EXPEC = 20,
  EXP_CANNOT_CONTAIN_PROC_IDENT = 21,
  RPAREN_MISS = 22,
  PREC_FACTOR_CANNOT_BEGIN_SYM = 23,
  SYMBOL_CANNOT_BEGIN_THIS_EXP = 24,
  NUMBER_TOO_LARGE = 25,
  RBRACK_EXPEC_AT_END = 26,
  CANNOT_STORE_IN_CONST_OR_PROC = 27
} errorCode;

#endif
