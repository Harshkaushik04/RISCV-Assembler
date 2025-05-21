#include "../headers/asm_to_mc.hpp"
using namespace std;

string int_to_hex_string(int num){
    stringstream ss;
    ss<<setfill('0')<<setw(8)<<hex<<num;
    return ss.str();
}


string int_to_binary_string(int value,int numBits) {
    return bitset<32>(value).to_string().substr(32 - numBits);  
}

vector<string> string_to_hex_bytes(string input) {
    vector<string> hex_bytes;
    for (unsigned char c : input) {
        stringstream ss;
        ss << hex << setw(2) << setfill('0') << static_cast<int>(c);
        hex_bytes.push_back(ss.str());
    }
    return hex_bytes;
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
    register_to_int_mapping["zero"]=0;
    register_to_int_mapping["ra"]=1;
    register_to_int_mapping["sp"]=2;
    register_to_int_mapping["gp"]=3;
    register_to_int_mapping["tp"]=4;
    register_to_int_mapping["fp"]=8;
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
    register_to_int_mapping["zero"]=0;
    register_to_int_mapping["ra"]=1;
    register_to_int_mapping["sp"]=2;
    register_to_int_mapping["gp"]=3;
    register_to_int_mapping["tp"]=4;
    register_to_int_mapping["fp"]=8;
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
                            tokens.emplace_back(to_string(address));
                            current.clear();
                            continue;
                        }
                    }
                    //5 cases of .data
                    if(current==".byte"){
                        dataOffestMode=currentDataOffestMode::BYTE;
                        tokens.emplace_back(current);
                        current.clear();
                        continue;
                    }
                    else if(current==".half"){
                        dataOffestMode=currentDataOffestMode::HALF;
                        tokens.emplace_back(current);
                        current.clear();
                        continue;
                    }
                    else if(current==".word"){
                        dataOffestMode=currentDataOffestMode::WORD;
                        tokens.emplace_back(current);
                        current.clear();
                        continue;
                    }
                    else if(current==".dword"){
                        dataOffestMode=currentDataOffestMode::DWORD;
                        tokens.emplace_back(current);
                        current.clear();
                        continue;
                    }
                    else if(current==".asciz"){
                        dataOffestMode=currentDataOffestMode::ASCIZ;
                        tokens.emplace_back(current);
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
                // cout<<"new"<<endl;
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

    //data directives
    for(int index=0;index<dividedDataInstructions.size();index++){
        string second;
        second=dividedDataInstructions[index][1];
        if(second==".byte"){
            handleByteDirective(index);
        }
        else if(second==".half"){
            handleHalfDirective(index);
        }
        else if(second==".word"){
            handleWordDirective(index);
        }
        else if(second==".dword"){
            handleDwordDirective(index);
        }
        else if(second==".asciiz"){
            handleAsciizDirective(index);
        }
    }
}
// ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND,MUL,DIV,REM,
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
    result=opcode|(rd<<7)|(func3<<12)|(rs1<<15)|(rs2<<20)|(func7<<25);
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
    cout<<"====================================="<<endl;
    cout<<"ASMinstruction:"<<ASMinstruction<<endl;
    cout<<"opcodeString:"<<opcodeString<<endl;
    cout<<"func3String:"<<func3String<<endl;
    cout<<"func7String:"<<func7String<<endl;
    cout<<"rdString:"<<rdString<<endl;
    cout<<"rs1String:"<<rs1String<<endl;
    cout<<"rs2String:"<<rs2String<<endl;
    cout<<"====================================="<<endl;
    Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
    +"-"+func7String+"-"+rdString+"-"+rs1String+"-"+rs2String+"-NULL";
    outputTextInstructions.emplace_back(Total);
}

// ADDI,SLTI,SLTIU,XORI,ORI,ANDI,SLLI,SRLI,SRAI,LD,LB,LH,LW,LBU,LHU,
void Assembler::convertIformatInstructions(int index){
    // cout<<"hi1"<<endl;
    string first=dividedTextInstructions[index][0];
    string second,third,fourth;
    second=dividedTextInstructions[index][1];
    third=dividedTextInstructions[index][2];
    fourth=dividedTextInstructions[index][3];
    u_int32_t rd,rs1,imm,result=0,opcode,func3;
    string opcodeString,rdString,func3String,rs1String,immString;
    u_int32_t temp;
    string Total,address,MCinstruction,ASMinstruction,comment;
    // cout<<"hi:"<<first<<endl;
    if(first=="addi"||first=="slti"||first=="sltiu"||first=="xori"||first=="ori"||first=="andi"
    ||first=="slli"||first=="srli"||first=="srai"){
        rd=register_to_int_mapping[second];
        rs1=register_to_int_mapping[third];
        if(fourth.substr(0,2)=="0x"){
            imm=stoul(fourth,nullptr,16);
        }
        else if(fourth.substr(0,2)=="0b"){
            imm=stoul(fourth,nullptr,2);
        }
        else{
            imm=stoul(fourth);
        }
        opcode=0b0010011;
        if(first=="addi"){
            func3=0;
        }
        else if(first=="slti"){
            func3=0b010;
        }
        else if(first=="sltiu"){
            func3=0b011;
        }
        else if(first=="xori"){
            func3=0b100;
        }
        else if(first=="ori"){
            func3=0b110;
        }
        else if(first=="andi"){
            func3=0b111;
        }
        else if(first=="slli"){
            func3=0b001;
        }
        else if(first=="srli"){
            func3=0b101;
        }
        else if(first=="srai"){
            func3=0b101;
        }
        if(first=="addi"||first=="slti"||first=="sltiu"||first=="xori"||first=="ori"||first=="andi"){
            result=opcode|(rd<<7)|(func3<<12)|(rs1<<15)|(imm<<20);
            MCinstruction="0x"+int_to_hex_string(result);
            ASMinstruction=first+" "+second+","+third+","+fourth;
            temp=4*index;
            address="0x"+int_to_hex_string(temp);
            opcodeString=int_to_binary_string(opcode,7);
            rdString=int_to_binary_string(rd,5);
            rs1String=int_to_binary_string(rs1,5);
            immString=int_to_binary_string(imm,12);
            func3String=int_to_binary_string(func3,3);
            cout<<"====================================="<<endl;
            cout<<"ASMinstruction:"<<ASMinstruction<<endl;
            cout<<"opcodeString:"<<opcodeString<<endl;
            cout<<"func3String:"<<func3String<<endl;
            cout<<"rdString:"<<rdString<<endl;
            cout<<"rs1String:"<<rs1String<<endl;
            cout<<"immString:"<<immString<<endl;
            cout<<"====================================="<<endl;
            Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
            +"-NULL-"+rdString+"-"+rs1String+"-NULL-"+immString;
            outputTextInstructions.emplace_back(Total);
        }
        else{
            u_int32_t func7=0b0100000;
            string func7String=int_to_binary_string(func7,7);
            if(first!="srai"){
                result=opcode|(rd<<7)|(func3<<12)|(rs1<<15)|(imm<<20);
            }
            else{
                result=opcode|(rd<<7)|(func3<<12)|(rs1<<15)|(imm<<20)|(func7<<25);
            }
            MCinstruction="0x"+int_to_hex_string(result);
            ASMinstruction=first+" "+second+","+third+","+fourth;
            temp=4*index;
            address="0x"+int_to_hex_string(temp);
            opcodeString=int_to_binary_string(opcode,7);
            rdString=int_to_binary_string(rd,5);
            rs1String=int_to_binary_string(rs1,5);
            immString=int_to_binary_string(imm,5);
            func3String=int_to_binary_string(func3,3);
            cout<<"====================================="<<endl;
            cout<<"ASMinstruction:"<<ASMinstruction<<endl;
            cout<<"opcodeString:"<<opcodeString<<endl;
            cout<<"func3String:"<<func3String<<endl;
            cout<<"func7String:"<<func7String<<endl;
            cout<<"rdString:"<<rdString<<endl;
            cout<<"rs1String:"<<rs1String<<endl;
            cout<<"immString:"<<immString<<endl;
            cout<<"====================================="<<endl;
            Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
            +"-"+func7String+"-"+rdString+"-"+rs1String+"-NULL-"+immString;
            outputTextInstructions.emplace_back(Total);
        }
    }
    else if(first=="ld"||first=="lb"||first=="lh"||first=="lw"||first=="lbu"||first=="lhu"){
        rd=register_to_int_mapping[second];
        if(third.substr(0,2)=="0x"){
            imm=stoul(third,nullptr,16);
        }
        else if(third.substr(0,2)=="0b"){
            imm=stoul(third,nullptr,2);
        }
        else{
            imm=stoul(third);
        }
        rs1=register_to_int_mapping[fourth];
        opcode=0b0000011;
        if(first=="ld"){
            func3=0b011;
        }
        else if(first=="lb"){
            func3=0;
        }
        else if(first=="lh"){
            func3=0b001;
        }
        else if(first=="lw"){
            func3=0b010;
        }
        else if(first=="lbu"){
            func3=0b100;
        }
        else if(first=="lhu"){
            func3=0b101;
        }
        result=opcode|(rd<<7)|(func3<<12)|(rs1<<15)|(imm<<20);
        MCinstruction="0x"+int_to_hex_string(result);
        ASMinstruction=first+" "+second+","+third+","+fourth;
        temp=4*index;
        address="0x"+int_to_hex_string(temp);
        opcodeString=int_to_binary_string(opcode,7);
        rdString=int_to_binary_string(rd,5);
        rs1String=int_to_binary_string(rs1,5);
        immString=int_to_binary_string(imm,12);
        func3String=int_to_binary_string(func3,3);
        cout<<"====================================="<<endl;
        cout<<"ASMinstruction:"<<ASMinstruction<<endl;
        cout<<"opcodeString:"<<opcodeString<<endl;
        cout<<"func3String:"<<func3String<<endl;
        cout<<"rdString:"<<rdString<<endl;
        cout<<"rs1String:"<<rs1String<<endl;
        cout<<"immString:"<<immString<<endl;
        cout<<"====================================="<<endl;
        Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
        +"-NULL-"+rdString+"-"+rs1String+"-NULL-"+immString;
        outputTextInstructions.emplace_back(Total);
    }
}

// SB,SH,SW,SD
void Assembler::convertSformatInstructions(int index){
    string first=dividedTextInstructions[index][0];
    string second,third,fourth;
    second=dividedTextInstructions[index][1];
    third=dividedTextInstructions[index][2];
    fourth=dividedTextInstructions[index][3];
    u_int32_t rs1,rs2,imm,result=0,opcode,func3;
    string opcodeString,rs2String,func3String,rs1String,immString;
    u_int32_t temp;
    string Total,address,MCinstruction,ASMinstruction,comment;
    opcode=0b0100011;
    rs2=register_to_int_mapping[second];
    if(third.substr(0,2)=="0x"){
        imm=stoul(third,nullptr,16);
    }
    else if(third.substr(0,2)=="0b"){
        imm=stoul(third,nullptr,2);
    }
    else{
        imm=stoul(third);
    }
    rs1=register_to_int_mapping[fourth];
    if(first=="sb"){
        func3=0;
    }
    else if(first=="sh"){
        func3=0b001;
    }
    else if(first=="sw"){
        func3=0b010;
    }
    else if(first=="sd"){
        func3=0b011;
    }
    u_int32_t imm_4_0  = (imm >> 0) & 0b11111;  // Extract bits [4:0]
    u_int32_t imm_11_5 = (imm >> 5) & 0b1111111;  // Extract bits [11:5] 
    result=opcode|(imm_4_0<<7)|(func3<<12)|(rs1<<15)|(rs2<<20)|(imm_11_5<<25);
    MCinstruction="0x"+int_to_hex_string(result);
    ASMinstruction=first+" "+second+","+third+","+fourth;
    temp=4*index;
    address="0x"+int_to_hex_string(temp);
    opcodeString=int_to_binary_string(opcode,7);
    rs2String=int_to_binary_string(rs2,5);
    rs1String=int_to_binary_string(rs1,5);
    immString=int_to_binary_string(imm,12);
    func3String=int_to_binary_string(func3,3);
    cout<<"====================================="<<endl;
    cout<<"ASMinstruction:"<<ASMinstruction<<endl;
    cout<<"opcodeString:"<<opcodeString<<endl;
    cout<<"func3String:"<<func3String<<endl;
    cout<<"rs1String:"<<rs1String<<endl;
    cout<<"rs2String:"<<rs2String<<endl;
    cout<<"immString:"<<immString<<endl;
    cout<<"====================================="<<endl;
    Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
    +"-NULL-NULL-"+rs1String+"-"+rs2String+"-"+immString;
    outputTextInstructions.emplace_back(Total);
}

// BEQ,BNE,BGE,BLT,BLTU,BGEU,
void Assembler::convertBformatInstructions(int index){
    string first=dividedTextInstructions[index][0];
    string second,third,fourth;
    second=dividedTextInstructions[index][1];
    third=dividedTextInstructions[index][2];
    fourth=dividedTextInstructions[index][3];
    u_int32_t rs1,rs2,result=0,opcode,func3,offset;
    string label;
    string opcodeString,rs2String,func3String,rs1String,immString;
    u_int32_t temp;
    string Total,address,MCinstruction,ASMinstruction,comment;
    opcode=0b1100011;
    rs1=register_to_int_mapping[second];
    rs2=register_to_int_mapping[third];
    label=fourth;
    if(first=="beq"){
        func3=0;
    }
    else if(first=="bne"){
        func3=0b001;
    }
    else if(first=="bge"){
        func3=0b101;
    }
    else if(first=="blt"){
        func3=0b100;
    }
    else if(first=="bltu"){
        func3=0b110;
    }
    else if(first=="bgeu"){
        func3=0b111;
    }
    auto it = symbolTable.find(label);
    if (it != symbolTable.end()) {
        u_int32_t currentAddress=4*index;
        offset=(it->second-currentAddress)/2;
    } else {
        cerr<<"Label:"<<label<<" not found in symbol table"<<endl;
    }
    u_int32_t offset_10=(offset>>10)&0b1;
    u_int32_t offset_3_0=offset&0b1111;
    u_int32_t offset_9_4=(offset>>4)&0b111111;
    u_int32_t offset_11=(offset>>11)&0b1;
    result=opcode|(offset_10<<7)|(offset_3_0<<8)|(func3<<12)|(rs1<<15)|(rs2<<20)|(offset_9_4<<25)|(offset_11<<31);
    MCinstruction="0x"+int_to_hex_string(result);
    ASMinstruction=first+" "+second+","+third+","+fourth;
    temp=4*index;
    address="0x"+int_to_hex_string(temp);
    opcodeString=int_to_binary_string(opcode,7);
    rs2String=int_to_binary_string(rs2,5);
    rs1String=int_to_binary_string(rs1,5);
    u_int32_t imm=offset<<1;
    immString=int_to_binary_string(imm,13);
    func3String=int_to_binary_string(func3,3);
    cout<<"====================================="<<endl;
    cout<<"ASMinstruction:"<<ASMinstruction<<endl;
    cout<<"opcodeString:"<<opcodeString<<endl;
    cout<<"func3String:"<<func3String<<endl;
    cout<<"rs1String:"<<rs1String<<endl;
    cout<<"rs2String:"<<rs2String<<endl;
    cout<<"immString:"<<immString<<endl;
    cout<<"====================================="<<endl;
    Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
    +"-NULL-NULL-"+rs1String+"-"+rs2String+"-"+immString;
    outputTextInstructions.emplace_back(Total);
}
// LUI,AUIPIC,
void Assembler::convertUformatInstructions(int index){
    string first=dividedTextInstructions[index][0];
    string second,third;
    second=dividedTextInstructions[index][1];
    third=dividedTextInstructions[index][2];
    u_int32_t rd,result=0,opcode,imm;
    string opcodeString,rdString,immString;
    u_int32_t temp;
    string Total,address,MCinstruction,ASMinstruction,comment;
    if(first=="lui"){
        opcode=0b0110111;
    }
    else{
        opcode=0b0010111;
    }
    rd=register_to_int_mapping[second];
    if(third.substr(0,2)=="0x"){
        imm=stoul(third,nullptr,16);
    }
    else if(third.substr(0,2)=="0b"){
        imm=stoul(third,nullptr,2);
    }
    else{
        imm=stoul(third);
    }
    result=opcode|(rd<<7)|(imm<<12);
    MCinstruction="0x"+int_to_hex_string(result);
    ASMinstruction=first+" "+second+","+third;
    temp=4*index;
    address="0x"+int_to_hex_string(temp);
    opcodeString=int_to_binary_string(opcode,7);
    rdString=int_to_binary_string(rd,5);
    immString=int_to_binary_string(imm,20);
    cout<<"====================================="<<endl;
    cout<<"ASMinstruction:"<<ASMinstruction<<endl;
    cout<<"opcodeString:"<<opcodeString<<endl;
    cout<<"rdString:"<<rdString<<endl;
    cout<<"immString:"<<immString<<endl;
    cout<<"====================================="<<endl;
    Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-NULL-NULL-NULL-NULL-NULL-"+immString;
    outputTextInstructions.emplace_back(Total);

}
// JAL,JALR
void Assembler::convertJformatInstructions(int index){
    string first=dividedTextInstructions[index][0];
    if(first=="jal"){
        string second,third;
        second=dividedTextInstructions[index][1];
        third=dividedTextInstructions[index][2];
        string label;
        u_int32_t rd,result=0,opcode,imm;
        string opcodeString,rdString,immString;
        u_int32_t temp;
        string Total,address,MCinstruction,ASMinstruction,comment;
        opcode=0b1101111;
        rd=register_to_int_mapping[second];
        label=third;
        auto it = symbolTable.find(label);
        if (it != symbolTable.end()) {
            u_int32_t currentAddress=4*index;
            imm=it->second-currentAddress;
        } else {
            cerr<<"Label:"<<label<<" not found in symbol table"<<endl;
        }
        u_int32_t offset=imm>>1;
        u_int32_t offset_18_11=(offset>>11)&0b11111111;
        u_int32_t offset_10=(offset>>10)&0b1;
        u_int32_t offset_9_0=(offset>>0)&0b1111111111;
        u_int32_t offset_20=(offset>>20)&0b1;
        result=opcode|(rd<<7)|(offset_18_11<<12)|(offset_10<<20)|(offset_9_0<<21)|(offset_20<<31);
        MCinstruction="0x"+int_to_hex_string(result);
        ASMinstruction=first+" "+second+","+third;
        temp=4*index;
        address="0x"+int_to_hex_string(temp);
        opcodeString=int_to_binary_string(opcode,7);
        rdString=int_to_binary_string(rd,5);
        immString=int_to_binary_string(imm,20);
        cout<<"====================================="<<endl;
        cout<<"ASMinstruction:"<<ASMinstruction<<endl;
        cout<<"opcodeString:"<<opcodeString<<endl;
        cout<<"rdString:"<<rdString<<endl;
        cout<<"immString:"<<immString<<endl;
        cout<<"====================================="<<endl;
        Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-NULL-NULL-NULL-NULL-NULL-"+immString;
        outputTextInstructions.emplace_back(Total);
    }
    else if(first=="jalr"){
        string second,third,fourth;
        second=dividedTextInstructions[index][1];
        third=dividedTextInstructions[index][2];
        fourth=dividedTextInstructions[index][3];
        u_int32_t rd,result=0,rs1,func3,opcode,imm;
        string opcodeString,rs1String,func3String,rdString,immString;
        u_int32_t temp;
        string Total,address,MCinstruction,ASMinstruction,comment;
        opcode=0b1100111;
        rd=register_to_int_mapping[second];
        rs1=register_to_int_mapping[third];
        if(fourth.substr(0,2)=="0x"){
            imm=stoul(fourth,nullptr,16);
        }
        else if(fourth.substr(0,2)=="0b"){
            imm=stoul(fourth,nullptr,2);
        }
        else{
            imm=stoul(fourth);
        }
        func3=0;
        result=opcode|(rd<<7)|(func3<<12)|(rs1<<15)|(imm<<20);
        MCinstruction="0x"+int_to_hex_string(result);
        ASMinstruction=first+" "+second+","+third+","+fourth;
        temp=4*index;
        address="0x"+int_to_hex_string(temp);
        opcodeString=int_to_binary_string(opcode,7);
        rdString=int_to_binary_string(rd,5);
        rs1String=int_to_binary_string(rs1,5);
        immString=int_to_binary_string(imm,12);
        func3String=int_to_binary_string(func3,3);
        cout<<"====================================="<<endl;
        cout<<"ASMinstruction:"<<ASMinstruction<<endl;
        cout<<"opcodeString:"<<opcodeString<<endl;
        cout<<"func3String:"<<func3String<<endl;
        cout<<"rdString:"<<rdString<<endl;
        cout<<"rs1String:"<<rs1String<<endl;
        cout<<"immString:"<<immString<<endl;
        cout<<"====================================="<<endl;
        Total=address+" "+MCinstruction+" , "+ASMinstruction+" # "+opcodeString+"-"+func3String
        +"-NULL-"+rdString+"-"+rs1String+"-NULL-"+immString;
        outputTextInstructions.emplace_back(Total);
    }
}

void Assembler::handleByteDirective(int index){
    int len=dividedDataInstructions[index].size();
    string address=dividedDataInstructions[index][0];
    u_int32_t updatedAddress=stoul(address);
    string output;
    for(int i=2;i<len;i++){
        address=int_to_hex_string(updatedAddress);
        output="0x"+address+" "+dividedDataInstructions[index][i];
        outputDataInstructions.emplace_back(output);
        updatedAddress+=4;
    }
}

void Assembler::handleHalfDirective(int index){
    int len=dividedDataInstructions[index].size();
    string address=dividedDataInstructions[index][0];
    u_int32_t updatedAddress=stoul(address);
    string output;
    string wordString;
    u_int16_t word;
    u_int8_t* parts=new u_int8_t[2];
    string* partStrings=new string[2];
    for(int i=2;i<len;i++){
        wordString=dividedDataInstructions[index][i];
        word=stoi(wordString);
        address=int_to_hex_string(updatedAddress);
        for(int j=0;j<2;j++){
            parts[j]=(word>>j*8)&0xFF;
            partStrings[j]=int_to_hex_string(parts[j]);
            output="0x"+address+" "+partStrings[j];
            outputDataInstructions.emplace_back(output);
            updatedAddress+=4;
            address=int_to_hex_string(updatedAddress);
        }
    }
}

void Assembler::handleWordDirective(int index){
    int len=dividedDataInstructions[index].size();
    string address=dividedDataInstructions[index][0];
    u_int32_t updatedAddress=stoul(address);
    string output;
    string wordString;
    u_int32_t word;
    u_int8_t* parts=new u_int8_t[4];
    string* partStrings=new string[4];
    for(int i=2;i<len;i++){
        wordString=dividedDataInstructions[index][i];
        word=stoi(wordString);
        address=int_to_hex_string(updatedAddress);
        for(int j=0;j<4;j++){
            parts[j]=(word>>j*8)&0xFF;
            partStrings[j]=int_to_hex_string(parts[j]);
            output="0x"+address+" "+partStrings[j];
            outputDataInstructions.emplace_back(output);
            updatedAddress+=4;
            address=int_to_hex_string(updatedAddress);
        }
    }
}

void Assembler::handleDwordDirective(int index){
    int len=dividedDataInstructions[index].size();
    string address=dividedDataInstructions[index][0];
    u_int32_t updatedAddress=stoul(address);
    string output;
    string wordString;
    u_int64_t word;
    u_int8_t* parts=new u_int8_t[8];
    string* partStrings=new string[8];
    for(int i=2;i<len;i++){
        wordString=dividedDataInstructions[index][i];
        word=stoi(wordString);
        address=int_to_hex_string(updatedAddress);
        for(int j=0;j<8;j++){
            parts[j]=(word>>j*8)&0xFF;
            partStrings[j]=int_to_hex_string(parts[j]);
            output="0x"+address+" "+partStrings[j];
            outputDataInstructions.emplace_back(output);
            updatedAddress+=4;
            address=int_to_hex_string(updatedAddress);
        }
    }
}

void Assembler::handleAsciizDirective(int index){
    int len=dividedDataInstructions[index].size();
    string address=dividedDataInstructions[index][0];
    u_int32_t updatedAddress=stoul(address);
    string output;
    string wordString;
    vector<string> hex_bytes;
    int len2;
    for(int i=2;i<len;i++){
        wordString=dividedDataInstructions[index][i];
        wordString.erase(remove(wordString.begin(), wordString.end(), '\"'), wordString.end()); //removes ""
        address=int_to_hex_string(updatedAddress);
        hex_bytes=string_to_hex_bytes(wordString);
        len2=hex_bytes.size();
        for(int j=0;j<len2;j++){
            output="0x"+address+" "+hex_bytes[j];
            outputDataInstructions.emplace_back(output);
            updatedAddress+=4;
            address=int_to_hex_string(updatedAddress);
        }
    }
}

void Assembler::makeOutputFile(string outputFilePath){
    ofstream file(outputFilePath);  
    for (string line : outputTextInstructions) {
        file << line << '\n';  
    }
    file<<"END"<<'\n';
    for (string line : outputDataInstructions) {
        file << line << '\n';  
    }
    file.close();  
    cout<<"file made at "<<outputFilePath<<endl;
}

void Assembler::assemble(string outputFilePath){
    convertALL();
    makeOutputFile(outputFilePath);
}
//correct .data as both address and data is in hex so less zeros will be there