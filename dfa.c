#include "dfa.h"

void runDFA(DFA* machine, FILE* input){
  
  char inChar;

  //Calling the machine's transition function repeatedly
  inChar = fgetc(input);
  while(!(machine->halt || feof(input))){
    machine->state = (*(machine->transition))(machine, inChar);
    inChar = fgetc(input);
  }

  //Moves the file pointer back one character if necessary
  if(machine->rewind)
    fseek(input, -1 * sizeof(char), SEEK_CUR);
}

//Initializing an empty machine
void initDFA(DFA* machine){

  machine->state = 0;

  machine->halt = 0;
  machine->accept = 0;
  machine->rewind = 0;

  machine->retVal.retString = 0;

}

//Rejecting a string
void rejectDFA(DFA* machine){
  machine->halt = 1;
  machine->accept = 0;
}
