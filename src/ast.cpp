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

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

void FloatExpr::genCode(Code &code){
}

void SubExpr::genCode(Code &code){
}

void DivExpr::genCode(Code &code){
}

void IdExpr::genCode(Code &code){
}

string ExprStatement::genCode(){
    return "Expr statement code generation\n";
}

string IfStatement::genCode(){
    return "If statement code generation\n";
}

void MethodInvocationExpr::genCode(Code &code){
    
}

string AssignationStatement::genCode(){
    return "Assignation statement code generation\n";
}

void GteExpr::genCode(Code &code){
}

void LteExpr::genCode(Code &code){
}

void EqExpr::genCode(Code &code){
}

void ReadFloatExpr::genCode(Code &code){
    
}

string PrintStatement::genCode(){
    return "Print statement code generation\n";
}

string ReturnStatement::genCode(){
    return "Return statement code generation\n";
}

string MethodDefinitionStatement::genCode(){
    return "Method definition code generation\n";
}