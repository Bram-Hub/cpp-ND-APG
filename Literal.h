//Richard Lee

#ifndef __LITERAL_H__
#define __LITERAL_H__

#include "Term.h"


//Container class for a literal
class Literal: public Term{
private:
  string name;
public:

  Literal(const string& aName):name(aName){}

  bool isLiteral()const{return true;}
  virtual const string print()const{return name;}
  virtual const string nicePrint()const{return name;}

};



#endif
