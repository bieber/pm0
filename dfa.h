#ifndef DFA_H
#define DFA_H

#include <stdio.h>
#include <stdlib.h>

//The set of possible values the machine can return
typedef struct{
  int retString;  //True if the machine returns a string
  token numeric;
  char* string;
} MachineValue;

//Describes a DFA
typedef struct DFA{

  //Defines the DFA itself
  int state;  //The machine's current state
  int (*transition)(struct DFA*, char);  //Transition function pointer
  
  //Flags
  int halt;  //Machine halts if set to 1
  int accept;  //Machine is in accept state if set to 1
  int rewind;  //Input file pointer will be stepped back 1 char if this is set
  
  //Return value
  MachineValue retVal;  //The machine's return value
  
} DFA;

//Runs a DFA and returns its output
void runDFA(DFA* machine, FILE* input);

//Initializes an empty DFA
void initDFA(DFA* machine);

//Rejects a string from a DFA
void rejectDFA(DFA* machine);

#endif
