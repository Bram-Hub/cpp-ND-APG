//Richard Lee

#ifndef __SUBPROOF_H__
#define __SUBPROOF_H__

#include "Proof.h"
#include "Term.h"


class Subproof: public Proof, public Term{
public:
  Proof* parent;
  ProofLine* me;

  Subproof(Term* premise,Term* conclusion);

  map<string,ProofLine*> workingTruths;

  virtual const string print()const;
  virtual const string nicePrint()const{return proofToString();}

  bool isLiteral()const {return false;}
  virtual bool isSubproof()const{return true;} 

  void prestep();

  bool subproofStep();

};









#endif
