#include "dfa.h"
#include "codes.h"

#include <string.h>

void runDFA(DFA* machine, FILE* input){
  
  int inChar;

  //Calling the machine's transition function repeatedly
  while(!machine->halt){
    inChar = fgetc(input);
    if(inChar == EOF)
      break;
    //Appending the character
    machine->retVal.string[strlen(machine->retVal.string)+1] = '\0';
    machine->retVal.string[strlen(machine->retVal.string)] = inChar;
    //Running the machine
    machine->state = (*(machine->transition))(machine, (char)inChar);
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
  machine->retVal.string[0]='\0';
}

//Rejecting a string
void rejectDFA(DFA* machine){
  machine->halt = 1;
  machine->accept = 0;
}
