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
#include <limits>
#include <sstream>

int main(int argc, char* argv[]){
  std::vector<std::uint8_t> output;

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <source_file>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1], std::ios::in);
  if (!file.is_open()){
    std::cerr << "Error: Could not open file." << std::endl;
    return 1;
  }

  else{
    std::map<std::string, uint8_t> ISA = {
      {"HLT", 0x0},
      {"ADD", 0x1},
      {"SUB", 0x2},
      {"MUL", 0x3},
      {"DIV", 0x4},
      {"MOD", 0x5},
      {"JMP", 0x6},
      {"JEQ", 0x7},
      {"JLT", 0x8},
      {"JGT", 0x9},
      {"CMP", 0xA},
      {"SDL", 0xB},
      {"ENS", 0xC},
      {"MOV", 0xD},
      {"INT", 0xE}
    };

    std::map<std::string, int> INSTSIZE = {
      {"HLT", 0},
      {"ADD", 3},
      {"SUB", 3},
      {"MUL", 3},
      {"DIV", 3},
      {"MOD", 3},
      {"JMP", 1},
      {"JEQ", 1},
      {"JLT", 1},
      {"JGT", 1},
      {"CMP", 2},
      {"SDL", 0}, // <-- Subject to change.
      {"ENS", 0},
      {"MOV", 2},
      {"INT", 1}
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

    // Assembling section
    for (const auto& inst: lines){
      if (inst[0] == '!') continue;
      // Extract the first opcode by finding the space
      std::string opcode = inst.substr(0, 3);

      // With opcodes done, parameters are next.
      std::vector<std::string> parameters;
      std::string line_copy = inst;
      line_copy.erase(0, 4); // Remove the opcode.

      // Remove all comments.
      // Find all mentions of a comment, and get rid of them.
      size_t commentpos = line_copy.find('!');
      if (commentpos != std::string::npos) {
        line_copy.erase(commentpos, line_copy.size());
      }

      // Separate the parameters
      while (!line_copy.empty()){
        size_t spacepos = line_copy.find(" ");
        if (spacepos != std::string::npos){
          if (spacepos > 0) {  // Avoid empty parameters
            parameters.push_back(line_copy.substr(0, spacepos));
          }

          line_copy.erase(0, spacepos + 1);  // Remove param and the space
        }

        else {
          if (!line_copy.empty()) {
            parameters.push_back(line_copy);  // Last parameter
          }

          break;
        }
      }

      // Check if opcode has less OR more parameters than it should.
      if (INSTSIZE.at(opcode) > parameters.size()){
        throw std::runtime_error("ASM32 : opcode '" + opcode + "' is missing parameters!");
      }

      if (INSTSIZE.at(opcode) < parameters.size()){ 
        throw std::runtime_error("ASM32 : opcode '" + opcode + "' has more parameters than it should!");
      }

      std::vector<uint8_t> binary_parameters;
      for (std::string item : parameters){
        if (parameters.size() > 0){
          if (item[0] == 'R' || item[0] == 'r' || item[0] == '@'){
            int data = stoi(item.substr(1));
            uint8_t LSB = (data) & 0xFF;
            uint8_t MLSB = (data >> 8) & 0xFF;
            uint8_t MMSB = (data >> 16) & 0xFF;
            uint8_t MSB = (data >> 24) & 0xFF;
            binary_parameters.push_back(LSB);
            binary_parameters.push_back(MLSB);
            binary_parameters.push_back(MMSB);
            binary_parameters.push_back(MSB);
          }
          else{
            int data = stoi(item);
            uint8_t LSB = (data) & 0xFF;
            uint8_t MLSB = (data >> 8) & 0xFF;
            uint8_t MMSB = (data >> 16) & 0xFF;
            uint8_t MSB = (data >> 24) & 0xFF;
            binary_parameters.push_back(LSB);
            binary_parameters.push_back(MLSB);
            binary_parameters.push_back(MMSB);
            binary_parameters.push_back(MSB);
          }
        }
      }

      uint8_t MD = 0;
      std::string MD_value = "";
      bool MD_available = false;
      if (parameters.size() > 0){
        MD_available = true;
        MD_value += "0b";
        std::vector<std::string> REV_parameters(parameters.rbegin(), parameters.rend());
        for (std::string item : REV_parameters){
          if (item[0] == 'R' || item[0] == 'r'){
            MD_value += "01";
          }

          else if (item[0] == '@'){
            MD_value += "10";
          }

          else{
            MD_value += "00";
          }
        }
        MD = stoi(MD_value, nullptr, 2);
      }

      // Push to output. DO NOT CHANGE ORDERING.
      output.push_back(ISA.at(opcode));

      if (MD_available){
        output.push_back(MD);
      }
      
      for (uint8_t item : binary_parameters){
        output.push_back(item);
      }
    }
  }

  // 2. Open the file in binary mode
  std::ofstream outFile(std::string(argv[1]) + ".bin", std::ios::out | std::ios::binary);

  if (outFile.is_open()) {
    // 3. Write the data
    outFile.write(reinterpret_cast<const char*>(output.data()), output.size());

    // 4. Close the file
    outFile.close();
    std::cout << "ASM32 : File written successfully." << std::endl;
  }
  
  else {
    std::cerr << "Error: Could not open file for writing." << std::endl;
    return 1;
  }

  return 0;
}