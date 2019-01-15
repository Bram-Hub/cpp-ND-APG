//Richard Lee

#ifndef __CLAUSE_H__
#define __CLAUSE_H__

#include <string>
#include "Term.h"

using namespace std;


//container class for a pair of Terms being acted upon
//by a logical operator


//Also containes the mapping between operator symbols and internal opCodes
class Clause: public Term{

 public:

   int numTerms;
   string op;
   Term* A;
   Term* B;

  Clause(int aNumTerms,string aOperator, Term** terms);
  bool isClause()const{return true;}

  //Outputs consistently with the reader input.
  const string nicePrint()const;

  //Outputs in prefix notation with space delineators
  //Used for most internal recording
  const string print()const;

  //If nonstandard operator symbols are to be used, these need to be modified.
  static int getOpIndex(const string& op);
  static string getOpString(const int& opCode);
};







#endif
