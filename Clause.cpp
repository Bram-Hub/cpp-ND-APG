//Richard Lee

#include "Clause.h"

#include <iostream>

using namespace std;

Clause::Clause(int aNumTerms,string aOperator, Term** terms):numTerms(aNumTerms),op(aOperator){

  switch(numTerms){
  case 2: B=terms[1];//break intentionally omitted here
  case 1: A=terms[0];//and here
  case 0: break;
  default: cerr<<"ERROR: Only 0-2ary operators supported, not "<<numTerms<<"ary."<<endl;break;
  }
}

const string Clause::nicePrint()const{
  switch(numTerms){
  case 0: return op+' ';;break;
  case 1: return op+'('+A->nicePrint()+')';break;
  case 2: return "("+A->nicePrint()+' '+op+' '+B->nicePrint()+")";break;
  default: cerr<<"ERROR in Clause nicePrint: numTerms="<<numTerms<<endl;return "Error: bad term";break;
  }
}

const string Clause:: print()const{
  switch(numTerms){
  case 0:return op+' ';break;
  case 1:return op+' '+A->print();break;
  case 2:return op+' '+A->print()+' '+B->print();break;
  default: cerr<<"ERROR in Clause print: numTerms="<<numTerms<<endl;return "Error: bad term";break;
  }
}

int Clause::getOpIndex(const string& op){

  if(op=="&")
    return 1;
  if(op=="|")
    return 2;
  if(op=="!")
    return 3;
  if(op=="~")
    return 4;
  if(op=="->")
    return 5;
  if(op=="<->")
    return 6;

  cerr<<"Error: unknown operator "<<op<<endl;
  return -1;
}

string Clause::getOpString(const int& opCode){

  switch(opCode){
  case 1: return "&";
  case 2: return "|";
  case 3: return "!";
  case 4: return "~";
  case 5: return "->";
  case 6: return "<->";
  default: cerr<<"Error: unknown opCode "<<opCode<<endl;return "ERROR";
  }

  return "ERROR2";
}
