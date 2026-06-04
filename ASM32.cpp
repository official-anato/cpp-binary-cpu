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
using namespace std::string_literals;

struct opcode{
  uint8_t binary_number;
  int parameter_count = 0;
  int size_bytes = 1;

  opcode() = default;
  opcode(uint8_t binary, int para, int sbytes) : binary_number(binary), parameter_count(para), size_bytes(sbytes) {}
};

struct variable{
  std::string inst;
  int value = 0;
  int variable_regver = 6; // What register this variable is associated with. E.g the first initiated variable will ALWAYS be R6.

  variable() = default;
  variable(std::string instvar, int valuevar, int variable_regvervar) : inst(instvar), value(valuevar), variable_regver(variable_regvervar) {}
};

int main(int argc, char* argv[]){
  std::vector<std::uint8_t> output;
  int byte_counter = 0;
  int starting_variable_reg = 6;

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
    std::map<std::string, opcode> unified_opcodes = {
      {"HLT", opcode(0x0, 0, 1)},
      {"ADD", opcode(0x1, 3, 14)},
      {"SUB", opcode(0x2, 3, 14)},
      {"MUL", opcode(0x3, 3, 14)},
      {"DIV", opcode(0x4, 3, 14)},
      {"MOD", opcode(0x5, 3, 14)},
      {"JMP", opcode(0x6, 1, 6)},
      {"JEQ", opcode(0x7, 1, 6)},
      {"JLT", opcode(0x8, 1, 6)},
      {"JGT", opcode(0x9, 1, 6)},
      {"CMP", opcode(0xA, 2, 10)},
      {"SDL", opcode(0xB, 0, 0)},
      {"ENS", opcode(0xC, 0, 1)},
      {"MOV", opcode(0xD, 2, 10)},
      {"INT", opcode(0xE, 1, 6)}
    };

    std::map<std::string, int> label_list = {
      // Format:
      // {label_name, byte_count}
    };

    std::map<std::string, variable> variable_list = {
      // Specification:
      // Variables are stored in RAM where they are created.
      // This means that until I've figured out the implementation of dynamically changing the variables,
      // all variables are currently constants.

      // Format:
      // format WIP rn ;3
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

    // Get labels, variables, etc
    for (const auto& inst: lines){
      size_t spacepos = inst.find(" ");
      if (spacepos == std::string::npos && inst[0] == '_'){
        throw std::runtime_error("ASM32 : Error: Your variable " + inst.substr(1) + " has no value!");
      }

      if (inst[0] == '!') continue;

      if (inst[0] == '_' && inst[spacepos-1] == '_'){ // Check if a line is variable.
        int value = std::stoi(inst.substr(spacepos+1, inst.size()-spacepos-1));
        std::string MOV =
        // Formulate the string
        "MOV "s + // MOV Opcode
        std::to_string(value) + // User-defined value for variable
        std::string(" R") + std::to_string(starting_variable_reg); // Assigned register for this variable.

        variable_list[inst.substr(1, spacepos-2)] = variable(MOV, value, starting_variable_reg);
        starting_variable_reg++;
        // std::cout << variable_list.begin()->second.inst << std::endl;
        continue;
      }

      if (inst[0] == ';'){// Check if a line is a label
        if (inst[1] == ' '){ // Accounts for a space
          label_list[inst.substr(2)] = byte_counter;
          continue;
        }

        else { // No space handler
          label_list[inst.substr(1)] = byte_counter;
          continue;
        }
      }

      else{ // If all checks fail, it's an opcode.
        // Increment the byte_counter to progress through the program.
        byte_counter += unified_opcodes.at(inst.substr(0,3)).size_bytes;
      }
    }

    byte_counter = 0; // Reset byte counter after first pass.

    // Assembling section
    for (const auto& inst: lines){
      // These are for the first pass, ignore them.
      size_t spacepos = inst.find(" ");
      if (spacepos == std::string::npos){
        throw std::runtime_error("ASM32 : Error: Your variable " + inst.substr(1) + " has no value!");
      }
      if (inst[0] == '_' && inst[spacepos-1] == '_') continue;
      if (inst[0] == ';' || (inst[0] == ';' && inst[1] == ' ')) continue;
      if (inst[0] == '!') continue;

      // Extract the first opcode by finding the space
      std::string opcode = inst.substr(0, 3);
      byte_counter += unified_opcodes.at(opcode).size_bytes;

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
        size_t while_spacepos = line_copy.find(" ");
        if (while_spacepos != std::string::npos){
          if (while_spacepos > 0) {  // Avoid empty parameters
            parameters.push_back(line_copy.substr(0, while_spacepos));
          }

          line_copy.erase(0, while_spacepos + 1);  // Remove param and the space
        }

        else {
          if (!line_copy.empty()) {
            parameters.push_back(line_copy);  // Last parameter
          }

          break;
        }
      }

      // Check if opcode has less OR more parameters than it should.
      if (unified_opcodes.at(opcode).parameter_count > static_cast<int>(parameters.size())){
        throw std::runtime_error("ASM32 : opcode '" + opcode + "' is missing parameters!");
      }

      if (unified_opcodes.at(opcode).parameter_count < static_cast<int>(parameters.size())){
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
            //std::cout << "data = " << (LSB|MLSB<<8|MMSB<<16|MSB<<24) << std::endl;
            binary_parameters.push_back(LSB);
            binary_parameters.push_back(MLSB);
            binary_parameters.push_back(MMSB);
            binary_parameters.push_back(MSB);
          }

          else if(item[0] == ';'){ // Labels
            int data = label_list.at(item.substr(1));
            uint8_t LSB = (data) & 0xFF;
            uint8_t MLSB = (data >> 8) & 0xFF;
            uint8_t MMSB = (data >> 16) & 0xFF;
            uint8_t MSB = (data >> 24) & 0xFF;
            binary_parameters.push_back(LSB);
            binary_parameters.push_back(MLSB);
            binary_parameters.push_back(MMSB);
            binary_parameters.push_back(MSB);
          }

          else if (item[0] == '_' && item[item.size()-1] == '_'){ // Variables
            variable variable_data = variable_list.at(item.substr(1, item.size()-2));
            int variable_value = variable_data.value;
            uint8_t LSB = (variable_value) & 0xFF;
            uint8_t MLSB = (variable_value >> 8) & 0xFF;
            uint8_t MMSB = (variable_value >> 16) & 0xFF;
            uint8_t MSB = (variable_value >> 24) & 0xFF;
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
      std::string MD_value = ""s;
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
      output.push_back(unified_opcodes.at(opcode).binary_number);

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
    std::cout << "Byte counter = " << (int)byte_counter << std::endl;
    std::cout << "ASM32 : File written successfully." << std::endl;
  }

  else {
    std::cerr << "Error: Could not open file for writing." << std::endl;
    return 1;
  }

  return 0;
}