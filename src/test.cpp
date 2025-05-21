#include "../headers/asm_to_mc.hpp"
using namespace std;

int main(){
    string filepath="../testInputs/test3.asm";
    Assembler assembler=Assembler(filepath);
    assembler.read_instructions();
    cout<<"input instructions:"<<endl;
    for(string instruction:assembler.inputInstructions){
        cout<<instruction<<endl;
    }
    cout<<"divided data instructions:"<<endl;
    for(vector<string> list:assembler.dividedDataInstructions){
        for(string word:list){
            cout<<word<<endl;
        }
        cout<<"====================="<<endl;
    }
    cout<<"divided text instructions:"<<endl;
    for(vector<string> list:assembler.dividedTextInstructions){
        for(string word:list){
            cout<<word<<endl;
        }
        cout<<"===================="<<endl;
    }
    cout<<"symbol table:"<<endl;
    for(pair<string,u_int32_t> new_pair:assembler.symbolTable){
        cout<<new_pair.first<<":"<<"0x"<<std::hex<<new_pair.second<<endl;
    }
    cout<<assembler.dividedDataInstructions.size()<<endl;
    cout<<assembler.dividedTextInstructions.size()<<endl;
    assembler.assemble("../testOutputs/test3.mc");
    cout<<"output text instructions:"<<endl;
    for(string line:assembler.outputTextInstructions){
        cout<<line<<endl;
    }
    return 0;
}