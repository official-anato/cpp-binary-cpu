/*
>> The Compiler for ANA32.
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

int main(int argc, char* argv[]){
  std::vector<std::uint8_t> output;
  std::ifstream file(argv[1], std::ios::in);
  if (!file.is_open()){
    std::cerr << "Error: Could not open file." << std::endl;
    return 1;
  }

  else{
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

    for (const auto& inst: lines){
      if (inst[0] == '!') continue;
      // std::cout << inst;
      // Extract the first token (opcode) by finding the space
      std::string opcode = inst.substr(0, 3);
      // output.push_back("0b" + std::bitset<8>(ISA.at(opcode)).to_string());
      output.push_back(ISA.at(opcode));
    }

  }

  // 2. Open the file in binary mode
  // ios::out = writing, ios::binary = don't treat data as text
  std::ofstream outFile(std::string(argv[1]) + ".bin", std::ios::out | std::ios::binary);

  if (outFile.is_open()) {
    // 3. Write the data
    // .data() gets the pointer to the start of the vector
    // .size() tells it how many bytes to write
    outFile.write(reinterpret_cast<const char*>(output.data()), output.size());

    // 4. Close the file
    outFile.close();
    std::cout << "File written successfully." << std::endl;
  }
  
  else {
  std::cerr << "Error: Could not open file for writing." << std::endl;
  return 1;
  }

  return 0;
}