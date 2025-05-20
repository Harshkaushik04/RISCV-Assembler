#include "../headers/asm_to_mc.hpp"
using namespace std;

string int_to_hex_string(int num){
    stringstream ss;
    ss<<setfill('0')<<setw(8)<<hex<<num;
    return ss.str();
}


std::string int_to_binary_string(int value,int numBits) {
    return bitset<32>(value).to_string().substr(32 - numBits);  
}

Assembler::Assembler(){
    this->inputFilePath="";
    this->inputInstructions={};
    this->dividedDataInstructions={};
    this->dividedTextInstructions={};
    this->outputTextInstructions={};
    this->outputDataInstructions={};
    this->symbolTable={};
    this->outputFilePath="";
    this->register_to_int_mapping={};
    for(int i=0;i<32;i++){
        register_to_int_mapping["x"+to_string(i)]=i;
    }
    for(int i=0;i<3;i++){
        register_to_int_mapping["t"+to_string(i)]=i+5;
    }
    for(int i=0;i<2;i++){
        register_to_int_mapping["s"+to_string(i)]=i+8;
    }
    for(int i=0;i<8;i++){
        register_to_int_mapping["a"+to_string(i)]=i+10;
    }
    for(int i=2;i<12;i++){
        register_to_int_mapping["s"+to_string(i)]=i+16;
    }
    for(int i=3;i<7;i++){
        register_to_int_mapping["t"+to_string(i)]=i+25;
    }
}

Assembler::Assembler(string inputFilePath){
    this->inputFilePath=inputFilePath;
    this->inputInstructions={};
    this->dividedDataInstructions={};
    this->dividedTextInstructions={};
    this->outputTextInstructions={};
    this->outputDataInstructions={};
    this->symbolTable={};
    this->outputFilePath="";
    this->register_to_int_mapping={};
    for(int i=0;i<32;i++){
        register_to_int_mapping["x"+to_string(i)]=i;
    }
    for(int i=0;i<3;i++){
        register_to_int_mapping["t"+to_string(i)]=i+5;
    }
    for(int i=0;i<2;i++){
        register_to_int_mapping["s"+to_string(i)]=i+8;
    }
    for(int i=0;i<8;i++){
        register_to_int_mapping["a"+to_string(i)]=i+10;
    }
    for(int i=2;i<12;i++){
        register_to_int_mapping["s"+to_string(i)]=i+16;
    }
    for(int i=3;i<7;i++){
        register_to_int_mapping["t"+to_string(i)]=i+25;
    }
}

void Assembler::read_instructions(){
    // INPUT ->inputFilePath
    // OUTPUT -> inputInstructions,dividedDataInstructions,dividedTextInstructions,symbolTable
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


void Assembler::convertALL(){
    //INPUT->inputInstructions,dividedDataInstructions,dividedTextInstructions,symbolTable
    //OUTPUT->outputTextInstructions,outputDataInstructions

    //text instrcutions
    string first;
    for(int index=0;index<dividedTextInstructions.size();index++){
        first=dividedTextInstructions[index][0];
        if(first=="add"||first=="sub"||first=="sll"||first=="slt"||first=="sltu"||first=="xor"||first=="srl"||
            first=="sra"||first=="or"||first=="and"||first=="mul"||first=="div"||first=="rem"){
                convertRformatInstructions(index);
            }
        else if(first=="addi"||first=="slti"||first=="sltiu"||first=="xori"||first=="ori"||first=="andi"
            ||first=="slli"||first=="srli"||first=="srai"||first=="ld"||first=="lb"||first=="lh"||first=="lw"||
            first=="lbu"||first=="lhu"){
                convertIformatInstructions(index);
            }
        else if(first=="sb"||first=="sh"||first=="sw"||first=="sd"){
            convertSformatInstructions(index);
        }
        else if(first=="beq"||first=="bne"||first=="bge"||first=="blt"||first=="bltu"||first=="bgeu"){
            convertBformatInstructions(index);
        }
        else if(first=="lui"||first=="auipc"){
            convertUformatInstructions(index);
        }
        else if(first=="jal"||first=="jalr"){
            convertJformatInstructions(index);
        }
        else{
            cerr<<"Instruction not implemented"<<endl;
        }
    }
}
// ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND,MUL,DIV,REM,
// ADDI,SLTI,SLTIU,XORI,ORI,ANDI,SLLI,SRLI,SRAI,LD,LB,LH,LW,LBU,LHU,
// SB,SH,SW,SD,
// BEQ,BNE,BGE,BLT,BLTU,BGEU,
// LUI,AUIPIC,
// JAL,JALR
void Assembler::convertRformatInstructions(int index){
    string first,second,third,fourth;
    first=dividedTextInstructions[index][0];
    second=dividedTextInstructions[index][1];
    third=dividedTextInstructions[index][2];
    fourth=dividedTextInstructions[index][3];
    u_int32_t rd,rs1,rs2,result=0,opcode,func3,func7;
    string opcodeString,rdString,func3String,rs1String,rs2String,func7String;
    u_int32_t temp;
    string Total,address,MCinstruction,ASMinstruction,comment;
    rd=register_to_int_mapping[second];
    rs1=register_to_int_mapping[third];
    rs2=register_to_int_mapping[fourth];
    opcode=0b0110011;
    if(first=="add"){
        func3=0;
        func7=0;
    }
    else if(first=="sub"){
        func3=0;
        func7=0b0100000;
    }
    else if(first=="sll"){
        func3=0b001;
        func7=0;
    }
    else if(first=="slt"){
        func3=0b010;
        func7=0;
    }
    else if(first=="sltu"){
        func3=0b011;
        func7=0;
    }
    else if(first=="xor"){
        func3=0b100;
        func7=0;
    }
    else if(first=="srl"){
        func3=0b101;
        func7=0;
    }
    else if(first=="sra"){
        func3=0b101;
        func7=0b0100000;
    }
    else if(first=="or"){
        func3=0b110;
        func7=0;
    }
    else if(first=="and"){
        func3=0b111;
        func7=0;
    }
    else if(first=="mul"){
        func3=0;
        func7=0b0000001;
    }
    else if(first=="div"){
        func3=0b100;
        func7=0b0000001;
    }
    else if(first=="rem"){
        func3=0b110;
        func7=0b0000001;
    }
    result=opcode+rd<<7+func3<<12+rs1<<15+rs2<<20+func7<<25;
    MCinstruction="0x"+int_to_hex_string(result);
    ASMinstruction=first+" "+second+","+third+","+fourth;
    temp=4*index;
    address="0x"+int_to_hex_string(temp);
    opcodeString=int_to_binary_string(opcode,7);
    rdString=int_to_binary_string(rd,5);
    rs1String=int_to_binary_string(rs1,5);
    rs2String=int_to_binary_string(rs2,5);
    func3String=int_to_binary_string(func3,3);
    func7String=int_to_binary_string(func7,7);
    Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
    +"-"+func7String+"-"+rdString+"-"+rs1String+"-"+rs2String+"-NULL";
    outputTextInstructions.emplace_back(Total);
}
void Assembler::convertIformatInstructions(int index){}
void Assembler::convertSformatInstructions(int index){}
void Assembler::convertBformatInstructions(int index){}
void Assembler::convertUformatInstructions(int index){}
void Assembler::convertJformatInstructions(int index){}

