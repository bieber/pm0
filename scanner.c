#include <stdio.h>
#include <stdlib.h>

#include "dfa.h"

int transition(DFA* this, char input);

int main(int argc, char* argv[]){

  FILE* fin;
  
  DFA machine;
  initDFA(&machine);
  machine.accept = 1;
  machine.transition = &transition;

  fin = fopen(argv[1], "r");
  runDFA(&machine, fin);

  fclose(fin);

  if(machine.accept)
    printf("String accepted\n");
  else
    printf("String rejected\n");

  return 0;
}

int transition(DFA* this, char input){
  
  printf("%d - %d\n", this->state, input);

  switch(this->state){
  case 0:  //Start state
    if(input == 'a'){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;

  case 1:  //Recognized an 'a'
    if(input == 'b'){
      this->accept = 1;
      return 2;
    }else{
      rejectDFA(this);
    }
    break;

  case 2: //Recognized a 'b'
    if(input == 'a'){
      this->accept = 0;
      return 1;
    }else if(input == '\n'){
      this->accept = 1;
      return 2;
    }else{
      rejectDFA(this);
    }
      
    break;
  
  }
  
  return -1;
}
