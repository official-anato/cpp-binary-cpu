/*
>> CURRENTLY WIP PROJECT : Expect bugs.
>> Anato's Virtual Machine in 32 bits!
>> Dubbed "ANA32"
>> Written by Anato.

>> This is the 2nd edition of ANA32. The goal for this edition is to
 >> refactor the code to handle the issues seen with the first edition
 >> such as bloated code or the feeling that some functions were written poorly.
*/

#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <climits>
#include <filesystem>
namespace fs = std::filesystem;

class RAM_Hardware {
  private:
    std::vector<uint8_t> RAM;

  public:
    RAM_Hardware(size_t size_bytes) : RAM(size_bytes, 0) {}
    size_t getSize() const {
      return RAM.size();
    }

    void write(const bool logging, const size_t& address, const uint8_t& value){
      if (address < RAM.size()){
        RAM[address] = value;
      }
    }

    uint8_t read(const bool logging, const size_t& address){
      if (address < RAM.size()){
        uint8_t value = RAM[address];
        return value;
      }

      else{
        throw std::runtime_error("Error: Attempted to read from an invalid RAM address.");
      }
    }
};

class Registers_Hardware{
  private:
    std::vector<uint8_t> Registers;

  public:
    Registers_Hardware() : Registers(32, 0) {}
    size_t getSize() const {
      return Registers.size();
    }

    void write(const bool logging, const size_t& address, const uint8_t& value){
      if (address < Registers.size()){
        Registers[address] = value;
      }
    }

    uint8_t read(const bool logging, const size_t& address){
      if (address < Registers.size()){
        uint8_t value = Registers[address];
        return value;
      }

      else{
        throw std::runtime_error("Error: Attempted to read from an invalid Register address.");
      }
    }
};

class SDL_GRAPHICS{
  private:
    RAM_Hardware VRAM = RAM_Hardware(720 * 720); // Draw_image() interprets each byte here and translates them to a pixel on a screen.
  
  public:
    // Screen resolution is 720x720.
    void init_sys(){}
    void write_to_VRAM(){}
    void read_from_VRAM(){}
    void draw_image(){}
}; // Currently empty class; to be filled in the future.

class CPU{
  private:
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    bool sdl_running = false;
    int PC = 0;
    RAM_Hardware RAM = RAM_Hardware(65535);
    Registers_Hardware Registers;
    SDL_GRAPHICS Graphics;

    // --- Helpers --- //
    uint8_t _fetch8(const bool logging){
      uint8_t val = RAM.read(logging, PC);
      PC++;
      return val;
    }

    uint16_t _fetch16(const bool logging){
      uint8_t val1 = _fetch8(logging);
      uint8_t val2 = _fetch8(logging);
      PC += 2;
      return (val1 << 8) | val2;
    }

    size_t _fetch24(const bool logging){
      uint8_t val1 = _fetch8(logging);
      uint8_t val2 = _fetch8(logging);
      uint8_t val3 = _fetch8(logging);
      PC += 3;
      return (val1 << 16) | (val2 << 8) | val3;
    }

    uint32_t _fetch32(const bool logging){
      uint8_t val1 = _fetch8(logging);
      uint8_t val2 = _fetch8(logging);
      uint8_t val3 = _fetch8(logging);
      uint8_t val4 = _fetch8(logging);
      PC += 4;
      return (val1 << 24) | (val2 << 16) | (val3 << 8) | val4;
    }

    void _0b11exception(std::string func_name){}

    // Internal functions //
    void load_data_to_RAM(const bool logging, const std::vector<uint8_t>& data){
      for (size_t i = 0; i < data.size(); i++){
        if (i < RAM.getSize()){
          RAM.write(logging, i, data[i]);
        }
      }
    }

    void kernel_print(const bool logging, const uint32_t& message_location, const uint32_t& length, const uint32_t& mode){}
    void kernel_userinput(const bool logging){} // input src, writing address
    void kernel_filewrite(const bool logging){} // filename, data}
    void kernel_fileread(const bool logging){} // filename, byte count
    void ALUInternals_math(const bool logging){}
    void __UpdFlg(uint32_t res){
      if ((int)res < 0){Zero = false; Carry = false; Sign = true;}
      else if ((int)res > 0){Zero = false; Carry = (res > INT_MAX) ? true : false; Sign = false;}
      else if ((int)res == 0){Zero = true; Carry = false; Sign = false;}
    }

  public:
    void ALU(const bool logging){}
    void jmp(const bool logging){}
    void jeq(const bool logging){}
    void jlt(const bool logging){}
    void jgt(const bool logging){}
    void cmp(const bool logging){}
    void interrupt(const bool logging, const uint32_t MD, const uint32_t intcode){
      switch (intcode & 0xff){

        case 0b0:{ // Print
          uint32_t message_location = Registers.read(logging, 0); // Register 0
          uint32_t length = Registers.read(logging, 1); // Register 1
          uint32_t mode = Registers.read(logging, 2); // Register 2
          kernel_print(logging, message_location, length, mode);
          break;
        }

        case 0b1: { // User Input
          uint32_t userinput = Registers.read(logging, 0); // Input source - R0
          uint32_t address = Registers.read(logging, 1); // Writing address - R1
          break;
        }

        case 0b10: { // File Write
          uint32_t filename_address = Registers.read(logging, 0); // Filename address - R0
          uint32_t filename_length = Registers.read(logging, 1); // Filename length - R1
          uint32_t data_address = Registers.read(logging, 2); // Data address - R2
          uint32_t data_length = Registers.read(logging, 3); // Data length - R3
          break;
        }

        case 0b11: { // File Read
          uint32_t filename_address = Registers.read(logging, 0); // Filename address - R0
          uint32_t filename_length = Registers.read(logging, 1); // Filename length - R1
          uint32_t data_address = Registers.read(logging, 2); // Data address - R2
          uint32_t data_length = Registers.read(logging, 3); // Data length - R3
          break;
        }

        default:{
          throw std::invalid_argument("Invalid Interrupt: This interrupt does not exist.");
          break;
        }
      }
    }
    void halt(const bool logging){}
    void sdl_system(const bool logging, const uint32_t intcode){
      if (!sdl_running){
        Graphics.init_sys();
        sdl_running = true;
        sdl_system(logging, intcode); // Re-run the function to execute the command after initializing SDL.
      }

      else{
        switch (intcode & 0xff){
          case 0b0:{ // Write to VRAM
            break;
          }
          
          case 0b01:{ // Read from VRAM
            break;
          }

          case 0b10:{ // Draw image
            break;
          }

          case 0b11:{ // Quit SDL
            break;
          }

          default:{
            throw std::invalid_argument("Invalid SDL Code: This code does not exist.");
            break;
          }
      }
      }
    }
    void ens(const bool logging){}
    void mov(const bool logging){}

    void run(const bool logging, const std::vector<uint8_t>& PRG){
      load_data_to_RAM(logging, PRG);
      bool running = true;
      while ((running) && (PC < (int)RAM.getSize())){
        uint8_t opcode = RAM.read(logging, PC);
        PC++;
        switch(opcode){
          case 0b0:{
            //log(logging, "HLT", {});
            //halt();
            running = false;
            break;
          }
            
          case 0b1: { // Add
            //log(logging, "ADD", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            //ALU(MD, A, B, R, 0b00);
            break;
          }
          
          case 0b10:{ // Sub
            //log(logging, "SUB", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            //ALU(MD, A, B, R, 0b01);
            break;
          }
          
          case 0b11:{ // Mul
            //log(logging, "MUL", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            //ALU(MD, A, B, R, 0b10);
            break;
          }
          
          case 0b100:{ // Div
            //log(logging, "DIV", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            //ALU(MD, A, B, R, 0b11);
            break;
          }
          
          case 0b101:{ // Mod
            //log(logging, "MOD", {std::to_string(MD), std::to_string(A), std::to_string(B), std::to_string(R)});
            //ALU(MD, A, B, R, 0b100);
            break;
          }
          
          // Here, MD has 2 uses.
          // The first 2 bits are either 01 and 10
          // referring to register or address,
          // the next 2 bits are 00 and 01
          // 00 being a relative jump;
          // and 01 being absolute.
          case 0b110:{
            //log(logging, "JMP", {std::to_string(MD), std::to_string(value)});
            //jmp(MD, value);
            break;
          }
          
          case 0b111:{
            //log(logging, "JEQ", {std::to_string(MD), std::to_string(value)});
            //jeq(MD, value);
            break;
          }
          
          case 0b1000:{
            //log(logging, "JLT", {std::to_string(MD), std::to_string(value)});
            //jlt(MD, value);
            break;
          }
          
          case 0b1001:{
            //log(logging, "JGT", {std::to_string(MD), std::to_string(value)});
            //jgt(MD, value);
            break;
          }
          
          case 0b1010:{
            //log(logging, "CMP", {std::to_string(MD), std::to_string(A), std::to_string(B)});
            //cmp(MD, A, B);
            break;
          }
          
          case 0b1011:{
            //log(logging, "SDL", {});
            //sdl_system(logging, intcode); // This command will function like interrupt(), but will communicate with SDL instead.
            break;
          }
          
          case 0b1100:{
            //log(logging, "ENS", {});
            //ens();
            break;
          }
          
          case 0b1101:{
            //log(logging, "MOV", {std::to_string(MD), std::to_string(A), std::to_string(B)});
            //mov(MD, A, B);
            break;
          }

          case 0b1110:{
            //log(logging, "INT", {std::to_string(MD), std::to_string(value)});
            //interrupt(MD, value);
            break;
          }

          default:{
            std::cout << "Invalid Opcode!" << std::endl;
             break;
          }
        }
        running = false;
      }
    }
};

int main(){
  CPU computer;
  bool logging = true;
  std::vector<uint8_t> PRG = {
    0b0
  };
  computer.run(logging, PRG);
  return 0;
}