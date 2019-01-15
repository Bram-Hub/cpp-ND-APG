//Richard lee



#include "InputParser.h"

#include <iostream>
#include <fstream>

int InputParser::DEBUG_LEVEL=10;

//Constructs an instance of the literal class starting at 'index'
//Will set 'index' to the space after the literal
Term* InputParser::makeLiteral(const string& line, int&index){

  if(InputParser::DEBUG_LEVEL>=4){
    cout<<"making Literal term from index: "<<index<<" of line "<<line<<endl;
  }

  string name="";

  while(islower(line[index])){
    name=name+line[index];
    index++;
  }

  if(InputParser::DEBUG_LEVEL>=5){
    cout<<"done making Literal term. Now at index: "<<index<<" of line "<<line<<endl;
  }

  return new Literal(name);
}


//Constructs a binary clause starting at index
//Presumes that a binary clause has syntax (t1 op t2)
//Will set 'index' to the space after the clause
Term* InputParser::makeBinaryClause(const string& line, int& index){
  if(InputParser::DEBUG_LEVEL>=4){
    cout<<"making Binary Clause term from index: "<<index<<" of line "<<line<<endl;
  }

  if(line[index]!='('){
    cerr<<"Error: invalid parser invocation at index "<<index<<" of line "<<line<<" Should be a binary clause"<<endl;
    return 0;
  }

  Term* terms[2];

  index++;

  terms[0]=makeTerm(line,index);

  if(InputParser::DEBUG_LEVEL>=5){
    cout<<"done making first term of Binary Clause. Now at index: "<<index<<" of line "<<line<<endl;
  }

  if(line[index]!=' '){
    cerr<<"Parse syntax error: binary operator at index "<<index<<" of line "<<line<<" is missing seperator between first clause and operator"<<endl;
    return 0;
  }
  index++;

  string opname;

  while(!((line[index]=='(')||(line[index]==')')||(line[index]==' ')||(islower(line[index])))){
    opname=opname+line[index];
    index++;
  }

  if(line[index]!=' '){
    cerr<<"Parse syntax error: binary operator "<< opname << " at index "<<index<<" of line "<<line<<" encountered an invalid character"<<endl;
    return 0;
  }

  index++;


  if(InputParser::DEBUG_LEVEL>=5){
    cout<<"done making op of Binary Clause. Is "<<opname<<"  Now at index: "<<index<<" of line "<<line<<endl;
  }

  terms[1]=makeTerm(line,index);

  if(line[index]!=')'){
    cerr<<"Parse syntax error: binary operator "<< opname << " at index "<<index<<" of line "<<line<<" is missing closing parenthesis"<<endl;
    return 0;
  }

  index++;
  if(InputParser::DEBUG_LEVEL>=5){
    cout<<"done making Binary Clause. Now at index: "<<index<<" of line "<<line<<endl;
  }

  return new Clause(2,opname,terms);
}



//Constructs a clause for a 0-ary or unary operator starting at 'index'
//Will set 'index' to the space after the clause
Term* InputParser::makeNuUnClause(const string& line, int& index){

  if(InputParser::DEBUG_LEVEL>=4){
    cout<<"making Null/Uniary Clause term from index: "<<index<<" of line "<<line<<endl;
  }

  string opname="";
    
  while(!((line[index]=='(')||(line[index]==' ')||(islower(line[index])))){
    opname=opname+line[index];
    
    index++;
  }

  if(InputParser::DEBUG_LEVEL>=5){
    cout<<"finished making Null/Uniary Clause opname. Is "<<opname<<". now at index: "<<index<<" of line "<<line<<endl;
  }

  if(islower(line[index])){
    cerr<<"Parse syntax error: 0/1ary operator at index "<<index<<" of line "<<line<<" contains invalid characters"<<endl;
    return 0;
  }
	  
  if(line[index]==' '){
    index++;
    if(InputParser::DEBUG_LEVEL>=5){
      cout<<"done making Uniary Clause. Now at index: "<<index<<" of line "<<line<<endl;
    }
    return new Clause(0,opname,0);
  }

  if(line[index]!='('){
    cerr<<"Parse syntax error: expected ' ' or '(' at "<<index<<" of line "<<line<<endl;
    return 0;
  }

  index++;

  Term* inner=makeTerm(line,index);

  if(line[index]!=')'){
    cerr<<"Parse syntax error: unary operator "<<opname<<" at index "<<index<<" of line "<<line<<" is missing ')'"<<endl;
    return 0;
  }
    
  index++;
  if(InputParser::DEBUG_LEVEL>=5){
    cout<<"done making Uniary Clause. Now at index: "<<index<<" of line "<<line<<endl;
  }

  return new Clause(1,opname,&inner);
}


//Constructs the term starting at
//Will set 'index' to the space after the clause
Term* InputParser::makeTerm(const string& line, int &index){

  if(InputParser::DEBUG_LEVEL>=3){
    cout<<"making term from index: "<<index<<" of line "<<line<<endl;
  }

  char c=line[index];
  if(islower(c))
     return makeLiteral(line,index);
  if(c=='(')
    return makeBinaryClause(line, index);
  else
    return makeNuUnClause(line,index);

  return 0;
}

//Reads the given input file and returns a pair containing 
//the set of premises and the set of conclusions
pair<set<Term*>,set<Term*> > InputParser::parse(){

  ifstream input(filename.c_str());

  set<Term*> premises;
  set<Term*> conclusions;
  string line;

  if(!(input.is_open())){
    cerr<<"Error opening file "+filename<<endl;
    premises.insert(NULL);
    return make_pair(premises,conclusions);
  }


  bool premiseFlag=true;

  if(InputParser::DEBUG_LEVEL>=1)
    cout<<"starting parsing"<<endl;

  while(!input.eof()){

    if(InputParser::DEBUG_LEVEL>=2)
      cout<<"starting parse loop"<<endl;

    getline(input,line);

    if(InputParser::DEBUG_LEVEL>=4)
      cout<<"read line : "<<line<<endl;

    if(line[0]=='-'){
      if(InputParser::DEBUG_LEVEL>=2)
	cout<<"break found. now parsing conclusions"<<endl;

      premiseFlag=false;
      continue;
    }

    int index=0;


    Term* t=makeTerm(line,index);

    if(InputParser::DEBUG_LEVEL>=2){
      cout<<"adding term"<<endl;
    }

    if(premiseFlag)
      premises.insert(t);
    else
      conclusions.insert(t);

  }

  input.close();

  if(InputParser::DEBUG_LEVEL>=1){
    cout<<"starting parsing"<<endl;
  }

  return make_pair(premises,conclusions);
}
