//Richard Lee

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>

#include <ctype.h>

#include "Term.h"
#include "Literal.h"
#include "Clause.h"

#include "ProofLine.h"
#include "Proof.h"
#include "Subproof.h"
#include "InputParser.h"
using namespace std;



int main(int argv, char** argc){

  bool defaultinputflag=true;
  bool defaultdebugflag=true;
  bool defaultoutputflag=true;
  string inputfile;
  string outputfile;


  for(unsigned int i=0;i<argv;i++){
    if(string(argc[i])=="-d"){
      defaultdebugflag=false;
      continue;
    }
    if(string(argc[i])=="-i"){
      i++;
      if(i<argv){
	inputfile=argc[i];
	defaultinputflag=false;
      }
      continue;
    }
    if(string(argc[i])=="-o"){
      i++;
      if(i<argv){
	outputfile=argc[i];
	defaultoutputflag=false;
      }
      continue;
    }
  }

  if(defaultdebugflag){
    InputParser::DEBUG_LEVEL=0;
    Proof::DEBUG_LEVEL=0;
  }
  else{
    InputParser::DEBUG_LEVEL=10;
    Proof::DEBUG_LEVEL=10;
  }


  InputParser input(defaultinputflag?"proof.txt":inputfile);

  pair<set<Term*>,set<Term*> > statements=input.parse();

  if(statements.first.begin()!=statements.first.end()  && (*(statements.first.begin()))==NULL){
    cout<<"Error reading input. Exiting."<<endl;
    return -1;
  }

  if(statements.second.size()==0){
    cout<<"No statements to prove. Exiting"<<endl;
  }

  Proof p(statements.first,statements.second);

  p.prove();

  if(defaultoutputflag)
    p.printProof();
  else{
    p.writeProof(outputfile);
  }

  vector<ProofLine*> lines=p.proof;

}
