/*
>> CURRENTLY WIP PROJECT
>> A simplified custom assembly language
>> Written by Anato.
*/

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

class Cpu{
  private:
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    int PC = 0;
    std::uint32_t reg[32] = {0};
    std::vector<uint8_t> RAM = std::vector<uint8_t>(65535, 0);
    
  public:
    bool save_output = false;
    std::string save_filename = "halt_finish.txt";
    
    // Underscore signifies helper functions.
    uint8_t _fetch(const std::vector<uint8_t>& PRG){
      uint8_t data = PRG.at(PC);
      PC++;
      return data;
    }
    
    void _0b11exception(std::string func_name){
      throw std::invalid_argument("0b00000011 is not accepted as a parameter for " + func_name);
    }
    
    void log(const bool& logging, const std::string& opcode, const std::vector<std::string>& params){
      if (!logging) return;
      else{
       std::fstream file("vm-log.txt", std::ios::app);
       if (file.is_open()){
          file << "[" << PC << "]" << ": ";
          file << "Executing opcode ";
         file << opcode;
         file << " with values {";
          for (const auto& str : params){file << str << ", ";}
          file << "}\n";
         }
      }
    }

    uint8_t _get_value(uint8_t MD, uint8_t A, const std::string& func_name){
      switch (MD){
        case 0b00:{
          return A;
          break;
        }
        
        case 0b01:{
          return reg[(int)A];
          break;
        }
        
        case 0b10:{
          return RAM[(int)A];
          break;
        }
        
        default:{
          _0b11exception(func_name);
          break;
        }
      }
      return 0;
    }
    
    void _writeram(uint8_t R, uint8_t value){
      RAM[R] = value;
      // std::cout << RAM[R] << std::endl; // Debug code. Comment later.
    }
    
    void _writeregister(uint8_t R, uint8_t value){
      reg[R] = value;
      // std::cout << reg[R] << std::endl; // Debug code. Comment later.
    }
    
    void _writedata(uint8_t loc, uint8_t MD3, uint8_t value){
      switch (MD3){
        case 0b00:{
          throw std::invalid_argument("Invalid Parameter : Immediate cannot be used for Result parameter.");
          break;
        }
        
        case 0b01:{
          _writeregister(loc, value);
          break;
        }
        
        case 0b10:{
          _writeram(loc, value);
          break;
        }
        
        default:{
          _0b11exception("'Add'");
          break;
        }
      }
    }
   
   void __UpdFlg(uint8_t res){
    if ((int)res < 0){Zero = false; Carry = false; Sign = true;}
    else if ((int)res > 0){Zero = false; Carry = (res > 4294967295U) ? true : false; Sign = false;}
    else if ((int)res == 0){Zero = true; Carry = false; Sign = false;}
   }
   
   void _UpdFlg(uint8_t MD, uint8_t R){
     if (MD == 0b01){
       __UpdFlg(reg[R]);
     }
     
     else if (MD == 0b10){
       __UpdFlg(RAM[R]);
     }
     
     else{
       throw std::invalid_argument("Flag Error: Flag update failure. Parameter is not register or address.");
     }
   }
    
    uint8_t _math(uint8_t A, uint8_t B, uint8_t operation){
      switch(operation){
        case 0b00:{
          return A+B;
          break;
        }
        
        case 0b01:{
          return A-B;
          break;
        }
        
        case 0b10:{
          return A*B;
          break;
        }
        
        case 0b11:{
          return A/B;
          break;
        }
        
        case 0b100:{
          return A%B;
          break;
        }
    
        default:{
          throw std::invalid_argument("Syntax Error: Invalid mathematical operation!");
          break;
        }
      }
    }
    
    void ALU(uint8_t MD, uint8_t A, uint8_t B, uint8_t R, uint8_t arithmetic){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'A'");
      uint8_t Y = _get_value(MD2, B, "'B'");
      _writedata(R, MD3, _math(X, Y, arithmetic));
      _UpdFlg(MD3, R);
    }
    
    void jmp(uint8_t MD, uint8_t value){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t relabs = (MD >> 2) & 0b11;
      switch(relabs){
        case 0b00:{
          PC += _get_value(MD1, value, "'A'");
          break;
        }
        
        case 0b01:{
          PC = _get_value(MD1, value, "'A'");
          break;
        }
      }
    }
    
    void jeq(uint8_t MD, uint8_t value){
      if (Zero == true){
        jmp(MD, value);
      }
    }
    
    void jlt(uint8_t MD, uint8_t value){
      if (Sign == true){
        jmp(MD, value);
      }
    }
    
    void jgt(uint8_t MD, uint8_t value){
      if (Sign == false && Zero == false){
        jmp(MD, value);
      }
    }
    
    void cmp(const uint8_t& MD, const uint8_t& A, const uint8_t& B){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'A'");
      uint8_t Y = _get_value(MD2, B, "'B'");
      uint8_t res = X-Y;
      _UpdFlg(MD3, res);
    }
    
    void sdl(){
      std::cout << "This command currently does nothing and exists only to make the compile happy. It's described features as per the documentation will be implemented in the future." << std::endl;
    }
    
    void halt(){
      if (save_output){
        // Declare file
        std::fstream file(save_filename, std::ios::app);
        
        if (file.is_open()){
          // Formatting for the file output
          file  << "RAM (0 - 65535): [";
          
          // Iterate through items in RAM and append them one by one.
          for (const auto& str : RAM){file << str << ", ";}
          file << "]\nPC: " << PC << "\nRegisters (R0 - R31): [";
          
          // Do the same for registers as well
          for (const auto& str : reg){file << str << ", ";}
          file << "]";
          file.close();
        }
        
        std::cout << "Program has finished. Saving data to " << save_filename << std::endl;
        std::exit(EXIT_SUCCESS);
        }
        
        else{
          // std::cout << "Program has finished." << std::endl; // Debug code. Enable only if you suspect your code shouldn't be stopping to see why.
          std::exit(EXIT_SUCCESS);
        }
    }
    
    void ens(){
      save_output = true;
    }
    
    void mov(const uint8_t& MD, const uint8_t& A, const uint8_t& B){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t source_value = _get_value(MD1, A, "'A'");
      switch(MD2){
        case 0b00:{
          throw std::invalid_argument("Invalid Parameter : Immediate cannot be used for target parameter.");
          break;
        }
        
        case 0b01:{
          _writeregister(B, source_value);
          break;
        }
        
        case 0b10:{
          _writeram(B, source_value);
          break;
        }
        
        default:{
          _0b11exception("'Mov'");
          break;
        }
      }
    }

    void interrupt(const uint8_t MD, const uint8_t& value){
      switch (value){
        case 0b0:{ // Read syscall. AKA print()
          uint8_t message_location = reg[0];
          uint8_t length = reg[1];
          uint8_t mode = reg[2];
          // if mode is 0, print as char.
          // If 1, print as string. If the string is "2" then it must print "2", not the ASCII value "2".
          // If 2, print as int.
          uint8_t return_PC = PC;
          PC = message_location;
          for (int i = 0; i < length; i++){
            switch (mode){
              case 0b00:{
                std::cout << (char)RAM.at(PC);
                break;
              }
              
              case 0b01:{
                std::cout << RAM.at(PC);
                break;
              }
              
              case 0b10:{
                std::cout << (int)RAM.at(PC);
                break;
              }
            }
            PC++;
          }

          PC = return_PC;
          
          break;
        }
        
        case 0b01:{ // GDI Syscall. AKA input()
          // Code comes later. Too complex.
          break;
        }
        
        default:{
          throw std::invalid_argument("Invalid Interrupt: This interrupt does not exist.");
          break;
        }
      }

    }
    
    void run(const bool& logging, const std::vector<uint8_t>& PRG){
      bool running = true;
      while (running && PC < (int)PRG.size()){
        uint8_t opcode = PRG.at(PC);
        PC++;
        switch(opcode){
          case 0b0:
            log(logging, "HLT", {});
            halt();
            running = false;
            break;
            
          case 0b1: { // Add
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            log(logging, "ADD", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            ALU(MD, A, B, R, 0b00);
            break;
          }
          
          case 0b10:{ // Sub
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            log(logging, "SUB", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            ALU(MD, A, B, R, 0b01);
            break;
          }
          
          case 0b11:{ // Mul
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            log(logging, "MUL", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            ALU(MD, A, B, R, 0b10);
            break;
          }
          
          case 0b100:{ // Div
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            log(logging, "DIV", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            ALU(MD, A, B, R, 0b11);
            break;
          }
          
          case 0b101:{ // Mod
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            log(logging, "MOD", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            ALU(MD, A, B, R, 0b100);
            break;
          }
          
          // Here, MD has 2 uses.
          // The first 2 bits are either 01 and 10
          // referring to register or address,
          // the next 2 bits are 00 and 01
          // 00 being a relative jump;
          // and 01 being absolute.
          case 0b110:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            log(logging, "JMP", {std::to_string(MD), std::to_string(value)});
            jmp(MD, value);
            break;
          }
          
          case 0b111:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            log(logging, "JEQ", {std::to_string(MD), std::to_string(value)});
            jeq(MD, value);
            break;
          }
          
          case 0b1000:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            log(logging, "JLT", {std::to_string(MD), std::to_string(value)});
            jlt(MD, value);
            break;
          }
          
          case 0b1001:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            log(logging, "JGT", {std::to_string(MD), std::to_string(value)});
            jgt(MD, value);
            break;
          }
          
          case 0b1010:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            log(logging, "CMP", {std::to_string(MD), std::to_string(A), std::to_string(B)});
            cmp(MD, A, B);
            break;
          }
          
          case 0b1011:{
            log(logging, "SDL", {});
            //sdl();
            break;
          }
          
          case 0b1100:{
            log(logging, "ENS", {});
            ens();
            break;
          }
          
          case 0b1101:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            log(logging, "MOV", {std::to_string(MD), std::to_string(A), std::to_string(B)});
            mov(MD, A, B);
            break;
          }

          case 0b1110:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            log(logging, "INT", {std::to_string(MD), std::to_string(value)});
            interrupt(MD, value);
            break;
          }
          
          default: {
            std::cout << "Invalid Opcode!" << std::endl;
             break;
          }
        }
      }
    }
    
};

int main(){
  Cpu computer;
  bool Logging = true;
  std::vector<uint8_t> PRG = {
  };
  
  computer.run(Logging, PRG);
  return 0;
}