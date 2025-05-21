#include<unordered_map>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<bitset>
#include<algorithm>

enum FormatType{
    Rformat,Iformat,Sformat,
    Bformat,Uformat,Jformat
};

enum Instruction{
    ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND,MUL,DIV,REM,
    ADDI,SLTI,SLTIU,XORI,ORI,ANDI,SLLI,SRLI,SRAI,LD,LB,LH,LW,LBU,LHU,
    SB,SH,SW,SD,
    BEQ,BNE,BGE,BLT,BLTU,BGEU,
    LUI,AUIPIC,
    JAL,JALR
};

enum ReadingMode{
    DATA,TEXT
};

enum currentDataOffestMode{
    OFF,BYTE,HALF,WORD,DWORD,ASCIZ
};

class Assembler{
public:
    std::string inputFilePath;
    std::vector<std::string> inputInstructions;
    std::vector<std::vector<std::string>> dividedDataInstructions;
    std::vector<std::vector<std::string>> dividedTextInstructions;
    std::vector<std::string> outputDataInstructions;
    std::vector<std::string> outputTextInstructions;
    std::string outputFilePath;
    std::unordered_map<std::string,u_int32_t> symbolTable;
    std::unordered_map<std::string,u_int32_t> register_to_int_mapping;
    void read_instructions();
    void convertALL();
    void convertRformatInstructions(int);
    void convertIformatInstructions(int);
    void convertSformatInstructions(int);
    void convertBformatInstructions(int);
    void convertUformatInstructions(int);
    void convertJformatInstructions(int);
    void handleByteDirective(int);
    void handleHalfDirective(int);
    void handleWordDirective(int);
    void handleDwordDirective(int);
    void handleAsciizDirective(int);
    void makeOutputFile(std::string);
    void assemble(std::string); //do all
    Assembler();
    Assembler(std::string);
};