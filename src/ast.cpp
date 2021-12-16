#include "ast.h"
#include <iostream>
#include <sstream>
#include <set>
#include "asm.h"

const char * floatTemps[] = {"$f0",
                            "$f1",
                            "$f2",
                            "$f3",
                            "$f4",
                            "$f5",
                            "$f6",
                            "$f7",
                            "$f8",
                            "$f9",
                            "$f10",
                            "$f11",
                            "$f12",
                            "$f13",
                            "$f14",
                            "$f15",
                            "$f16",
                            "$f17",
                            "$f18",
                            "$f19",
                            "$f20",
                            "$f21",
                            "$f22",
                            "$f23",
                            "$f24",
                            "$f25",
                            "$f26",
                            "$f27",
                            "$f28",
                            "$f29",
                            "$f30",
                            "$f31"
                        };

#define FLOAT_TEMP_COUNT 32
set<string> intTempMap;
set<string> floatTempMap;

extern Asm assemblyFile;

int globalStackPointer = 0;
int labelCounter = 0;


string saveState(){
    set<string>::iterator it = floatTempMap.begin();
    stringstream ss;
    ss<<"sw $ra, " <<globalStackPointer<< "($sp)\n";
    globalStackPointer+=4;
    return ss.str();
}


string getFloatTemp(){
    for (int i = 0; i < FLOAT_TEMP_COUNT; i++)
    {
        if(floatTempMap.find(floatTemps[i]) == floatTempMap.end()){
            floatTempMap.insert(floatTemps[i]);
            return string(floatTemps[i]);
        }
    }
    cout<<"No more float registers!"<<endl;
    return "";
}
string retrieveState(string state){
    std::string::size_type n = 0;
    string s = "sw";
    while ( ( n = state.find( s, n ) ) != std::string::npos )
    {
    state.replace( n, s.size(), "lw" );
    n += 2;
    globalStackPointer-=4;
    }
    return state;
}

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

string getNewLabel(string prefix){
    stringstream ss;
    ss<<prefix << labelCounter;
    labelCounter++;
    return ss.str();
}

string saveState(){
    set<string>::iterator it = floatTempMap.begin();
    stringstream ss;
    ss<<"sw $ra, " <<globalStackPointer<< "($sp)\n";
    globalStackPointer+=4;
    return ss.str();
}

void FloatExpr::genCode(Code &code){
    string floatTemp = getFloatTemp();
    code.place = floatTemp;
    stringstream ss;
    ss << "li.s " << floatTemp <<", "<< this->number <<endl;
    code.code = ss.str();
}

void SubExpr::genCode(Code &code){
    Code leftCode, rightCode;
    stringstream ss;
    code.place = getFloatTemp();
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    ss << leftCode.code << endl
        << rightCode.code <<endl
        << "sub "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;
}

void DivExpr::genCode(Code &code){
    Code leftCode, rightCode;
    stringstream ss;
    code.place = getFloatTemp();
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    ss << leftCode.code << endl
    << rightCode.code <<endl
    <<  "div.s "<< code.place<<", "<< leftCode.place <<", "<< rightCode.place;

}

void IdExpr::genCode(Code &code){
    if(floatTempMap.find(this->id) == floatTempMap.end()){
        string floatTemp = getFloatTemp();
        code.place = floatTemp;
        code.code = "l.s "+ floatTemp + ", "+ this->id + "\n";
    }
}

string ExprStatement::genCode(){
    Code exprCode;
    this->expr->genCode(exprCode);
    releaseFloatTemp(exprCode.place);
    return exprCode.code;
}

string IfStatement::genCode(){
    string endIfLabel = getNewLabel("endif");
    Code exprCode;
    this->conditionalExpr->genCode(exprCode);
    stringstream code;
    code << exprCode.code << endl;
    code << "bc1f "<< endIfLabel <<endl;
    list<Statement *>::iterator currStat = this->trueStatement.begin();
    while (currStat != this->trueStatement.end())
    {
        Statement * stmt = *currStat;
        if(stmt != NULL){
            code<< stmt->genCode()<<endl;
        }
        currStat++;
    }
    currStat = this->falseStatement.begin();
    while (currStat != this->falseStatement.end())
    {
        Statement * stmt = *currStat;
        if(stmt != NULL){
            code<< stmt->genCode()<<endl;
        }
        currStat++;
    }
    code<< endIfLabel <<" :"<< endl;
    releaseFloatTemp(exprCode.place);
    return code.str();
}

void MethodInvocationExpr::genCode(Code &code){
 list<Expr *>::iterator it = this->expressions.begin();
    list<Code> codes;
    stringstream ss;
    Code argCode;
    while (it != this->expressions.end())
    {
        (*it)->genCode(argCode);
        ss << argCode.code <<endl;
        codes.push_back(argCode);
        it++;
    }

    int i = 0;
    list<Code>::iterator placesIt = codes.begin();
    while (placesIt != codes.end())
    {
        releaseFloatTemp((*placesIt).place);
        ss << "mfc1 $a"<<i<<", "<< (*placesIt).place<<endl;
        i++;
        placesIt++;
    }

    ss<< "jal "<< this->id<<endl;
    string reg;
    reg = getFloatTemp();
    ss << "mfc1 $v0, "<< reg<<endl;
    code.code = ss.str();
    code.place = reg;
}

string AssignationStatement::genCode(){
    Code ststCode;
    stringstream ss;
    this->value->genCode(ststCode);
    ss<< ststCode.code <<endl;
    string name = ((IdExpr *)this->value)->id;
    if(floatTempMap.find(name) == floatTempMap.end())
        ss << "s.s "<<ststCode.place << ", "<<name <<endl;
    releaseFloatTemp(ststCode.place);
    ststCode.code = ss.str();
    return ststCode.code;
}

void LteExpr::genCode(Code &code){
    Code leftCode;
    Code rightCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code <<endl<< rightCode.code<<endl;
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    ss<< "c.le.s "<< leftCode.place<< ", "<< rightCode.place<<endl;
    code.code = ss.str();
}

void LteExpr::genCode(Code &code){
    Code leftCode;
    Code rightCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code <<endl<< rightCode.code<<endl;
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    ss<< "c.le.s "<< leftCode.place<< ", "<< rightCode.place<<endl;
    code.code = ss.str();
}

void EqExpr::genCode(Code &code){
    Code leftCode;
    Code rightCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code <<endl<< rightCode.code<<endl;
    releaseFloatTemp(leftCode.place);
    releaseFloatTemp(rightCode.place);
    ss<< "c.lt.s "<< rightCode.place<< ", "<< leftCode.place<<endl;
    code.code = ss.str();
}

void ReadFloatExpr::genCode(Code &code){

}

string PrintStatement::genCode(){
    list<Expr *>::iterator it = this->expressions.begin();
    stringstream ss;
    Code exprCode;
    while (it != this->expressions.end())
    {
        (*it)->genCode(exprCode);
        it++;
    }
        ss<< exprCode.code<<endl;
        ss << "mov.s $f12, "<< exprCode.place<<endl
        << "li $v0, 2"<<endl
        << "syscall"<<endl;
        return ss.str();
}

string ReturnStatement::genCode(){
   Code exprCode;
    this->expr->genCode(exprCode);
    releaseFloatTemp(exprCode.place);
    stringstream ss;
    ss << exprCode.code << endl
    << "move $v0, "<< exprCode.place <<endl;
    return ss.str();
}

string MethodDefinitionStatement::genCode(){
    if(this->stmts.size() == 0)
        return "";

    int stackPointer = 4;
    globalStackPointer = 0;
    stringstream code;
    code << this->id<<": "<<endl;
    string state = saveState();
    code <<state<<endl;
    if(this->params.size() > 0){
        list<string >::iterator it = this->params.begin();
        for(int i = 0; i< this->params.size(); i++){
            code << "sw $a"<<i<<", "<< stackPointer<<"($sp)"<<endl;
            stackPointer +=4;
            globalStackPointer +=4;
            it++;
        }
    }
    list<Statement *>::iterator stmt = this->stmts.begin();
    while (stmt != stmts.end())
    {
        (*stmt)->genCode();
        (*stmt)++;
    }
    stringstream sp;
    int currentStackPointer = globalStackPointer;
    sp << endl<<"addiu $sp, $sp, -"<<currentStackPointer<<endl;
    code << retrieveState(state);
    code << "addiu $sp, $sp, "<<currentStackPointer<<endl;
    code <<"jr $ra"<<endl;
    floatTempMap.clear();
    string result = code.str();
    result.insert(id.size() + 2, sp.str());
    return result;
}