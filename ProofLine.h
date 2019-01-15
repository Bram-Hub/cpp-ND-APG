//Richard Lee

#ifndef __PROOF_LINE_H__
#define __PROOF_LINE_H__


#include "Term.h"
#include <vector>
#include <set>
#include <map>

//Forward definition of subproof needed for type declarations
class Subproof;


//A wraper class for a Term object giving structure
//for logical implications and proof formating
class ProofLine{
public:

  Term* term;


  bool inProof;
  bool neededInProof;

  bool isPremise;
  bool isConclusion;

  bool easyFlag;
  bool mediumFlag;
  bool hardFlag;

  bool isTrue;

  int lineNumber;

  string rulename;//rule by which provenBy proves this
  map<ProofLine*,string> proves;
  set<ProofLine*> provenBy;

  set<set<ProofLine*> > sufficientProofs;//disjunctive normal form
  set<Subproof*> mySubproofs;//subproofs this ProofLine is in 


  //Causes this to assume all the mapping responsibilites of other
  //Should have this->term==other->term
  void usurp(ProofLine* other);

  bool operator<(const ProofLine* other)const{return term<(other->term);}
 

  ProofLine(Term* aTerm, bool aIsPremise=0, bool aIsConclusion=0):term(aTerm),inProof(0), neededInProof(0), isPremise(aIsPremise),isConclusion(aIsConclusion), easyFlag(0),mediumFlag(0),hardFlag(0),isTrue(0){}

  bool hasSubproof()const{return term->isSubproof();}

  //Returns the lines to which this ProofLine correlates
  const pair<int,int> lineRange()const;
  //Provides a nice formating of the lines
  const string printMyLines()const;
  //Provides a nice formated string of the necessary proof lines and the induction type
  const string references()const;
  //Provides a nice formated string of this ProofLine
  const string proofPrint(int numSpaces, int depth=0)const;

  //Generates a set of subproofs which could prove this proof
  pair<string, set<Subproof*> > generateHardSubproofs();

  //generates simple statements which could prove this
  bool generateEasyProofs();
  //generates simple statements which this proves
  set<pair<string,Term*> > generateEasyDeductions()const;

  //generates statments which this and other statements together prove
  //output format= set((opname,proven_statment),set(necessary statments))
  set<pair<pair<string,Term*>,set<ProofLine*> > > generateMediumDeductions(map<string,ProofLine*>& truestatements,map<string,ProofLine*>& goals);

};




#endif
