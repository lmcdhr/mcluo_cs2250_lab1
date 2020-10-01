#  Lab 1 Operating System: Linker

#### NYU CSCI 2250
#### Author: Mucheng Luo (ml6389)



## 1. pattern of project

1. Main objective

this project is going to accomplish a linker, which will parse the input file using two passes and output the symbol table, the address map as well as the error messages

2. logic graph of project

![avatar](https://s1.ax1x.com/2020/10/01/0K0Goq.png)



## 2. details about functions

### 1. Module.cpp

1. main objective

this is a class for Module. The object comes from this class will store information of a module, including errors, symbol defined, used and the instructions.

2. functions and definitions
    1. int number: the # of module in a input
    2. int address_base: the base address of a module
    3. vector<string> use_list: the uselist of a module
    4. vector<string> symbol_used: the symbol used in the instructions
    5. vector<string> symbol_defined: symbols defined in the definition part
    6. vector<Instruction> instruction_list: the instruction list of the module
    7. vector<string> symbol_in_uselist_but_not_used: store symbols in the uselist but noe used by any instruction
    
### 2. Instrucion.cpp

1. main objective

this is a class for Instructions. The object comes from this class will store information of an instruction, including the errors made,  address type, address and the symbol related.

2. functions and definitions
    1. string type: the address type
    2. int op: the op
    3. int opcode: the opcode
    4. int operand: the operand
    5. string symbol_used_without_defined: symbol used for error 3
    6. bool has_op_error: error 11
    7. bool use_symbol_without_definition_error: error 3
    8. bool external_too_large_error: error 6
    9. bool absolute_address_too_large_error: error 8
    10. bool relative_address_too_large_error: error 9
    11. bool immediate_address_too_large_error: error 10

### 3. main.cpp

1. main objective

the main body of the project, will process two passes and output the result

2. functions and definitions(please see the annotation in the codes for more details)
    1. auxiliary functions:
      
        1. contains
        
        2. getErrors
        3. getThreeDigitString
        4. findDefiniedModule
        
    2. tokenizor: get_next_token
    
    3. function in pass one:
    
        1. getDinitions
        2. getUseList
        3. getInstructions
        4. printSymbolTable
        5. passOne
    
    4. functions in pass two:
    
        1. getDefinitionSecondPass
        2. getUseListSecondPass
        3. getInstructionSecondPass
        4. outputAddressTable
        5. passTwo
    
    5. main function: main



## 3. how to run

1. unzip the .zip file

   ```shell
   unzip mcluo_cs2250_lab1.zip
   ```

2. make the file

   ```
   make
   ```

3. run the main(please use your own path)

   ```
   ./main home/sample2/input-3
   ```

4. clean the make

   ```
   make clean
   ```

   



## 4. output samples

1. make and run a single sample

![avatar](https://s1.ax1x.com/2020/10/01/0KsPht.png)

2. run the check script

![avatar](https://s1.ax1x.com/2020/10/01/0KsAc8.png)
