//Richard Lee


#include "Proof.h"
#include "Subproof.h"

#include <iostream>
#include <list>
#include <math.h>
#include <fstream>

using namespace std;

int Proof::DEBUG_LEVEL=10;


Proof::Proof(set<Term*> aPremises, set<Term*> aConclusions):depth(0){

  for(set<Term*>::iterator iter=aPremises.begin();iter!=aPremises.end();iter++)
    premises.insert(new ProofLine(*iter,1,0));

  for(set<Term*>::iterator iter=aConclusions.begin();iter!=aConclusions.end();iter++)
    conclusions.insert(new ProofLine(*iter,0,1));


}

void Proof::numberLines(int &startnum){
  for(unsigned int i=0;i<proof.size();i++){
    if (proof[i]->hasSubproof()){
      Subproof* temp = (Subproof*)(proof[i]->term);
      temp->numberLines(startnum);
      continue;
    }

    proof[i]->lineNumber=startnum;
    startnum++;
  }
}

void Proof::numberLines(){
  int i=1;
  numberLines(i);

}


//recursivly sets neededInProof to true for all statments needed to prove here
void Proof::backwardsNecSpider(bool setValue, ProofLine* here){
  for(set<ProofLine*>::iterator iter=here->provenBy.begin();iter!=here->provenBy.end();iter++){
    if((*iter)->neededInProof)
      continue;
    
    (*iter)->neededInProof=setValue;
    backwardsNecSpider(setValue, *iter);
  }
}


//recursivly creates storage
//an ordering of the needed proof lines
//in which no line is ahead of a line which requires it
void Proof::backwardsPutSpider(list<ProofLine*>& storage, ProofLine* here){
  for(set<ProofLine*>::iterator iter=here->provenBy.begin();iter!=here->provenBy.end();iter++){

    for(map<ProofLine*,string>::iterator iter2=(*iter)->proves.begin();iter2!=(*iter)->proves.end();iter2++)
      if(iter2->first->neededInProof && !(iter2->first->inProof))//if there is an earlier need for *iter
	continue;

    if((*iter)->isPremise)
      continue;

    storage.push_front(*iter);
    (*iter)->inProof=1;

    if((*iter)->hasSubproof()){
      ((Subproof*)((*iter)->term))->formProof();
    }

    backwardsPutSpider(storage, *iter);
  }
}

void Proof::formProof(){

  for(set<ProofLine*>::iterator iter=conclusions.begin();iter!=conclusions.end();iter++){
    if((*iter)->neededInProof)
      continue;
    (*iter)->neededInProof=1;
    backwardsNecSpider(1, *iter);
  }
  
  list<ProofLine*> storage;

  for(set<ProofLine*>::iterator iter=conclusions.begin();iter!=conclusions.end();iter++){

    for(map<ProofLine*, string>::iterator iter2=(*iter)->proves.begin();iter2!=(*iter)->proves.end();iter2++)
      if(iter2->first->neededInProof && !(iter2->first->inProof))//if there is an earlier need for *iter
	continue;

    if((*iter)->isPremise)
      continue;

    storage.push_front(*iter);
    (*iter)->inProof=1;
    backwardsPutSpider(storage, *iter);
  }

  proof=vector<ProofLine*>();

  for(set<ProofLine*>::iterator iter=premises.begin();iter!=premises.end();iter++){
    proof.push_back(*iter);
  }
  for(list<ProofLine*>::iterator iter=storage.begin();iter!=storage.end();iter++){
    proof.push_back(*iter);
  }

}


string Proof::proofToString()const{
    int numLines=proof[proof.size()-1]->lineRange().second;
    unsigned int totalSpaces=((int)floor(log10(numLines)))+1;
    return proofToString(totalSpaces);
}


string Proof::proofToString(unsigned int totalSpaces)const{

  if(DEBUG_LEVEL>=2){
    cout<<"Printing proof, depth:"<<depth<<endl;
  }

    string output;

   if(DEBUG_LEVEL>=2){
     int numLines=proof[proof.size()-1]->lineRange().second;
     cout<<"Proof has "<<numLines<<" lines, and tot spa is "<<totalSpaces<<endl;
   }


    for(unsigned int i=0;i<proof.size();i++){

      //beware short-circuit ||
      if((!(proof[i]->isPremise))&&(i==0 || proof[i-1]->isPremise)){
	   string blank=string(totalSpaces,' ');
           if(DEBUG_LEVEL>=6){
             cout<<"Printing breaker"<<endl<<"BR"<<blank<<"EAK"<<endl;
           }

	   output=output+blank+string(depth+1,'|')+"------\n";
      }
      
      output=output+(proof[i])->proofPrint(totalSpaces,depth)+((proof[i]->term->isSubproof())?"":"\n");
    }
    if(DEBUG_LEVEL>=2){
      cout<<"Done printing proof\n"<<this->print()<<endl;
    }

    return output;
}

void Proof::writeProof(const string& filename)const{
  ofstream out(filename.c_str());

  out<<proofToString()<<endl;
}

void Proof::printProof()const{
  cout<<endl<<proofToString()<<endl;
}


void Proof::prove(){

  if(DEBUG_LEVEL>=1)
    cout<<"Starting Proof"<<endl;

  initializeProof();

  bool first_loop_flag=true;

  while(1)
  {
    if(DEBUG_LEVEL>=2)
      cout<<"Starting a loop Truths: "<<truestatements.size()<<" Goals: "<<goals.size()<<endl;

    newtruestatements.clear();
    bool progress=proofHeartbeat();
    
    for(map<string,ProofLine*>::iterator iter=newtruestatements.begin();iter!=newtruestatements.end();iter++){
      truestatements[iter->first]=iter->second;
    }
    newtruestatements.clear();

    if(!(progress || first_loop_flag)){
      if(first_loop_flag)
	first_loop_flag=false;
      else{
	cerr<<"I can't prove this!"<<endl;
	return;
      }
    }
    first_loop_flag=false;

    if(DEBUG_LEVEL>=3)
      cout<<"Checking for completed goals: "<<progress<<endl;

    checkForCompletedGoals();

    for(map<string,ProofLine*>::iterator iter=newtruestatements.begin();iter!=newtruestatements.end();iter++){
      truestatements[iter->first]=iter->second;
    }
    newtruestatements.clear();

    if(areWeDone())
      break;
  }

  if(DEBUG_LEVEL>=1)
    cout<<"Proof Generated.  Formating proof."<<endl;

  formProof();

  if(DEBUG_LEVEL>=1)
    cout<<"Proof formated.  Numbering Lines."<<endl;

  numberLines();

  if(DEBUG_LEVEL>=1)
    cout<<"Proof finished"<<endl;

}



void Proof::initializeProof(){
  //use starting conclusions as goals
  for(set<ProofLine*>::iterator iter=conclusions.begin();iter!=conclusions.end();iter++){
    goals[(*iter)->term->print()]=*iter;
  }

  //use premises as starting true statements
  for(set<ProofLine*>::iterator iter=premises.begin();iter!=premises.end();iter++){
    (*iter)->isTrue=1;
    truestatements[(*iter)->term->print()]=*iter;
  }

}

bool Proof::doMediumStuff(){

  bool progress= false;
  //do medium deductions
  for(map<string,ProofLine*>::iterator iter=truestatements.begin();iter!=truestatements.end();iter++){
    if(DEBUG_LEVEL>=5)
      cout<<"Starting medium deductions for "<<iter->first<<endl;

    set<pair<pair<string,Term*>,set<ProofLine*> > > ded= iter->second->generateMediumDeductions(truestatements,goals);
    for(set<pair<pair<string,Term*>,set<ProofLine* > > >::iterator iter2=ded.begin();iter2!=ded.end();iter2++){

      if(DEBUG_LEVEL>=6)
        cout<<"Deduced "<<(*iter2).first.second->print()<<" from "<<iter->first<<" by "<<(*iter2).first.first<<endl;

      map<string,ProofLine*>::iterator iter3=truestatements.find((*iter2).first.second->print());
      if(iter3==truestatements.end()){//if we haven't already proven this
	ProofLine* p=new ProofLine((*iter2).first.second);
	progress=true;
	//for each statment needed to prove it
	for(set<ProofLine*>::iterator iter4=(*iter2).second.begin();iter4!=(*iter2).second.end();iter4++){
	  (*iter4)->proves[p]=(*iter2).first.first;
	}
	p->provenBy=(*iter2).second;
	p->rulename=(*iter2).first.first;
	newtruestatements[(*iter2).first.second->print()]=p;



      }
    }
  }

  return progress;

}

bool Proof::doHardStuff(){
  if(DEBUG_LEVEL>=5)
    cout<<"Starting hard stuff"<<endl;

  bool progress=false;

  //do hard subproof generation/expansion of goals
  for(map<string,ProofLine*>::iterator iter=goals.begin();iter!=goals.end();iter++){
    if(DEBUG_LEVEL>=6)
      cout<<"Starting hard subproof generation expansions for "<<iter->first<<endl;

    map<ProofLine*,set<ProofLine*> >::iterator iter2=subproofs.find(iter->second);

    if(iter2==subproofs.end()){
      if(DEBUG_LEVEL>=7)
	cout<<"Attempting to create subproofs for "<<iter->first<<endl;
      pair<string, set<Subproof*> > aSubproofs=iter->second->generateHardSubproofs();
      if(DEBUG_LEVEL>=7)
	cout<<"Created "<<aSubproofs.second.size()<<" subproofs for "<<iter->first<<endl;
      if(aSubproofs.second.size()>0){
	progress=true;
	set<ProofLine*> subbox;

        for(set<Subproof*>::iterator iter3=aSubproofs.second.begin();iter3!=aSubproofs.second.end();iter3++){
	  if(DEBUG_LEVEL>=8)
	    cout<<"Created subproof premise: "<<(*((*iter3)->premises.begin()))->term->print()<<" conclusion: "<<(*((*iter3)->conclusions.begin()))->term->print()<<endl;

	  (*iter3)->parent=this;
	  (*iter3)->depth=depth+1;
	  ProofLine* p=new ProofLine(*iter3);
	  (*iter3)->me=p;
	  p->proves.insert(make_pair(iter->second,aSubproofs.first));
	  subbox.insert(p);
	}

	subproofs[iter->second]=subbox;
	iter->second->sufficientProofs.insert(subbox);
      }
      if(DEBUG_LEVEL>=7)
	cout<<"Finished to creating subproofs for "<<iter->first<<endl;
    }
  }


  //increment all subproofs
  for(map<ProofLine*,set<ProofLine*> >::iterator iter= subproofs.begin();iter!=subproofs.end();iter++){
    for(set<ProofLine*>::iterator iter2=iter->second.begin();iter2!=iter->second.end();iter2++){

    bool output=false;
    if(!((*iter2)->isTrue)){
      if(DEBUG_LEVEL>=8)
	cout<<"Stepping subproof"<<endl;
      output=((Subproof*)((*iter2)->term))->subproofStep();
      if(DEBUG_LEVEL>=8)
	cout<<"Step complete"<<endl;
    }
    progress=(progress)?progress:output;
  }
 }
  if(DEBUG_LEVEL>=5)
    cout<<"Finished hard stuff"<<endl;
  return progress;
}


bool Proof::proofHeartbeat(){

  if(DEBUG_LEVEL>=3)
    cout<<"Starting a beat"<<endl;

   bool progress=doEasyStuff();

   if(DEBUG_LEVEL>=4)
     cout<<"Finished easy stuff: "<<progress<<endl;

    
   if(!progress)
     progress=doMediumStuff();{
     if(DEBUG_LEVEL>=4)
       cout<<"Finished medium stuff: "<<progress<<endl;
   }
    
   if(!progress){
     progress=doHardStuff();
     if(DEBUG_LEVEL>=4)
       cout<<"Finished hard stuff: "<<progress<<endl;
   }

   if(DEBUG_LEVEL>=4)
     cout<<"Finished a beat: "<<progress<<endl;

    return progress;
}



void Proof::pruneGoals(ProofLine* p){

  if(DEBUG_LEVEL>=8)
    cout<<"Starting pruning goals from"<<p->term->print()<<endl;

  //for each ProofLine which might have proved it
  for(set<set<ProofLine*> >::iterator iter=p->sufficientProofs.begin();iter!=p->sufficientProofs.end();iter++){
    for(set<ProofLine*>::iterator iter2=iter->begin();iter2!=iter->end();iter2++){
      //if it isn't the one which proved p
      if(p->provenBy.find(*iter2)==p->provenBy.end()){
	//remove it from the set

        if(DEBUG_LEVEL>=8)
          cout<<"Removing"<<p->term->print()<<" from "<<(*iter2)->term->print()<<endl;

        (*iter2)->proves.erase(p);

	//if there is nothing left for this goal to prove
	if((*iter2)->proves.size()==0){

	  pruneGoals(*iter2);//we have no need to prove *iter2
	  goals.erase((*iter2)->term->print());
	  delete (*iter2);
	}
      }
    }
  }


  if(DEBUG_LEVEL>=8)
    cout<<"Done pruning goals from "<<p->term->print()<<endl;

}


void Proof::cascadeGoals(ProofLine* p){

  if(DEBUG_LEVEL>=6)
    cout<<"Starting cascade from "<<p->term->print() <<endl;

  //for each statment potentially proven by p
  for(map<ProofLine*,string>::iterator iter=p->proves.begin();iter!=p->proves.end();iter++){
    if(iter->first->isTrue){

      if(DEBUG_LEVEL>=6)
        cout<<iter->first->term->print()<<" is already true."<<endl;
      continue;
    }

    bool proven=false;

    if(DEBUG_LEVEL>=6)
      cout<<"Testing "<<(iter->first->term->print())<<" if proven"<<endl;

    //for each potential proof of *iter
    for(set<set<ProofLine*> >::iterator iter2=iter->first->sufficientProofs.begin();iter2!=iter->first->sufficientProofs.end() && !proven;iter2++){
      //which contains p
      if(iter2->find(p)!=iter2->end()){
	bool alltrue=true;
	//see if each necessary statment *iter3 is now true
	for(set<ProofLine*>::iterator iter3=iter2->begin();iter3!=iter2->end() &&alltrue;iter3++){
	  if(truestatements.find((*iter3)->term->print())==truestatements.end()&&newtruestatements.find((*iter3)->term->print())==newtruestatements.end()){
	    if(DEBUG_LEVEL>=7)
	      cout<<"Needed unproven statment "<<(*iter3)->term->print()<<endl;
	    alltrue=false;
	  }
	  }
	if(alltrue){
          if(DEBUG_LEVEL>=7)
            cout<<"Is proven "<<(iter->first->term->print())<<endl;

	  iter->first->provenBy=*iter2;
	  iter->first->rulename=(*(iter2->begin()))->proves[(iter->first)];
	  proven=true;
	  newtruestatements[iter->first->term->print()]=iter->first;

	  goals.erase(iter->first->term->print());
          //could add detection in lower part of function to remove other fufilled goals but above catches it.


	  iter->first->isTrue=true;
	  pruneGoals(iter->first);
	  cascadeGoals(iter->first);
          if(DEBUG_LEVEL>=7)
            cout<<"Succeeded proving "<<(iter->first->term->print())<<endl;
	}
      }
    }
    if(DEBUG_LEVEL>=6)
      cout<<"Done proving "<<iter->first->term->print()<<endl;
    if(DEBUG_LEVEL>=6)
      cout<<"Done proving "<<iter->first->term->print()<<" using "<<p->term->print()<<endl;
  }

  if(DEBUG_LEVEL>=7)
    cout<<"Finished cascade from "<<p->term->print() <<endl;

}

bool Proof::checkForCompletedGoals(){
  bool progress=false;

  for(map<string,ProofLine*>::iterator iter=truestatements.begin();iter!=truestatements.end();iter++){
    if(DEBUG_LEVEL>=4)
      cout<<"Checking if "<< iter->first <<" is a goal."<<endl;

    string t=iter->first;
    map<string,ProofLine*>::iterator iterG=goals.find(t);
    if(iterG!=goals.end()){//if we have proven a goal
      if(DEBUG_LEVEL>=5)
        cout<<"Confirmed "<<iter->first <<" as a goal."<<endl;

      progress=true;
      ProofLine* old=iter->second;
      ProofLine* next=iterG->second;
      next->isTrue=true;
      goals.erase(t);
      newtruestatements[t]=next;
      next->usurp(old);

      if(DEBUG_LEVEL>=5)
        cout<<"Pruning goals from "<<iter->first <<endl;

      pruneGoals(next);
      if(DEBUG_LEVEL>=5)
        cout<<"Cascading goals from "<<iter->first <<endl;
      cascadeGoals(next);


    }
  }

  return progress;
}


void Proof::updateGoals(ProofLine*p){

  for(set<set<ProofLine*> >::iterator iter2=p->sufficientProofs.begin();iter2!=p->sufficientProofs.end();iter2++){
    for(set<ProofLine*>::iterator iter3=iter2->begin();iter3!=iter2->end();iter3++){

      string t=(*iter3)->term->print();

      map<string,ProofLine*>::iterator point=goals.find(t);
      if(point==goals.end()){
	goals.insert(make_pair(t,*iter3));
      }
      else{
	(*iter3)->usurp(point->second);
	point->second=(*iter3);
      }
    }
  }
}

bool Proof::doEasyStuff(){

  if(DEBUG_LEVEL>=4)
    cout<<"Starting easy stuff"<<endl;

  bool progress=false;


  //do easy deductions
  for(map<string,ProofLine*>::iterator iter=truestatements.begin();iter!=truestatements.end();iter++){
    if(DEBUG_LEVEL>=5)
      cout<<"Starting easy deductions for "<<iter->first<<endl;

    set<pair<string,Term*> > ded= iter->second->generateEasyDeductions();
    for(set<pair<string,Term*> >::iterator iter2=ded.begin();iter2!=ded.end();iter2++){

      if(DEBUG_LEVEL>=6)
        cout<<"Deduced "<<iter2->second->print()<<" from "<<iter->first<<" by "<<iter2->first<<endl;

      map<string,ProofLine*>::iterator iter3=truestatements.find(iter2->second->print());
      if(iter3==truestatements.end()){
	ProofLine* p=new ProofLine(iter2->second);
	progress=true;
        iter->second->proves[p]=iter2->first;
	p->provenBy.insert(iter->second);
	p->rulename=iter2->first;
	newtruestatements[iter2->second->print()]=p;
      }
    }
  }

  //do easy expansions of goals
  for(map<string,ProofLine*>::iterator iter=goals.begin();iter!=goals.end();iter++){
    if(DEBUG_LEVEL>=5)
      cout<<"Starting easy goal expansions for "<<iter->first<<endl;

    if((iter->second)->generateEasyProofs()){
      if((iter->second)->sufficientProofs.size()>0){
	progress=true;
        if(DEBUG_LEVEL>=5)
          cout<<"Updating goals: "<<iter->first<<endl;

	updateGoals(iter->second);
      }
    }
  }
  return progress;
}


bool Proof::areWeDone()const{


  if(DEBUG_LEVEL>=3)
    cout<<"Checking for completion"<<endl;

  for(set<ProofLine*>::iterator iter=conclusions.begin();iter!=conclusions.end();iter++){

    if(truestatements.find((*iter)->term->print())==truestatements.end()){
	if(DEBUG_LEVEL>=4)
	  cout<<"Unproven conclusions "<<(*iter)->term->print()<<" remain"<<endl;

        return false;

    }

  }

  if(DEBUG_LEVEL>=4){
    cout<<"Proof is complete"<<endl;
  }
  return true;
}


const string Proof::print()const{


  if(DEBUG_LEVEL>=11)
    cout<<"Printing Proof"<<endl;

  string s;

  for(set<ProofLine*>::const_iterator iter=premises.begin();iter!=premises.end();iter++){
    s=s+(*iter)->term->print()+"\n";
  }

  s=s+"---\n";
  
  for(set<ProofLine*>::const_iterator iter=conclusions.begin();iter!=conclusions.end();iter++){
    s=s+(*iter)->term->print()+"\n";
  }

  if(DEBUG_LEVEL>=11)
    cout<<"Proof printed as:"<<s<<endl;

  return s;


}
