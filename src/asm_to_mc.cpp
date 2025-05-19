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
        cerr<<"error at line 33:asm_to_mc.cpp, Couldnt find input file path"<<inputFilePath<<endl;
    }
    while(getline(inputFile,line)){
        inputInstructions.emplace_back(line);
    }
    for(string line:inputInstructions){
        if(line.size()>=5){
            if(line.substr(0,5)==".text"){
            currentMode=ReadingMode::TEXT;
            continue;
        }
        else if(line.substr(0,5)==".data"){
            currentMode=ReadingMode::DATA;
            continue;
        }
        }
        vector<string> tokens;
        string current;
        for (char ch : line) {
            if (ch==' '||ch==','||ch =='('||ch == ')') {
                if (!current.empty()) {
                    // cout<<"current and current.back():"<<current<<"  "<<current.back()<<endl;
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
                            tokens.emplace_back(label);
                            current.clear();
                            continue;
                        }
                    }
                    //5 cases of .data
                    if(current==".byte"){
                        dataOffestMode=currentDataOffestMode::BYTE;
                        current.clear();
                        continue;
                    }
                    else if(current==".half"){
                        dataOffestMode=currentDataOffestMode::HALF;
                        current.clear();
                        continue;
                    }
                    else if(current==".word"){
                        dataOffestMode=currentDataOffestMode::WORD;
                        current.clear();
                        continue;
                    }
                    else if(current==".dword"){
                        dataOffestMode=currentDataOffestMode::DWORD;
                        current.clear();
                        continue;
                    }
                    else if(current==".asciz"){
                        dataOffestMode=currentDataOffestMode::ASCIZ;
                        current.clear();
                        continue;
                    }
                    //changing currentDataOffset for asciz
                    if(dataOffestMode==currentDataOffestMode::ASCIZ && current!=".asciz"){
                        currentDataOffset+=current.size()+1; //n+1
                        dataOffestMode=currentDataOffestMode::OFF;
                    }
                    else if(dataOffestMode==currentDataOffestMode::BYTE && current!=".byte"){
                        currentDataOffset+=1;
                    }
                    else if(dataOffestMode==currentDataOffestMode::HALF && current!=".byte"){
                        currentDataOffset+=2;
                    }
                    else if(dataOffestMode==currentDataOffestMode::WORD && current!=".byte"){
                        currentDataOffset+=4;
                    }
                    else if(dataOffestMode==currentDataOffestMode::DWORD && current!=".byte"){
                        currentDataOffset+=8;
                    }
                    tokens.emplace_back(current);
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
        dataOffestMode=currentDataOffestMode::OFF;
        if(current.back()==':'){
            // cout<<"h1"<<"current and current.back():"<<current<<"  "<<current.back()<<endl;
            string label=line.substr(0,current.size()-1);
            if(currentMode==ReadingMode::TEXT){
                string label=current.substr(0,current.size()-1);
                u_int32_t address=dividedTextInstructions.size()*4;
                symbolTable[label]=address;
                current.clear();
                // cout<<"hello"<<endl;
                // cout<<"divided text instructions:"<<endl;
                for(vector<string> list:dividedTextInstructions){
                    for(string word:list){
                        cout<<word<<endl;
                    }
                }
                continue;
            }
            else{ //ReadingMode::DATA
                string label=current.substr(0,current.size()-1);
                u_int32_t address=0x10000000+currentDataOffset;
                symbolTable[label]=address;
                tokens.emplace_back(label);
                current.clear();
            }
        }
        if (!current.empty()) tokens.push_back(current);
        if(tokens.size()>0){
            if(currentMode==ReadingMode::DATA){
                dividedDataInstructions.emplace_back(tokens);
            }
            else{
                dividedTextInstructions.emplace_back(tokens);
            }
        }
    }
}




