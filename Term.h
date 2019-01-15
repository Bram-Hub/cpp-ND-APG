//Richard Lee

#ifndef __TERM_H__
#define __TERM_H__

#include <string>

using namespace std;


//Abstract container class for self-contained logical syntactic fragments
//Implemented as literals, clauses, and subproofs.
//Future implementation of 'generic term' subclass would aid in remaining inference rules
class Term{
public:
  virtual const string print()const =0;
  virtual const string nicePrint()const =0;

  virtual bool isLiteral()const{return 0;}
  virtual bool isSubproof()const{return 0;}
  virtual bool isClause()const{return 0;}
  virtual bool isGeneric()const{return 0;}

  virtual bool operator<(const Term* other)const{return (this->print()<(other->print()));}
  virtual bool operator==(const Term* other)const {return this->print()==(other->print());}
};


#endif
