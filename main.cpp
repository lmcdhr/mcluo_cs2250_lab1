//
//  main.cpp
//  mcluo_cs2250_lab1
//
//  Created by Mucheng Luo on 9/28/20.
//  Copyright © 2020 Mucheng Luo. All rights reserved.
//

#include "Header.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

// these are signals returned in pass one
// will show if there is parse errors. if there is not, return 1 which stands for pass one is complete
const int SYMBOLS_TOO_LARGE = -1;
const int USELIST_TOO_LARGE = -2;
const int DEFLIST_TOO_LARGE = -3;
const int INSTRUCTION_TOO_MANY = -4;
const int NUMBER_EXPECTED = -5;
const int SYM_EXPECTED = -6;
const int ADDR_EXPECTED = -7;
const int COMPLETE = 1;

// auxiliary functions

// return true if a vector contains a target string
bool contains(vector<string> vec, string target){
    for(auto v: vec){
        if(v == target){
            return true;
        }
    }
    return false;
}

// return errors strings to help the output
string getErrors(int error_index){
    switch(error_index){
        case -1: return "SYM_TOO_LONG";
        case -2: return "TOO_MANY_USE_IN_MODULE";
        case -3: return "TOO_MANY_DEF_IN_MODULE";
        case -4: return "TOO_MANY_INSTR";
        case -5: return "NUM_EXPECTED";
        case -6: return "SYM_EXPECTED";
        case -7: return "ADDR_EXPECTED";
        case 2: return "Error: This variable is multiple times defined; first value used";
            
    }
    return "";
}

// convert numbers into three digits string,
// used in the output of address map
string getThreeDigitString(int index){
    string res = to_string(index);
    int size = res.size();
    for(int i=0; i<3 - size; i++){
        res.insert(0, "0");
    }
    return res;
}

// find the module that defined symbol
// usde in check error 4 in pass two
int findDefinedModule(vector<Module> module_list, string symbol){
    for(Module module: module_list){
        if(contains(module.symbol_defined, symbol)){
            return module.number;
        }
    }
    return 0;
}

// the tokenizor function
// will read a symbol(except blank, \t and \n) one of a time
// will record the line number and offset that is reading
// will record the line number and offset of the last symbol has been read(used in error ouput)
// will record the number of token
string get_next_token(ifstream &readfile, int &line_number, int &line_offset, int& last_line_number, int &last_offset, int &last_length, int &token_count){
    char token_got;
    string final_token;
    token_got = readfile.get();
    while(token_got != EOF){
        line_offset++;
        token_count++;
        int token_size = final_token.size();
        // meet blank or tab, it can be the finish of the last token
        // if we have not met a token, we should continue
        if(token_got == ' ' || token_got == '\t'){
            if(token_size == 0){
                token_got = readfile.get();
            }else{
                return final_token;
            }
        }
        // meet \n, the same as last senario, return token or continue
        // we should also fresh the line number and offset
        else if(token_got == '\n'){
            last_length = line_offset;
            if(token_size == 0){
                line_offset = 1;
                line_number++;
                token_got = readfile.get();
            }else{
                line_offset = 1;
                line_number++;
                return final_token;
            }
        }
        // meet a symbol, it can be a start of a new token or the coninue of the last token
        else{
            if(token_size == 0){
                //record the start point of the token for output
                last_line_number = line_number;
                last_offset = line_offset - 1;
                final_token += token_got;
                token_got = readfile.get();
            }else{
                final_token += token_got;
                token_got = readfile.get();
            }
            
        }
    }
    
    // if there is no token to read anymore(which means after the loop, we got a blank)
    // we need to track back to get the line number and offset of the last token which is not blank because it has not been stored
    if(final_token == ""){
        // go back to the last line with tokens
        last_line_number = line_number - 1;
        // if we are at a new line, we also need to get the offset of last token by the length of last token we recorded
        if(line_offset == 1 && line_number - last_line_number >= 1 && token_count >= 1){
            last_offset = last_length - 1;
        }
    }
    return "";
}

// parse functions used in pass one

// parse the definition and check syntax errors
int getDefinitions(ifstream &readfile, string count_string, map<string, int> &symbol_map, map<string, vector<int> > &symbol_errors, int &line_number, int &line_offset, int& last_line_number, int &last_offset, int &instruction_count, Module &module, int &last_length, int &token_count){
    // number expected error, need a symbol stands for number of definitions
    if(count_string == ""){
        cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
        return NUMBER_EXPECTED;
    }
    // number expected error, the number of definition should be a number but not a string
    for(int i=0; i<count_string.size(); i++){
        if(!isdigit(count_string[i])){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
            return NUMBER_EXPECTED;
        }
    }
    
    int definition_count = stoi(count_string);
    
    // denifition too large error, definition number is limited to be less than 16
    if(definition_count > 16){
        cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(DEFLIST_TOO_LARGE) << endl;
        return DEFLIST_TOO_LARGE;
    }
    
    // get the symbol name and value
    for(int i=0; i<definition_count; i++){
        // get symbol name
        string symbol = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        // symbol expected error, we need the name of symbol
        if(symbol == ""){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(SYM_EXPECTED) << endl;
            return SYM_EXPECTED;
        }
        // symbol expected error, symbol shoud start with alpha
        if(!isalpha(symbol[0])){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(SYM_EXPECTED) << endl;
            return SYM_EXPECTED;
        }
        //symbol too large error, symbol size is limited to be less than 16
        if(symbol.size() > 16){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(SYMBOLS_TOO_LARGE) << endl;
            return SYMBOLS_TOO_LARGE;
        }

        // get symbol value
        string value_string = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        // number expected error, there should be a number stands for the value
        if(value_string == ""){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
            return NUMBER_EXPECTED;
        }
        // number expected error, value should be a number
        for(int j=0; j<value_string.size(); j++){
            if(!isdigit(value_string[j])){
                cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
                return NUMBER_EXPECTED;
            }
        }
        int value = stoi(value_string);
        
        // symbol already defined, record error 2
        if(symbol_map.find(symbol) != symbol_map.end()){
            symbol_errors[symbol].push_back(2);
        }else{
            symbol_map[symbol] = value + instruction_count;
            module.symbol_defined.push_back(symbol);
        }
    }
    
    return COMPLETE;
}

// parse the uselist and check syntax errors
int getUseList(ifstream &readfile, string count_string, int &line_number, int &line_offset,
               int& last_line_number, int &last_offset, int &last_length, int &token_count){
    // get number of uselist
    // number expected error, there should be a number stands for size of use list
    if(count_string == ""){
        cout << "Parse Error line "<<line_number<< " offset " << line_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
        return NUMBER_EXPECTED;
    }
    // number expected error, the size should be a number
    for(int i=0; i<count_string.size(); i++){
        if(!isdigit(count_string[i])){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
            return NUMBER_EXPECTED;
        }
    }
    
    int use_count = stoi(count_string);
    
    // use list too large error, the size should be less than 16
    if(use_count > 16){
        cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(USELIST_TOO_LARGE) << endl;
        return USELIST_TOO_LARGE;
    }
    
    // get uselist and add it into module
    for(int i=0; i<use_count; i++){
        string user = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        // symbol expected error, we need a user string
        if(user == ""){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(SYM_EXPECTED) << endl;
            return SYM_EXPECTED;
        }
        // symbol expected error, symbol should start with alpha
        if(!isalpha(user[0])){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(SYM_EXPECTED) << endl;
            return SYM_EXPECTED;
        }
        // symbol too large error, the size of symbol should be less than 16
        if(user.size() > 16){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(SYMBOLS_TOO_LARGE) << endl;
            return SYMBOLS_TOO_LARGE;
        }
    }
    
    return COMPLETE;
}

// parse the instruction and check syntax error
int getInstructions(ifstream &readfile, string count_string, Module &module, int &line_number, int &line_offset,
               int& last_line_number, int &last_offset, int &instruction_count, int &last_length, int &token_count){
    // set the base address of module
    module.address_base = instruction_count;
    // get the number of instructions
    // number expected error, there should the string of number
    if(count_string == ""){
        cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
        return NUMBER_EXPECTED;
    }
    //number expected error, the number of instructions should be numeric
    for(int i=0; i<count_string.size(); i++){
        if(!isdigit(count_string[i])){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
            return NUMBER_EXPECTED;
        }
    }
    
    int sub_instruction_count = stoi(count_string);
    // instruction too many error, instruction exceeds machine size(512)
    if(instruction_count + sub_instruction_count > 512){
        cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(INSTRUCTION_TOO_MANY) << endl;
        return INSTRUCTION_TOO_MANY;
    }
    
    // get instructions and op
    for(int i=0; i<sub_instruction_count; i++){
        string type = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        string code_string = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        // address expected error, address type should be one of these four types
        if(type != "A" && type != "E" && type != "R" && type != "I"){
            cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(ADDR_EXPECTED) << endl;
            return ADDR_EXPECTED;
        }
        // number expected error, we need a code string
        if(code_string == ""){
            cout << "Parse Error line "<<line_number<< " offset " << line_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
            return NUMBER_EXPECTED;
        }
        // number expected error, the op should be numeric
        for(int j=0; j<code_string.size(); j++){
            if(!isdigit(code_string[j])){
                cout << "Parse Error line "<<last_line_number<< " offset " << last_offset <<": "<<getErrors(NUMBER_EXPECTED) << endl;
                return NUMBER_EXPECTED;
            }
        }
    }
    
    // count the total number of instructions, which will be the size of all modules
    instruction_count += sub_instruction_count;
    return COMPLETE;
}

// print the symbol table got from pass one
// also check error 2 to make sure the symbol table can be used in pass two
void printSymbolTable(map<string, int> &symbol_map, map<string, vector<int> > symbol_errors, int instruction_count, vector<Module> module_list){
    // check error 2
    for(auto pair: symbol_map){
        string symbol = pair.first;
        int value = pair.second;
        if(value >= instruction_count){
            for(Module module: module_list){
                if(contains(module.symbol_defined, symbol)){
                    cout << "Warning: Module " <<module.number<<": "<<symbol<< " too big " <<value<< " (max="<<instruction_count-1<<") assume zero relative"<<endl;;
                    symbol_map[symbol] = 0;
                }
            }
        }
    }
    
    // start the output
    cout << "Symbol Table" << endl;
    for(auto pair: symbol_map){
        cout << pair.first << "=" << pair.second<<" ";
        if(symbol_errors.find(pair.first) != symbol_errors.end()){
            string symbol = pair.first;
            for(int error: symbol_errors[symbol]){
                cout << getErrors(error);
            }
        }
        cout << endl;
    }
}

// functions used in pass two

// get definition in pass two
// nothing to do with definition because it has been done in pass one
// will only make the ifstream move forward to skip the definition part
void getDefinitionSecondPass(ifstream &readfile, string definition_count_string,
                             int &line_number, int &line_offset, int& last_line_number, int &last_offset, int &last_length, int &token_count){
    // get number of uselist
    int definition_count = stoi(definition_count_string);
    
    // get definitions
    for(int i=0; i<definition_count; i++){
        string symbol = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        string value_string = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
    }
}

// get uselist in pas two
// will record the symbol used in this module for error check in the future
void getUseListSecondPass(ifstream &readfile, string use_count_string, Module &module,
                          int &line_number, int &line_offset, int& last_line_number, int &last_offset, int &last_length, int &token_count){
    // get number of uselist
    int use_count = stoi(use_count_string);
    
    // get uselist and add it into module
    for(int i=0; i<use_count; i++){
        string user = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        if(!contains(module.use_list, user)){
            module.use_list.push_back(user);
        }
    }
}

// parse instructions and store the record by new some instruction objects.
// all the instructions gotten will be stored in the instruction list of the the module they belong to
void getInstructionSecondPass(ifstream &readfile, string instruction_count_string, Module &module, map<string, int> &symbol_map, int instruction_count,
                              int &line_number, int &line_offset, int& last_line_number, int &last_offset, int &last_length, int &token_count){
    
    // get the start address of the module and the number of instructions
    int base = module.address_base;
    int sub_instruction_count = stoi(instruction_count_string);
        
    // get instructions
    for(int i=0; i<sub_instruction_count; i++){
        string type = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
        string code_string = get_next_token(readfile, line_number, line_offset, last_line_number, last_offset, last_length, token_count);
            
        
        // check op and error for Immediate address error
        int op = stoi(code_string);
        Instruction instruction(type, op);
        // op error, record error 11
        if(op > 9999 && type != "I"){
            instruction.has_op_error = true;
            op = 9999;
            instruction.opcode = 9;
            instruction.operand = 999;
            module.instruction_list.push_back(instruction);
            continue;
        }
        // immediate error, record error 10
        if(op > 9999 && type == "I"){
            instruction.immediate_address_too_large_error = true;
            op = 9999;
            instruction.operand = 999;
            instruction.opcode = 9;
            module.instruction_list.push_back(instruction);
            continue;
        }
        int opcode = op / 1000;
        int operand = op % 1000;
        instruction.opcode = opcode;
        instruction.operand = operand;

        if(type == "A"){
            // absolute address too big, error 8
            if(operand >= 512){
                instruction.absolute_address_too_large_error = true;
                instruction.operand = 0;
            }
        }
        else if(type == "E"){
            // external address too big, error 6
            if(operand >= module.use_list.size()){
                instruction.type = "I";
                instruction.external_too_large_error = true;
                module.instruction_list.push_back(instruction);
                continue;
            }
            // symbol used never defined, error 3
            string symbol = module.use_list[operand];
            if(symbol_map.find(symbol) == symbol_map.end()){
                instruction.use_symbol_without_definition_error = true;
                instruction.operand = 0;
                instruction.symbol_used_without_defined = symbol;
                module.instruction_list.push_back(instruction);
                module.symbol_used.push_back(symbol);
                continue;
            }
            // normal situation
            int value = symbol_map[symbol];
            instruction.operand = value;
            if(!contains(module.symbol_used, symbol)){
                module.symbol_used.push_back(symbol);
            }
        }
        else if(type == "R"){
            // relative address exceeds size of module, error 9
            if(base + operand > instruction_count){
                instruction.relative_address_too_large_error = true;
                instruction.operand = base;
            }else{
                instruction.operand = base + operand;
            }
        }
        module.instruction_list.push_back(instruction);
    }
    // symbol in use list but not used by any instruction, error 7
    for(string symbol_in_uselist: module.use_list){
        if(!contains(module.symbol_used, symbol_in_uselist)){
            module.symbol_in_uselist_but_not_used.push_back(symbol_in_uselist);
        }
    }
}

// output the address map in pass two
void outputAddressTable(vector<Module> &module_list, map<string, int> symbol_map){
    int index = 0;
    cout << endl;
    cout << "Memory Map" << endl;
    
    // check if errors in every instruction occures
    for(Module module: module_list){
        for(Instruction instruction: module.instruction_list){
            cout << getThreeDigitString(index) << ": " << to_string(instruction.opcode) << getThreeDigitString(instruction.operand) << " ";
            if(instruction.absolute_address_too_large_error == true){
                cout << "Error: Absolute address exceeds machine size; zero used";
            }
            if(instruction.external_too_large_error == true){
                cout << "Error: External address exceeds length of uselist; treated as immediate";
            }
            if(instruction.has_op_error == true){
                cout << "Error: Illegal opcode; treated as 9999";
            }
            if(instruction.immediate_address_too_large_error == true){
                cout << "Error: Illegal immediate value; treated as 9999";
            }
            if(instruction.relative_address_too_large_error == true){
                cout << "Error: Relative address exceeds module size; zero used";
            }
            if(instruction.use_symbol_without_definition_error == true){
                cout << "Error: "<<instruction.symbol_used_without_defined<<" is not defined; zero used";
            }
            cout << endl;
            index++;
        }

        // check if any symbol is in use list but has not been used by any instruction, warning 7
        if(module.symbol_in_uselist_but_not_used.size() != 0){
            for(string symbol: module.symbol_in_uselist_but_not_used){
                cout << "Warning: Module "<<module.number<<": "<<symbol<<" appeared in the uselist but was not actually used"<<endl;
            }
        }
    }
    
    // check if a defined symbol is never used, warning 4
    for(auto pair: symbol_map){
        string symbol = pair.first;
        bool found = false;
        for(Module module: module_list){
            if(contains(module.use_list, symbol)){
                found = true;
                break;
            }
        }
        if(found == false){
            int module_number = findDefinedModule(module_list, symbol);
            cout << "Warning: Module "<<module_number<<": "<<symbol<<" was defined but never used"<<endl;
        }
    }
    
}

// pass two
// will parse the file again and print the address map
void passTwo(string path, vector<Module> &module_list, map<string, int> &symbol_map, int instruction_count){
    ifstream readfile;
    readfile.open(path);
    string token;
    int line_number = 1;
    int line_offset = 1;
    int last_line_number = 1;
    int last_line_offset = 1;
    int last_length = 1;
    int token_count = 1;
    int module_index = 0;
    vector<string> symbol_defined;
    vector<Module> new_module_list;
    
    // some parameter is useless in the get_next_tokens for the second pass.
    // But for space saving, I use the read function again and transmit some useless parameters
    while((token = get_next_token(readfile, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count)) != ""){
        Module module = module_list[module_index];
        
        // get definitions into each module;
        // actually record nothing, but this helps us move the pointer of ifstream
        getDefinitionSecondPass(readfile, token, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        
        // get uselist into each module;
        token = get_next_token(readfile, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        getUseListSecondPass(readfile, token, module, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        
        // parse instructions
        token = get_next_token(readfile, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        getInstructionSecondPass(readfile, token, module, symbol_map, instruction_count ,
                                 line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        
        new_module_list.push_back(module);
        ++module_index;
    }
    
    // output result from instruction list of each module in module list
    outputAddressTable(new_module_list, symbol_map);
    
}

// pass one
// will check syntax errors and get the symbol table and module list if is there is no syntax error
// will push the process into pass two if there is no syntax error,
// if there is a syntax error, abort the whole process
void passOne(string path){
    
    int module_number = 0;
    int line_number = 1;
    int line_offset = 1;
    int last_line_number = 1;
    int last_line_offset = 1;
    int last_length = 1;
    int token_count = 1;
    int instruction_count = 0;
    int signal = 0;
    vector<Module> module_list;
    map<string, int> symbol_map;
    map<string, vector<int> > symbol_errors;
    string p = path;
    ifstream readfile;
    readfile.open(p);
    string token;
    while((token = get_next_token(readfile, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count)) != ""){
            
        // assemble modules
        Module module;
        ++module_number;
        module.number = module_number;
            
        // check definitions
        signal = getDefinitions(readfile, token, symbol_map, symbol_errors, line_number,
                                line_offset, last_line_number, last_line_offset, instruction_count, module, last_length, token_count);
        if(signal != COMPLETE){
            break;
        }
            
        // check uselist
        token = get_next_token(readfile, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        signal = getUseList(readfile, token, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        if(signal != COMPLETE){
            break;
        }
    
        // check instructions
        token = get_next_token(readfile, line_number, line_offset, last_line_number, last_line_offset, last_length, token_count);
        signal = getInstructions(readfile, token, module, line_number, line_offset, last_line_number, last_line_offset, instruction_count, last_length, token_count);
        if(signal != COMPLETE){
            break;
        }
        module_list.push_back(module);

    }
    
    // if the signal is COMPLETE, means there is no syntax error, go to pass two
    if(signal == COMPLETE){
        printSymbolTable(symbol_map, symbol_errors, instruction_count, module_list);
        passTwo(p, module_list, symbol_map, instruction_count);
    }
}

int main(int argc, const char * argv[]) {
    //string path = "/Users/muchengluo/Desktop/NYU/2250OS/Lab/Lab1/lab1/lab1/input_data/input-7";
    string path = argv[1];
    passOne(path);
    return 0;
}
