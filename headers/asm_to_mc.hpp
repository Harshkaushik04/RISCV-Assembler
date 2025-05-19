#include<unordered_map>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>

enum FormatType{
    Rformat,Iformat,Sformat,
    Bformat,Uformat,Jformat
};

enum Instruction{
    ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND,MUL,DIC,REM,
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

struct MCinstruction{
    u_int32_t opcode;
    u_int32_t func3;
    u_int32_t func7;
    u_int32_t rd;
    u_int32_t rs1;
    u_int32_t rs2;
    u_int32_t immediate; 
};



class Assembler{
public:
    std::string inputFilePath;
    std::vector<std::string> inputInstructions;
    std::vector<std::vector<std::string>> dividedDataInstructions;
    std::vector<std::vector<std::string>> dividedTextInstructions;
    std::vector<std::string> outputInstructions;
    std::vector<MCinstruction> outputMCinstructions;
    std::string outputFilePath;
    std::unordered_map<std::string,u_int32_t> symbolTable;
    void makeInputInstructions();
    void makeSymbolTable();
    void convertRformatInstructions();
    void convertIformatInstructions();
    void convertSformatInstructions();
    void convertBformatInstructions();
    void convertUformatInstructions();
    void convertJformatInstructions();
    void convertALL();
    void makeOutputFile();
    void assemble(); //do all
    Assembler();
    Assembler(std::string);
};