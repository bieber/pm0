#include <stdio.h>
#include <stdlib.h>

#include "dfa.h"

void transition(DFA* this, char input);

int main(int argc, char* argv[]){
  
  DFA machine;
  initDFA(&machine);
  machine.transition = &transition;

  return 0;
}

void transition(DFA* this, char input){
  
  switch(this->state){
  case 0:  //Start state
    if(input == 'a'){
      this->state = 1;
    }else{
      rejectDFA(this);
    }
    break;

  case 1:  //Recognized an 'a'
    if(input == 'b'){
      this->state = 2;
      this->accept = 1;
    }else{
      rejectDFA(this);
    }
    break;

  case 2: //Recognized a 'b'
    if(input == 'a'){
      this->state = 1;
      this->accept = 0;
    }else{
      rejectDFA(this);
    }
      
    break;
  
  }

}
