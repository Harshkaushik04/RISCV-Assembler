#include "../headers/asm_to_mc.hpp"
using namespace std;

int main(){
    string filepath="../testInputs/test1.asm";
    Assembler assembler=Assembler(filepath);
    assembler.makeInputInstructions();
    cout<<"input instructions:"<<endl;
    for(string instruction:assembler.inputInstructions){
        cout<<instruction<<endl;
    }
    cout<<"divided input instrcutions:"<<endl;
    for(pair<vector<string>,ReadingMode> pair1:assembler.dividedInputInstructions){
        for(string word:pair1.first){
            cout<<word<<endl;
        }
    }
    return 0;
}