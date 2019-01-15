//Richard Lee

#include "Subproof.h"
#include <iostream>

using namespace std;

Subproof::Subproof(Term* premise, Term* conclusion){

  ProofLine* premiseL=new ProofLine(premise,1,0);
  premises.insert(premiseL);

  ProofLine* conclusionL= new ProofLine(conclusion,0,1);
  conclusions.insert(conclusionL);

  workingTruths[premise->print()]=premiseL;
  goals[conclusion->print()]=conclusionL;

}

const string Subproof::print()const{

  if(DEBUG_LEVEL>=11)
    cout<<"PrintingSubproof"<<endl;

  string s;
  s=s+(*(premises.begin()))->term->print()+"\n---\n"+(*(conclusions.begin()))->term->print();


  if(DEBUG_LEVEL>=11)
    cout<<"Subproof printed as:"<<s<<endl;

  return s;
}



bool Subproof::subproofStep(){
  bool progress=false;

  if(DEBUG_LEVEL>=9)
    cout<<"Regenerating true statments"<<endl;
  newtruestatements.clear();  

  truestatements=parent->truestatements;


  if(DEBUG_LEVEL>=9)
    cout<<"Finished copying parent."<<endl;

  for(map<string,ProofLine*>::iterator iter=workingTruths.begin();iter!=workingTruths.end();iter++){
    truestatements[iter->first]=iter->second;
  }

  if(DEBUG_LEVEL>=9)
    cout<<"Done regenerating true statment. doing beat"<<endl;

  newtruestatements.clear();
  progress=proofHeartbeat();
  
  if(DEBUG_LEVEL>=9)
    cout<<"subproof beat complete"<<endl;

  for(map<string,ProofLine*>::iterator iter=newtruestatements.begin();iter!=newtruestatements.end();iter++){
    if(parent->truestatements.find(iter->first)==parent->truestatements.end()){//don't want to copy known statements
      workingTruths[iter->first]=iter->second;
      iter->second->mySubproofs.insert(this);
    }
  }

  for(map<string,ProofLine*>::iterator iter=newtruestatements.begin();iter!=newtruestatements.end();iter++){
    truestatements[iter->first]=iter->second;
  }
  newtruestatements.clear();

  checkForCompletedGoals();
  if(DEBUG_LEVEL>=9)
    cout<<"Finished checking for completed goals in subproof. Checking for completion"<<endl;

  for(map<string,ProofLine*>::iterator iter=newtruestatements.begin();iter!=newtruestatements.end();iter++){
    if(parent->truestatements.find(iter->first)==parent->truestatements.end()){
      workingTruths[iter->first]=iter->second;
      iter->second->mySubproofs.insert(this);
    }
  }
  newtruestatements.clear();


  if(areWeDone()){
    if(DEBUG_LEVEL>=9)
      cout<<"Subproof\n"<<this->print()<<"\nis proven."<<endl;



    progress=true;
    me->isTrue=true;
    parent->newtruestatements[print()]=me;


    parent->cascadeGoals(me);
    if(DEBUG_LEVEL>=10)
      cout<<"Subproof\n"<<this->print()<<"\ninserting dependencies."<<endl;

    for(map<string,ProofLine*>::iterator iter=workingTruths.begin();iter!=workingTruths.end();iter++){
      for(set<ProofLine*>::iterator iter2=iter->second->provenBy.begin();iter2!=iter->second->provenBy.end();iter2++){
	if(workingTruths.find((*iter2)->term->print())==workingTruths.end()){
	  me->provenBy.insert(*iter2);
	}
      }
    }
    if(DEBUG_LEVEL>=10)
      cout<<"Subproof\n"<<this->print()<<"\ndone inserting dependencies."<<endl;

  }

  if(DEBUG_LEVEL>=9)
    cout<<"Finished subproof step."<<endl;

  return progress;
}
