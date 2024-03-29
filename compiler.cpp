#include <iostream>
#include <fstream>
#include <iterator>
#include <list>
#include <regex>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <array>
using namespace std;
static vector<pair<string,string>> matches;

void program();
void statement();
void match(string t);
void matchlex(string t);
void if_stmt();
void for_stmt();
void return_stmt();
void break_stmt();
void continue_stmt();
void expr();
void exp();
void method_name();
void location();
void method_call();
void var_decl();
void method_decl();
void field_decl();
void block();
bool int_literal();
bool literal();
bool bin_op();
bool type();
string string_literal();
void error(string location);
bool exprbool();
void comment();
void Parser();
void inli();

string lookaheadlex(unsigned long long =1);
string lookaheadtok(unsigned long long =1);


static string lexem;

static string token;

static size_t counter=0;

static size_t parsertree=1;

static size_t inl=1;

static fstream file;

static string lookAheadtok;
static string lookAheadlex;
int main()
{
  ifstream readFile;
  ofstream outputFile("scanner_output.txt");

  readFile.open("tiny_sample_code.txt");

    vector <pair<string,string>> patterns
        {

        {"//[^\n]*","comment"},
        {"\"[^\"]*\"","string_literal"},                    //|\"[a-zA-Z0-9?/><:;,.!@#$%\\^&\\*\\(\\)\\+=-_`~/ ]*\"
        {"0x[0-9a-fA-f]+","hex_literal"},
        {"[0-9]+[a-zA-Z_]+[0-9a-zA-Z_]*","id_error"},
        {"[0-9]+" , "decimal_literal"} ,
        {"true|false","bool_literal"},
       // {"int|boolean","type"},
        {"==|!=","eq_op"},
        {"=|-=|\\+=","assign_op"},
        {"if|for|return|break|continue|class|callout|void|int|boolean" , "reserve_word"},
        {"[a-zA-Z_]+[0-9a-zA-Z_]*", "id"},
        {"\\+|-|\\*|/|%", "arth_op"},
        {"&&|\\|\\|","cond_op"},
        {"<|>|<=|>=" , "rel_op"},
        {"\\(","("},
        {"\\)",")"},
        {"\\[","["},
        {"\\]","]"},
        {"\\{","{"},
        {"\\}","}"},
        {"'[^ ]'","char_literal"},
        {"\"","\""},
        {"'[^ ][^ ]+'","char_literal_error"},
        {";",";"},
        {",",","},
        {"!","!"},
        {"[#@~//^]+","unknown_figure"}


 };


  string reg;
  for(auto const& x : patterns)
      reg += "(" + x.first + ")|"; // parenthesize the submatches

  reg.pop_back();// erase the last char of string OR reduce the length by one for the | sign
  string str;
  while( getline(readFile,str))
  {


       regex re(reg, regex::extended); // regex::extended for longest match

        auto words_begin = sregex_iterator(str.begin(), str.end(), re);
        auto words_end = sregex_iterator();

        for(auto it = words_begin; it != words_end; ++it)
        {
            size_t index = 0;

            for( ; index < it->size(); ++index)

                if(!it->str(index + 1).empty()) // determine which submatch was matched
                    break;



                if (patterns[index].second!="comment")

                matches.push_back(make_pair(it->str(),patterns[index].second));




        }


}

  cout<<"running scannner..."<<endl;
  outputFile<<"lexem" << " ---> " << "token "<< endl;

  for ( auto match = matches.begin(); match != matches.end(); ++match ){

      outputFile<<match->first << " ---> " << match->second << endl;
  }
  auto begin=matches.end();
  auto end=begin+10;
  cout<<"scanner succeeded."<<endl<<endl;
  for(auto i=begin+1;i<end;i++)
  {
      matches.push_back(make_pair("end of lexem","end of token"));
  }

  token=matches.at(counter).second;
  lexem=matches.at(counter).first;
  lookAheadtok=matches.at(counter+1).second;
  lookAheadlex=matches.at(counter+1).first;
  program();//entern to parser
    return 0;
}


//parser methods
void program(){
    inl=1;
    comment();
    if(lexem=="class")
    {
        Parser();
        matchlex("class");
        comment();
        if(lexem=="Program")

        {
             matchlex("Program");

            comment();
            if (token=="{")
            {

                match("{");
                comment();


                comment();
                while(type()&&lookaheadlex(2)!="(")
                {
                field_decl();
                }
                comment();
                while(lookaheadlex(2)=="("&&(lexem=="void"||type()))
                {
                method_decl();

                }
                comment();
                if (token=="}")
                {
                    match("}");
                    comment();
                    if(token!="end of token")
                        cout<<lexem <<" should be deleted"<<endl;
                    else

                    cout << "Program Succeeded" << endl ;
                }

            else
                    error("} ");


         }
           else{
                error("{ ");
            }
      }
        else{
            error("Program ");
        }

    }
    else {
        error("class ");
    }


}

void field_decl(){
    inl=1;
    Parser();
    comment();
    if(type())
    {
        match("type");
        match("id");
        comment();
        if(token=="[")
        {
            match("[");
            int_literal();
            match("int_literal");
            match("]");
        }
        comment();
    while(token==","){
        match(",");
        match("id");
        comment();
        if(token=="[")
        {
            match("[");
            int_literal();
            match("int_literal");
            match("]");
        }
    }
    match(";");
    cout<<"field_decl suceeded"<<endl;
    }
     parsertree--;

}

void method_decl(){
    inl=1;
     Parser();
    comment();
    if(type())
        match("type");
    else if(lexem=="void")
        matchlex("void");

    else
        cout<<"your method needs a type";
    match("id");


            match("(");
            comment();
            if(type()){
            match("type");
            match ("id");}
            comment();
            while(token==","){
                match(",");
                type();
                match("type");
                match ("id");

            }

            match(")");

   block();

   cout<<"method_decl suceeded"<<endl;
   parsertree--;

    }


    void block(){
        inl=1;
        Parser();
            match("{");
            comment();
            while (type()) {
                        var_decl();

                        }
            comment();
           while(token=="id"||lexem=="if"||lexem=="for"||lexem=="return"||lexem=="break"||lexem=="continue"||lexem=="callout"||lexem=="{"){
                        statement();
                        }

             match("}");
             cout<<"block suceeded"<<endl;
               parsertree--;

}
    void var_decl(){
        inl=1;
        Parser();
        comment();
        if(type())
        {
            match("type");
            match("id");
          comment();
        while(token==","){
            match(",");
            match("id");

        }
        match(";");
        cout<<"var_decl suceeded"<<endl;
    }
         parsertree--;

    }
    void statement() {
        inl=1;
        Parser();
        comment();
        if (lexem == "if") {

            if_stmt();
        }
        else if (lexem == "for") {
            for_stmt();
             cout << "statement Succeeded" << endl;
        }
        else if (lexem == "return") {
            return_stmt();
             cout << "statement Succeeded" << endl;
        }
        else if (lexem == "break") {
            break_stmt();
             cout << "statement Succeeded" << endl;
        }
        else if (lexem == "continue") {
            continue_stmt();
             cout << "statement Succeeded" << endl;
        }
        else if((token=="id"&&lookaheadlex()=="(")||(lexem=="callout")){
            method_call();
            match(";");
             cout << "statement Succeeded" << endl;
        }
        else if (token=="id") {
            location();
            match("assign_op");
            expr();
            match(";");
             cout << "statement Succeeded" << endl;

        }
        else if (token=="{") {
            block();

        }
        else {
            error("statement");
        }
         parsertree--;

    }

    void if_stmt() {
        inl=1;
         Parser();
        cout<<"if statement is running"<<endl;
        matchlex("if");
        match("(");
        expr();

        match(")");
        block();
        cout << "if statement Succeeded" << endl;
        comment();
        if(lexem=="else"){
            matchlex("else");
            block();
             cout << "else statement Succeeded" << endl;
        }
         parsertree--;

    }

    void for_stmt() {
        inl=1;
        Parser();
        matchlex("for");
        match("id");
        matchlex("=");
        expr();
        match(",");
        expr();
        block();
        cout << "for statement Succeeded" << endl;
         parsertree--;
    }

    void return_stmt() {
        inl=1;
         Parser();
        matchlex("return");
        expr();
        match(";");

        cout << "return statement Succeeded" << endl;
         parsertree--;
    }

    void break_stmt() {
        inl=1;
         Parser();
        matchlex("break");
        match(";");
        cout << "break statement Succeeded" << endl;
         parsertree--;
    }

    void continue_stmt() {
        inl=1;
        Parser();
        matchlex("continue");
        match(";");
        cout << "continue statement Succeeded" << endl;
         parsertree--;
    }
    void expr(){
        inl=1;
         Parser();
        exp();
        if(bin_op()){
            match("bin_op");
            expr();
        }
        cout << "expr Succeeded" << endl;
         parsertree--;

    }

    void exp() {//lookahead dare

        if(token=="id"&& lookaheadlex()=="["){
            location();

        }
        else if(token=="id"&& lookAheadlex=="("){
            method_call();

        }
        else if(token=="id"){
            location();

        }
        else if (lexem=="callout"){
            method_call();
        }
        else if(literal()){
            match("literal");

        }
        else if (token=="(") {
            match("(");
            expr();
            match(")");

        }
        else if (token=="!")
        {
            match("!");
            expr();
        }
        else if(lexem=="-")
        {
            matchlex("-");
            expr();
        }



    }

void method_call(){
    inl=1;
     Parser();
    comment();
    if (lexem=="callout"){
        matchlex("callout");
        match("(");
        match("string_literal");

        comment();
        while(token==","){

        match(",");

        comment();
        if(token=="string_literal")
        match("string_literal");
        else {

            expr();
        }}
        match(")");

        cout<<"method_call succeeded!"<<endl;


    }

    else if (token=="id") {
        method_name();
        match("(");
        expr();
        comment();
        while(token==",")
        {
            match(",");
            expr();
        }
        match(")");
        cout<<"method_call succeeded!"<<endl;


    }
     parsertree--;
}









void location(){
    inl=1;
     Parser();
    comment();
    if(token=="id")
    {
        match("id");}
        comment();
        if(token=="["){
            match("[");
            expr();
            match("]");
        }
         cout << "location Succeeded" << endl;
          parsertree--;

    }
void method_name(){
    inl=1;
     Parser();
      comment();
      if(token=="id")
    {
        match("id");

      }
       parsertree--;
}




bool int_literal(){
    inl=1;

    comment();
    if (token=="decimal_literal"||token=="hex_literal"){
        token="int_literal";
        cout << "int_literal replacement succeeded"<<endl;

        return 1;
}
    return 0;
}
bool literal(){
    inl=1;

    comment();
    int_literal();
    comment();
    if(token=="int_literal"||token=="bool_literal"||token=="char_literal"){
        token="literal";
        cout << "literal replacement succeeded"<<endl;
         return 1;
    }
    return 0;
}
bool bin_op(){
    inl=1;

    comment();
    if(token=="cond_op"||token=="rel_op"||token=="eq_op"||token=="arth_op"){
      token="bin_op";
      cout<<"bin_op replacement done"<<endl;
      return 1;
    }
  return 0;
}
bool type(){
    inl=1;

    comment();
    if(lexem=="int"||lexem=="boolean"){
      token="type";
      cout<<"type replacement done"<<endl;
      return 1;}
    return 0;


}
bool exprbool(){
    inl=1;

    comment();
    if((token=="id"&& lookaheadlex()=="[")||(token=="id"&& lookAheadlex=="(")||(token=="id")||(token=="(")||(lexem=="callout")|| (token=="!")||(lexem=="-"))
        return true;
    return false;

}



void match(string t) {

   inli();
    comment();
    while (token=="comment"){
       ++counter;
       token=matches.at(counter).second;
       lexem=matches.at(counter).first;
    }
    comment();
    if (t ==token&&matches.at(counter).second!="end of token") {
        counter++;
        token=matches.at(counter).second;
        lexem=matches.at(counter).first;
       cout<<"match  *"<<t<<"* done"<<endl;
    }
    else {
        error(t);
    }

}
void matchlex(string t){
    inli();

    comment();
    while(token=="comment"){
     ++counter;
     token=matches.at(counter).second;
     lexem=matches.at(counter).first;}
    comment();
    if(t==lexem&&matches.at(counter).first!="end of lexem"){
        counter++;
        token=matches.at(counter).second;
        lexem=matches.at(counter).first;
        cout<<"match  *"<<t<<"* done"<<endl;
    }

    else {
        error(t);
    }
}
 string lookaheadlex(unsigned long long signal){

     comment();
    if(matches.at(counter).first!="end of lexem" && matches.at(counter+signal-1).first!="end of lexem"){
    lookAheadlex=matches.at(counter+signal).first;
    lookAheadtok=matches.at(counter+signal).second;
    }
    else{
        lookAheadlex="comment";
        lookAheadtok="comment";
    }

    return lookAheadlex;
}
string lookaheadtok(unsigned long long signal){
    comment();
    if(matches.at(counter).first!="end of lexem" && matches.at(counter+signal-1).first!="end of lexem"){
    lookAheadlex=matches.at(counter+signal).first;
    lookAheadtok=matches.at(counter+signal).second;
    }
    else{
        lookAheadlex="comment";
        lookAheadtok="comment";
    }

    return lookAheadtok;
}
void error(string location) {
    comment();
    cout <<"*"<< location << "* was expected"<<endl;
    exit(0);
}

void comment()
{
    while(token=="comment"){
        ++counter;
        token=matches.at(counter).second;
        lexem=matches.at(counter).first;}

}
void Parser(){


    parsertree++;
}
void inli(){
    cout<<"line[ "<<parsertree<<" : ";
    cout<<inl<<" ] ";
    inl++;
}


