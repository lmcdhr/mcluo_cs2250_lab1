//
//  Header.h
//  mcluo_cs2250_lab1
//
//  Created by Mucheng Luo on 9/28/20.
//  Copyright © 2020 Mucheng Luo. All rights reserved.
//

#include<string>
#include<iostream>
#include<vector>
using namespace std;

// for the convinience, all the variables are designed as public.

class Instruction{
private:
public:
    string type;
    int op;
    int opcode;
    int operand;
    string symbol_used_without_defined; // error 3
    bool has_op_error; //error 11
    bool use_symbol_without_definition_error; //error 3
    bool external_too_large_error; //error 6
    bool absolute_address_too_large_error; //error 8
    bool relative_address_too_large_error; //error 9
    bool immediate_address_too_large_error; //error 10
    
    
    Instruction(string type, int code);
};

class Module{
private:
public:
    int number;
    int address_base;
    vector<string> use_list;
    vector<string> symbol_used;
    vector<string> symbol_defined;
    vector<Instruction> instruction_list;
    vector<string> symbol_in_uselist_but_not_used;// error 7
    
    Module();
};
