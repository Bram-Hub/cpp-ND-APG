//Richard Lee

#include "Subproof.h"
#include "Clause.h"
#include <iostream>
#include <math.h>
#include <cstdio>
#include <string>


#define SHORT_BUFFER_LENGTH 1000
#define LONG_BUFFER_LENGTH 4000

using namespace std;

const pair<int,int> ProofLine::lineRange()const{

  if(hasSubproof()){
    vector<ProofLine*>* lines=&(((Subproof*)term)->proof);

    int min=(*lines)[0]->lineRange().first;
    int max=(*lines)[lines->size()-1]->lineRange().second;

    return make_pair(min,max);
  }
  return make_pair(lineNumber,lineNumber);
}

const string ProofLine::printMyLines()const{
  char buffer [100];

  if(hasSubproof()){
    pair<int,int> that=lineRange();
    sprintf(buffer,"%i-%i",that.first,that.second);
    return(string(buffer));
  }
  else{
    sprintf(buffer,"%i",lineNumber);
    return (string(buffer));
  }
}

const string ProofLine::references()const{
  char buffer[SHORT_BUFFER_LENGTH];
  string output;
  output=rulename+" ";

  //may prefer toString if a newer complier is availiable
  for(set<ProofLine*>::const_iterator iter=provenBy.begin();iter!=provenBy.end();iter++){
    sprintf(buffer," %s",(*iter)->printMyLines().c_str());
    output=output.append(buffer);
  }
  return output;
}


//depth=the number of nested subproofs. For something in the main proof, depth=0
const string ProofLine::proofPrint(int numSpaces, int depth)const{

  char buffer[LONG_BUFFER_LENGTH];
  if(term->isSubproof()){
    ((Subproof*)term)->depth=depth+1;
    return ((Subproof*)term)->proofToString(numSpaces);
  }
  else{
    sprintf(buffer,"%i%s%s%s  %s",lineNumber,string(" ",numSpaces-floor(log10(lineNumber))-1).c_str(),string(depth+1,'|').c_str(),term->nicePrint().c_str(),references().c_str());

    return string(buffer);
  }
}


bool ProofLine::generateEasyProofs(){
  if(easyFlag)
    return 0;

  easyFlag=1;

  if(term->isSubproof())
    return 0;

  set<ProofLine*> aSufficientProof;

  if(term->isLiteral())
    return 0;

  if(term->isClause()){
    Clause* c=(Clause*) term;
    ProofLine* a;
    ProofLine* b;

    int opCode=Clause::getOpIndex(c->op);
    aSufficientProof.clear();
    switch(opCode){
    case 1://& intro case
      a=new ProofLine(c->A);
      a->proves[this]="& intro";
      aSufficientProof.insert(a);
      b=new ProofLine(c->B);
      b->proves[this]="& intro";
      aSufficientProof.insert(b);
      sufficientProofs.insert(aSufficientProof);
      break;
    case 2://| intro case
      a=new ProofLine(c->A);
      a->proves[this]="| intro";
      aSufficientProof.insert(a);
      sufficientProofs.insert(aSufficientProof);
      aSufficientProof.clear();
      b=new ProofLine(c->B);
      b->proves[this]="| intro";
      aSufficientProof.insert(b);
      sufficientProofs.insert(aSufficientProof);
      break;
    case -1:
      cerr<<"Unknown operator in "<<term->print()<<endl;
      break;
    }


    return 1;
  }

  cerr<<"Error: unknown Term!!"<<endl;
  return 0;
}


set<pair<string, Term*> > ProofLine::generateEasyDeductions()const{

  set<pair<string,Term*> > deductions;
  if(term->isLiteral()){
    return deductions;
  }

  if(!term->isClause())
    return deductions;

  Clause* c=(Clause*)term;
  int opNum=Clause::getOpIndex(c->op);

  switch(opNum){
  case 1://& elim
    deductions.insert(make_pair("& elim",c->A));
    deductions.insert(make_pair("& elim",c->B));
    break;
  case 3://! elim
    if(c->A->isClause()&& Clause::getOpIndex(((Clause*)(c->A))->op)==3)
      deductions.insert(make_pair("! elim",((Clause*)(c->A))->A));
  }

  return deductions;
}


//have a goal ProofLine take over a true statement ProofLine
void ProofLine::usurp(ProofLine* other){
  if((term->print())!=(other->term->print())){
     cerr<<"Error: Imposter "<<other->term->print()<<" attempting to usurp "<<term->print()<<endl;
     return;
   }

  isTrue=other->isTrue;

  for(map<ProofLine*,string>::iterator iter=other->proves.begin();iter!=other->proves.end();iter++)
    proves.insert(*iter);
  
  rulename=other->rulename;
  provenBy=other->provenBy;

  if(other->isPremise){
    rulename="reiter";
    provenBy.insert(other);
  }

  for(map<ProofLine*,string>::iterator iter=proves.begin();iter!=proves.end();iter++){
    if(iter->first->isTrue){
      iter->first->provenBy.erase(other);
      iter->first->provenBy.insert(this);
    }
  }


  for(set<ProofLine*>::iterator iter=provenBy.begin();iter!=provenBy.end();iter++){
    if((*iter)->isTrue){
      (*iter)->proves[this]=(*iter)->proves[other];
      (*iter)->proves.erase(other);
    }
  }
   
  //replace reference in proving statments  
  if(!isTrue){
    for(map<ProofLine*,string>::iterator iter=proves.begin();iter!=proves.end();iter++){
      for(set<set<ProofLine*> >::iterator iter2=iter->first->sufficientProofs.begin();iter2!=iter->first->sufficientProofs.end();iter2++){

	if(iter2->find(other)!=iter2->end()){

	  const set<ProofLine*>& foo=*iter2;
	  set<ProofLine*> next=foo;
	  next.erase(other);
	  next.insert(this);
	  iter->first->sufficientProofs.erase(foo);
	  iter->first->sufficientProofs.insert(next);
	}

      }
    }
  }


  //replace references in subproofs
  for(set<Subproof*>::iterator iter=other->mySubproofs.begin();iter!=other->mySubproofs.end();iter++){
    (*iter)->workingTruths[term->print()]=this;


  }


  if(!other->isPremise)
    delete other;

}


set<pair<pair<string,Term*>,set<ProofLine*> > > ProofLine::generateMediumDeductions(map<string,ProofLine*>& truestatements,map<string,ProofLine*>& goals){

  set<pair<pair<string,Term*>,set<ProofLine*> > > deductions;

  if(term->isLiteral()){
    return deductions;
  }

  if(!term->isClause())
    return deductions;

  Clause* c=(Clause*)term;
  int opNum=Clause::getOpIndex(c->op);


  map<string,ProofLine*>::iterator iter;

  switch(opNum){
  case 4://~ elim
    for(iter=goals.begin();iter!=goals.end();iter++){
      set<ProofLine*> set1;
      Term* t= iter->second->term;
      set1.insert(this);
      deductions.insert(make_pair(make_pair("~ elim",t),set1));
    }
    break;
  case 5://-> elim
    iter=truestatements.find(c->A->print());
    if (iter!=truestatements.end()){
      set<ProofLine*> set1;
      set1.insert(this);
      set1.insert(iter->second);
      Term* t=c->B;
      deductions.insert(make_pair(make_pair("-> elim",t),set1));
    }
    break;
  case 6://<-> elim
    iter=truestatements.find(c->A->print());
    if (iter!=truestatements.end()){
      set<ProofLine*> set1;
      set1.insert(this);
      set1.insert(iter->second);
      Term* t=c->B;
      deductions.insert(make_pair(make_pair("<-> elim",t),set1));
    }
    map<string,ProofLine*>::iterator iter=truestatements.find(c->B->print());
    if (iter!=truestatements.end()){
      set<ProofLine*> set1;
      set1.insert(this);
      set1.insert(iter->second);
      Term* t=c->A;
      deductions.insert(make_pair(make_pair("<-> elim",t),set1));
    }
    break;
  }


  return deductions;
}

pair<string,set<Subproof*> > ProofLine::generateHardSubproofs(){


  set<Subproof*> proofs;

  if(term->isSubproof())
    return make_pair("",proofs);

  if(!(term->isClause()))
    return make_pair("",proofs);

  Clause* c=(Clause*)term;
  int opNum=Clause::getOpIndex(c->op);

  Subproof* s;
  Subproof* s1;
  Subproof* s2;

  switch(opNum){
  case 3://! intro
    s=new Subproof(c->A,new Clause(0,Clause::getOpString(4),0));
    proofs.insert(s);
    return make_pair("! intro",proofs);
    break;
  case 5://-> intro
    s=new Subproof(c->A,c->B);
    proofs.insert(s);
    return make_pair("-> intro",proofs);
    break;
  case 6://<-> intro
    s1=new Subproof(c->A,c->B);
    s2=new Subproof(c->B,c->A);
    proofs.insert(s1);
    proofs.insert(s2);
    return make_pair("<-> intro",proofs);
    break;
  }


  return make_pair("",proofs);
}
