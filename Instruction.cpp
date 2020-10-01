//
//  Instruction.cpp
//  mcluo_cs2250_lab1
//
//  Created by Mucheng Luo on 9/28/20.
//  Copyright © 2020 Mucheng Luo. All rights reserved.
//

#include "Header.h"

Instruction::Instruction(string type, int code){
    this->type = type;
    this->op = code;
    this->opcode = code/1000;
    this->operand = code%1000;
    this->has_op_error = false; //error 11
    this->use_symbol_without_definition_error = false;; //error 3
    this->external_too_large_error = false; //error 6
    this->absolute_address_too_large_error = false; //error 8
    this->relative_address_too_large_error = false; //error 9
    this->immediate_address_too_large_error = false;
}

