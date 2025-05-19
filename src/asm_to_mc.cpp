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
    int currentDataOffset=0;
    currentDataOffestMode dataOffestMode=currentDataOffestMode::OFF;
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
            if(currentMode==ReadingMode::TEXT){
                u_int32_t address=dividedTextInstructions.size()*4;
                symbolTable[label]=address;
            }
        }
        vector<string> tokens;
        string current;
        for (char ch : line) {
            if (ch==' '||ch==','||ch =='('||ch == ')') {
                if (!current.empty()) {
                    if(current.back()==':'){
                        if(currentMode==ReadingMode::TEXT){
                            string label=current.substr(0,current.size()-1);
                            u_int32_t address=dividedTextInstructions.size()*4;
                            symbolTable[label]=address;
                            current.clear();
                            continue;
                        }
                        else{ //ReadingMode::DATA
                            string label=current.substr(0,current.size()-1);
                            u_int32_t address=0x10000000+currentDataOffset;
                            symbolTable[label]=address;
                            //current data offset is yet to be changed
                            current.clear();
                            continue;
                        }
                    }
                    //5 cases of .data
                    if(current==".byte"){
                        currentDataOffset+=1;
                    }
                    else if(current==".half"){
                        currentDataOffset+=2;
                    }
                    else if(current==".word"){
                        currentDataOffset+=8;
                    }
                    else if(current==".dword"){
                        currentDataOffset+=16;
                    }
                    else if(current==".asciz"){
                        dataOffestMode=currentDataOffestMode::ASCIZ;
                    }
                    //changing currentDataOffset for asciz
                    if(dataOffestMode==currentDataOffestMode::ASCIZ && current!=".asciz"){
                        currentDataOffset+=current.size()+1; //n+1
                        dataOffestMode=currentDataOffestMode::OFF;
                    }
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




