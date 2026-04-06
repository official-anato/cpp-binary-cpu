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
0b00001111 MD A : Read R1 : R1 = address
0b00010000 MD A B : Mov R1, R2 : R1 = Value, R2 = target (ram address or register address)

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
SDL2 has it's own dedicated section of RAM initialized, called VRAM. VRAM has 518kb. Per address, the most significant byte determines whether or not the pixel is enabled. This system is bitmap only.

The next 6 bits are RGB. A color is assigned per 2 bits. The most significant bit is whether or not it is that color, and the least significant bit is whether or not it is dark or light.

The final bit, when on, halves the brightness intensity of the pixel.

e.g 10000000 = Pixel is enabled. All RGB values are 0, meaning it is black.
e.g 11000000 = Pixel is enabled. It is light red.

---

TODO: (top is easiest, bottom is hardest.)
- Implement try/catch handling for errors.
- Replace all instances of implicit typecasting to be static_cast<>.
- Create a 'log mode' parameter (writes to a log file every instruction)
- Implement the saving and loading of programs to an external file.
- Update opcodes to support the 1 byte limit
- Change uint8_t to uint32_t (aka switching to a 32 bit system.)
- Add debug options. (Stopping execution at a certain PC value, slowing the execution rate to 1 instruction per second, etc)
- Write an assembler.
- Add support for syscalls (e.g calling 'print'.)
- Deprecate GDI, make a syscall version.
- Add support for external syscalls (User-made syscalls, aka libraries)
- Get SDL2 support working.
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
    
    void gdi(){
      std::cout << "This command currently does nothing and exists only to make the compile happy. It's described features as per the documentation will be implemented in the future." << std::endl;
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
    
    void read(const uint8_t& MD, const uint8_t& value){
      uint8_t MD1 = MD & 0b11;
      std::cout << _get_value(MD1, value, "'read value'");
    }
    
    void mov(const uint8_t& MD, const uint8_t& value){
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
            
          case 0b1: { // Add
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            ALU(MD, A, B, R, 0b00);
            break;
          }
          
          case 0b10:{ // Sub
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            ALU(MD, A, B, R, 0b01);
            break;
          }
          
          case 0b11:{ // Mul
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            ALU(MD, A, B, R, 0b10);
            break;
          }
          
          case 0b100:{ // Div
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
            ALU(MD, A, B, R, 0b11);
            break;
          }
          
          case 0b101:{ // Mod
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            uint8_t R = _fetch(PRG);
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
            jmp(MD, value);
            break;
          }
          
          case 0b111:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            jeq(MD, value);
            break;
          }
          
          case 0b1000:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            jlt(MD, value);
            break;
          }
          
          case 0b1001:{
            uint8_t MD = _fetch(PRG);
            uint8_t value = _fetch(PRG);
            jgt(MD, value);
            break;
          }
          
          case 0b1010:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            uint8_t B = _fetch(PRG);
            cmp(MD, A, B);
            break;
          }
          
          case 0b1011:{
            //gdi();
            break;
          }
          
          case 0b1100:{
            //sdl();
            break;
          }
          
          case 0b1101:{
            ens();
            break;
          }
          
          case 0b1110:{
            uint8_t MD = _fetch(PRG);
            uint8_t A = _fetch(PRG);
            read(MD, A);
            break;
          }
          
          case 0b1111:{
            mov();
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
  computer.ens();
  std::vector<uint8_t> PRG = {
    0b1110, 0b00, 0b01000001,
    0b0
  }; // This program tests the 'read()' assembly instruction by printing the ASCII character 'A'.
  
  computer.run(PRG);
  return 0;
}