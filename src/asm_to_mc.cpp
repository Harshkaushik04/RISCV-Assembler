#include "../headers/asm_to_mc.hpp"
using namespace std;

Assembler::Assembler(){
    this->inputFilePath="";
    this->inputInstructions={};
    this->dividedDataInstructions={};
    this->dividedTextInstructions={};
    this->outputInstructions={};
    this->outputMCinstructions={};
    this->symbolTable={};
    this->outputFilePath="";
}

Assembler::Assembler(string inputFilePath){
    this->inputFilePath=inputFilePath;
    this->inputInstructions={};
    this->dividedDataInstructions={};
    this->dividedTextInstructions={};
    this->outputInstructions={};
    this->outputMCinstructions={};
    this->symbolTable={};
    this->outputFilePath="";
}

void Assembler::makeInputInstructions(){
    fstream inputFile(inputFilePath);
    string line;
    ReadingMode currentMode=ReadingMode::TEXT;
    if(!inputFile.is_open()){
        cerr<<"error at line 31:asm_to_mc.cpp, Couldnt find input file path"<<inputFilePath<<endl;
    }
    while(getline(inputFile,line)){
        inputInstructions.emplace_back(line);
    }
    for(string line:inputInstructions){
        if(line==".text"){
            currentMode=ReadingMode::TEXT;
            continue;
        }
        else if(line==".data"){
            currentMode=ReadingMode::DATA;
            continue;
        }
        if(line.back()==':'){
            string label=line.substr(0,line.size()-1);
            if(currentMode==ReadingMode::DATA){
                cerr<<"error at line 48:asm_to_mc.cpp, current readin mode is data but should have been text"<<endl;
            }
            else{
                u_int32_t address=dividedTextInstructions.size()*4;
                symbolTable[label]=address;
            }
        }
        vector<string> tokens;
        string current;
        for (char ch : line) {
            if (ch==' '||ch==','||ch =='('||ch == ')') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            }
            else if(ch=='#'){
                break;
            } 
            else {
                current += ch;
            }
        }
        if (!current.empty()) tokens.push_back(current);
        if(currentMode==ReadingMode::DATA){
            dividedDataInstructions.emplace_back(tokens);
        }
        else{
            dividedTextInstructions.emplace_back(tokens);
        }
    }
}




