/*
>> Compiler for Anato's custom assembly language.
>> Written by Anato.
*/

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <sstream>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " <source_file> <output_option> [output_file]" << std::endl;
    std::cout << "Output options: 1 for terminal, 2 for file" << std::endl;
    return 1;
  }
  std::vector<std::string> output;

  /* THIS IS REFERENCE CODE. DO NOT TOUCH.
  std::cout << "Program Name: " << argv[0] << std::endl;
  std::cout << "Number of arguments: " << argc - 1 << std::endl;
  for (int i = 1; i < argc; ++i) {
    std::cout << "Argument " << i << ": " << argv[i] << std::endl;
  }
  return 0;
  */

  // Ignore argv[0]
  // argv[1] is the source code filename
  // argv[2] is 1 or 2, if 1 it outputs the compiled code into the terminal. If it's 2, it outputs a file.
  // argv[3] is if argv[2] is 2, the assembly in a file named by the user.
  
  // Process 1: Read the source code, compile it, and store the output to a vector.
  std::fstream file(argv[1], std::fstream::in);
  if (file.is_open()){
    std::map<std::string, uint8_t> ISA = {
      {"ADD", 0b00000001},
      {"SUB", 0b00000010},
      {"MUL", 0b00000011},
      {"DIV", 0b00000100},
      {"MOD", 0b00000101},
      {"JMP", 0b00000110},
      {"JEQ", 0b00000111},
      {"JLT", 0b00001000},
      {"JGT", 0b00001001},
      {"CMP", 0b00001010},
      {"SDL", 0b00001011},
      {"ENS", 0b00001100},
      {"HLT", 0b00001101},
      {"READ", 0b00001110},
      {"MOV", 0b00001111}
   };
  
    std::stringstream buffer;
    buffer << file.rdbuf();
    buffer.seekg(0);

    // Separate each line of code by a newline and store it in a vector.
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(buffer, line)) {
      if (!line.empty()) {
        lines.push_back(line);
      }
    }
    
    // Compiling section
    for (const auto& inst: lines){
      if (inst[0] == '+') continue;
      std::cout << inst;
      // Extract the first token (opcode) by finding the space
      size_t spacePos = inst.find(' ');
      std::string opcode = (spacePos == std::string::npos) ? inst : inst.substr(0, spacePos);
      std::cout << ISA.at(opcode) << std::endl;
    }
  
    // This should be last.
    // This is when process 1 finishes compiling and sends the output to the output vector.
    for (const auto& line : lines){output.push_back(line);}
  }

  else{
    std::cout << "File not found!" << std::endl;
  }
  

  // Process 2: Output the compiled code.
  
  // Process 2.1: Output to terminal if argv[2] is 1.
  if (*argv[2] == '1'){
   for (const auto& line : output){
     std::cout << line << std::endl;
    }
  }

  // Process 2.2: Output to a file if argv[2] is 2. 
  else if (*argv[2] == '2'){
    std::fstream file(std::string(argv[3]), std::fstream::out);
    if (file.is_open()){
      for (const auto& line : output){
        file << line << std::endl;
       }
       file.close();
    }

    else{
      std::cout << "Error creating file!" << std::endl;
    }
  }
  
  return 0;
}