//Richard Lee

#ifndef __PROOF_H__
#define __PROOF_H__


#include "Term.h"
#include "ProofLine.h"

#include <map>
#include <set>
#include <string>
#include <list>
#include <vector>

class Subproof;


class Proof{
public:


  static int DEBUG_LEVEL;

  Proof(set<Term*> aPremises, set<Term*> aConclusions);
  Proof(){}

  set<ProofLine*> premises;
  set<ProofLine*> conclusions;

  //format map<proven statments, set<ProofLines which prove the statment> >
  map<ProofLine*,set<ProofLine*> > subproofs;
  
  //the string is the printed form of the derefference ProofLine*
  map<string,ProofLine*> truestatements;
  map<string,ProofLine*> newtruestatements;
  map<string,ProofLine*> goals;

  int depth;

  vector<ProofLine*> proof;



  virtual bool isSubproof()const{return false;}
  void numberLines();
  
  virtual string proofToString()const;
  void writeProof(const string& filename)const;
  void printProof()const;

  //prints the premises and conclusions
  const string print()const;

  void prove();

  //internal proof funnctions
  void initializeProof();
  bool proofHeartbeat();
  bool doEasyStuff();
  bool doMediumStuff();
  bool doHardStuff();
  bool checkForCompletedGoals();
  void pruneGoals(ProofLine* p);
  void updateGoals(ProofLine* p);
  void cascadeGoals(ProofLine* p);
  bool areWeDone()const;
  void numberLines(int &startnum);
  void backwardsNecSpider(bool state, ProofLine* here);
  void backwardsPutSpider(list<ProofLine*>& storage, ProofLine* here);
  void formProof();
  string proofToString(unsigned int totalSpaces)const;


};

#endif
