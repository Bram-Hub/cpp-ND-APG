//Richard Lee

#ifndef __INPUT_PARSER_H__
#define __INPUT_PARSER_H__

#include "Term.h"
#include "Literal.h"
#include "Clause.h"

#include <iostream>
#include <string>
#include <set>
#include <fstream>

using namespace std;


//Class to parse input file.
class InputParser{


public:
  string filename;

  InputParser(const string& aFilename):filename(aFilename){}

  static int DEBUG_LEVEL;
private:

  //Internal functions used to parse.
  static Term* makeLiteral(const string& line, int&index);
  static Term* makeBinaryClause(const string& line, int& index);
  static Term* makeNuUnClause(const string& line, int& index);

public:

  //valid syntactic forms:
  //for operator A, clauses *, and literal a
  //A(*)
  //(* A *)
  // a
  // A+' '

  //Parses a line into a Term starting at index 'index' of the string 'line'.
  //moves index to the first character after the term
  static Term* makeTerm(const string& line, int &index);

  pair<set<Term*>,set<Term*> > parse();

};



#endif
