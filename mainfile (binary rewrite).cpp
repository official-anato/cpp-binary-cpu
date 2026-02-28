/*
CURRENTLY WIP PROJECT
A simplified custom assembly language (Binary Rewrite edition)
Written by Anato.
*/

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

/*
-- DOCS --
Instructions:
+ NOTE: All mentions of R(x) can be interchanged with immediate values as well.
+ NOTE: MD refers to a single-byte parameter. It allows you to tell the CPU what the next 2 parameters' type are. Refer to the MD section for more information.
* Arithmetic *
0b00000001 MD A B R : Add R1, R2, R3 : R1 = X, R2 = Y, R3 = result

0b00000010 MD A B R : Sub R1, R2, R3 : R1 = X, R2 = Y, R3 = result

0b00000011 MD A B R : Mul R1, R2, R3 : R1 = X, R2 = Y, R3 = result

0b00000100 MD A B R : Div R1, R2, R3 : R1 = X, R2 = Y, R3 = result

0b00000101 MD A B R : Mod R1, R2, R3 : R1 = X, R2 = Y, R3 = result

* Branching *
0b00000110 MD A : Jmp R1 : R1 = destination
0b00000111 MD A : Jeq R1 : R1 = destination
0b00001000 MD A : Jlt R1 : R1 = destination
0b00001001 MD A : Jgt R1 : R1 = destination

* Comparison *
0b00001010 MD A B : Cmp R1, R2 : R1 = X, R2 = Y

* Machine control *
0b00001011 MD A : Gdi R1 : R1 = register/ram address. Takes user input, places them in R1.
0b00001100 : Sdl : enables SDL2 system. (Note: REFER TO RAM LOCATIONS FOR CONFIGS.)
0b00001101 : Ens : Enables output to a save file.
0b00001110 : Hlt : Halts the CPU.
0b00001111 MD A B : Mmc R1, R2 : R1 = address, R2 = value. Modifies RAM directly.
0b00010000 MD A : Read R1 : R1 = address
0b00010001 MD A B : Mov R1, R2 : R1 = Value, R2 = target (ram address or register address)

* Registers and flags *
- Flags -
ZF : Zero flag - true or false, determines whether last operation resulted in zero or not.
CF : Carry flag - true or false, determines if the last operation has become larger than 65535
SF : Sign Flag - true or false, determines if the last operstion resulted in a negative or positive number. True for Negative, False for Positive.

- Registers -
R0 to R31 are available for usage. This totals up to 32 registers, BUT NOTE THAT R32 IS NOT AVAILABLE.

* MD Section *
As mentioned, MD is a single-byte parameter that comes after the instruction byte.

00 : imm
01 : reg
10 : adr

Example: 0b01 0b010101 0b01 0b01 0b10
Explanation: The first byte is 0b01, which is the 'add' instruction. The second byte is the MD byte. 01 is register, so, 010101 says that the next three parameters are all referring to the register. Next is 0b01, which according to the MD byte is register 1, and so pulls data from that. The one after that is also 0b01. And finally, 0b10 is register 2, which is where we will store the data.

RAM Locations:
* SDL2 *
SDL2 has it's own dedicated section of RAM initialized, called VRAM. VRAM has 518,000 addresses available, 720x720. 1 pixel per address.

*/

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
    }
    
    void _writeram(uint8_t R, uint8_t value){
      RAM[R] = value;
      std::cout << RAM[R] << std::endl;
    }
    
    void _writeregister(uint8_t R, uint8_t value){
      reg[R] = value;
      std::cout << reg[R] << std::endl;
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
   
   void _updflag(uint8_t res){
    if ((int)res < 0){Zero = false; Carry = false; Sign = true;}
    else if ((int)res > 0){Zero = false; Carry = (result > 4294967295U) ? true : false; Sign = false;}
    else if ((int)res == 0){Zero = true; Carry = false; Sign = false;}
   }
    
    void add(const uint8_t& MD, const uint8_t& A, const uint8_t& B, const uint8_t& R){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'B'");
      uint8_t Y = _get_value(MD2, B, "'A'");
      _writedata(R, MD3, X+Y);
    }
    
    void sub(const uint8_t& MD, const uint8_t& A, const uint8_t& B, const uint8_t& R){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'B'");
      uint8_t Y = _get_value(MD2, B, "'A'");
      _writedata(R, MD3, X-Y);
    }
    
    void mul(const uint8_t& MD, const uint8_t& A, const uint8_t& B, const uint8_t& R){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'B'");
      uint8_t Y = _get_value(MD2, B, "'A'");
      _writedata(R, MD3, X*Y);
    }
    
    void div(const uint8_t& MD, const uint8_t& A, const uint8_t& B, const uint8_t& R){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'B'");
      uint8_t Y = _get_value(MD2, B, "'A'");
      _writedata(R, MD3, X/Y);
    }
    
    void mod(const uint8_t& MD, const uint8_t& A, const uint8_t& B, const uint8_t& R){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'B'");
      uint8_t Y = _get_value(MD2, B, "'A'");
      _writedata(R, MD3, X%Y);
    }
    
    void jmp(){
    }
    
    void jeq(){
    }
    
    void jlt(){
    }
    
    void jgt(){
    }
    
    void cmp(const uint8_t& MD, const uint8_t& A, const uint8_t& B){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint8_t X = _get_value(MD1, A, "'B'");
      uint8_t Y = _get_value(MD2, B, "'A'");
      uint8_t res = X-Y;
      _updflag(res);
    }
    
    void halt(){
    }
    
    void run(const std::vector<uint8_t>& PRG){
      bool running = true;
      while (running){
        uint8_t opcode = PRG.at(PC);
        PC++;
        switch(opcode){
          case 0b0:
            halt();
            running = false;
            break;
            
          case 0b1: {
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            
            add(MD, A, B, R);
            break;
          }
          
          case 0b10:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            sub(MD, A, B, R);
            break;
          }
          
          case 0b11:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            mul(MD, A, B, R);
            break;
          }
          
          case 0b100:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            div(MD, A, B, R);
            break;
          }
          
          case 0b101:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            mod(MD, A, B, R);
            break;
          }
          
          case 0b110:{
            uint8_t MD = _fetch();
            jmp(MD);
            break;
          }
          
          case 0b111:{
            uint8_t MD = _fetch();
            jeq(MD);
            break;
          }
          
          case 0b1000:{
            uint8_t MD = _fetch();
            jlt(MD);
            break;
          }
          
          case 0b1001:{
            uint8_t MD = _fetch();
            jgt(MD);
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
  std::vector<uint8_t> PRG = {
    0b1, 0b010101, 0b01, 0b01, 0b10,
    0b0
  };
  computer.run(PRG);
  return 0;
}