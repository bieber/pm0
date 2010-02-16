#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
    if(isalpha(input)){
      if(input == 's')
        return 2;
      else if(input == 'f')
        return 13;
      else if(input == 't')
        return 17;
      else if(input == 'm')
        return 38;
      else if(input == 'w')
        return 40;
      else
        return 1;
    }else if(isdigit(input)){
      return 46;
    }else{
      rejectDFA(this);
    }
    break;

  case 1:
    if(isalnum(input))
      return 1;
    else
      rejectDFA(this);
    break;
  
  case 2:
    if(isalnum(input)){
      if(input == 'y')
        return 4;
      else if(input == 'n')
        return 6;
      else if(input == 'i'){
        this->accept = 1;
        return 3;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 3:
    if(input == ' '){
      this->retVal.retString = 26;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 4:
    if(isalnum(input)){
      if(input == 'a')
        return 5;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 5:
    if(isalnum(input)){
      if(input == 'w'){
        this->accept = 1;
        return 1;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 6:
    if(input == ' '){
      this->retVal.retString = 27;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 7:
    if(isalnum(input)){
      if(input == 'n')
        return 8;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 8:
    if(isalnum(input)){
      if(input == 'g')
        return 9;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 9:
    if(isalnum(input)){
      if(input == 'a')
        return 8;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 10:
    if(input == '\''){
      return 11;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 11:
    if(isalnum(input)){
      if(input == 'i'){
        this->accept = 1;
        return 12;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 12:
    if(input == ' '){
      this->retVal.retString = 21;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 13:
    if(isalnum(input)){
      if(input == 'p')
        return 14;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 14:
    if(isalnum(input)){
      if(input == 'e')
        return 15;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 15:
    if(input == '\''){
      this->accept = 1;
      return 16;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 16:
    if(input == ' '){
      this->retVal.retString = 22;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 17:
    if(isalnum(input)){
      if(input == 'x')
        return 18;
      else if(input == 's')
        return 27;
      else if(input == 'e')
        return 32;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 18:
    if(isalnum(input)){
      if(input == 'o'){
        this->accept = 1;
        return 19;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 19:
    if(input == ' '){
      this->retVal.retString = 23;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      if(input == 'k')
        return 20;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 20:
    if(isalnum(input)){
      if(input == 'e')
        return 21;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 21:
    if(isalnum(input)){
      if(input == 'f')
        return 22;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 22:
    if(isalnum(input)){
      if(input == 'y')
        return 23;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 23:
    if(isalnum(input)){
      if(input == 'y')
        return 24;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 24:
    if(isalnum(input)){
      if(input == 'a')
        return 25;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 25:
    if(isalnum(input)){
      if(input == 'w'){
        this->accept = 1;
        return 26;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 26:
    if(input == ' '){
      this->retVal.retString = 33;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 27:
    if(isalnum(input)){
      if(input == 'a')
        return 28;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 28:
    if(isalnum(input)){
      if(input == 'k')
        return 29;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 29:
    if(isalnum(input)){
      if(input == 'r')
        return 30;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 30:
    if(isalnum(input)){
      if(input == 'r'){
        this->accept = 1;
        return 31;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 31:
    if(input == ' '){
      this->retVal.retString = 24;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 32:
    if(isalnum(input)){
      if(input == 'n')
        return 33;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 33:
    if(isalnum(input)){
      if(input == 'g')
        return 34;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 34:
    if(isalnum(input)){
      if(input == 'k')
        return 35;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 35:
    if(isalnum(input)){
      if(input == 'r')
        return 36;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 36:
    if(isalnum(input)){
      if(input == 'r'){
        this->accept = 1;
        return 37;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 37:
    if(input == ' '){
      this->retVal.retString = 25;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 38:
    if(isalnum(input)){
      if(input == 'i')
        return 39;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 39:
    if(input == ' '){
      this->retVal.retString = 32;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 40:
    if(isalnum(input)){
      if(input == 'r')
        return 41;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 41:
    if(isalnum(input)){
      if(input == 'r')
        return 42;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 42:
    if(isalnum(input)){
      if(input == 'r')
        return 43;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
  
  case 43:
    if(isalnum(input)){
      if(input == 'p')
        return 44;
      else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 44:
    if(isalnum(input)){
      if(input == 'a'){
        this->accept = 1;
        return 45;
      }else
        return 1;
    }else{
      rejectDFA(this);
    }
    break;
    
  case 45:
    if(input == ' '){
      this->retVal.retString = 31;
      this->accept = 1;
      this->halt = 1;
    }else if(isalnum(input)){
      return 1;
    }else{
      rejectDFA(this);
    }
    break;
  }
  
  case 46: // Handling digits, not finished yet
    if(isdigit(input)){
      if(atoi(this->retVal.retString) <= 99999){
        this->retVal.retString = this->retVal.retString * 10 + input;
      }
      else
        
    }else{
      rejectDFA(this);
    }
    break;
    
  return -1;
}


















