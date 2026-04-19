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
#include <bitset>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " <source_file> <output_option> [output_file]" << std::endl;
    std::cout << "Output options: 1 for terminal, 2 for file" << std::endl;
    return 1;
  }
  std::vector<std::string> output;

  // Ignore argv[0]
  // argv[1] is the source code filename
  // argv[2] is 1 or 2, if 1 it outputs the compiled code into the terminal. If it's 2, it outputs a file.
  // argv[3] is if argv[2] is 2, the assembly in a file named by the user.
  
  // Process 1: Read the source code, compile it, and store the output to a vector.
  std::fstream file(argv[1], std::fstream::in);
  if (file.is_open()){
    std::map<std::string, uint8_t> ISA = {
      {"HLT", 0b00000000},
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
      {"MOV", 0b00001101},
      {"INT", 0b00001110}
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
      if (inst[0] == '!') continue;
      // std::cout << inst;
      // Extract the first token (opcode) by finding the space
      std::string opcode = inst.substr(0, 3);
      output.push_back("0b" + std::bitset<8>(ISA.at(opcode)).to_string());
      
      // With opcodes done, parameters are next.
      // Remove the opcode.
      // Each line, remove all commas.
      // Remove all comments.
      // Separate the parameters, convert to binary, and push_back() to output.
      std::vector<std::string> parameters;
      std::string line_copy = inst;
      line_copy.erase(0, 4);
      size_t commentpos = line_copy.find('!');
      if (commentpos != std::string::npos) {
        line_copy.erase(commentpos, line_copy.size());
      }

      size_t commapos = line_copy.find(',');
      if (commapos != std::string::npos) {
        parameters.push_back(line_copy.substr(0, commapos));
        line_copy.erase(0, commapos + 1);
        parameters.push_back(line_copy);
      }

      for (std::string item : parameters){
        output.push_back(item);
      }
      
    }
  }
  
  else{
    std::cout << "File not found!" << std::endl;
  }

  // Process 2: Output the compiled code.
  
  // Process 2.1: Output to terminal if argv[2] is 1.
  if (*argv[2] == '1'){
   for (const auto& line : output){
     std::cout << line << ", ";
    }
  }

  // Process 2.2: Output to a file if argv[2] is 2. 
  else if (*argv[2] == '2'){
    std::fstream file(std::string(argv[3]), std::fstream::out);
    if (file.is_open()){
      for (const auto& line : output){
        file << line << ", ";
       }
       file.close();
    }

    else{
      std::cout << "Error creating file!" << std::endl;
    }
  }
  
  return 0;
}