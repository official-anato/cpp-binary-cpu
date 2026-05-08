/*
>> CURRENTLY WIP PROJECT : Expect bugs.
>> Anato's Virtual Machine in 32 bits!
>> Dubbed ANA32
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

    void write(const bool& logging, const size_t& address, const uint8_t& value){
      if (address < RAM.size()){
        RAM[address] = value;
      }
    }

    uint8_t read(const bool& logging, const size_t& address){
      if (address < RAM.size()){
        uint8_t value = RAM[address];
        return value;
      }

      else{
        throw std::runtime_error("Error: Attempted to read from an invalid RAM address.");
      }
    }
};

class Registers_Hardware{};

class SDL_GRAPHICS{
  RAM_Hardware VRAM = RAM_Hardware(720 * 720); // Draw_image() interprets each byte here and translates them to a pixel on a screen.
  // Screen resolution is 720x720.
  void write_to_VRAM(){}
  void read_from_VRAM(){}
  void draw_image(){}
}; // Currently empty class; to be filled in the future.

class CPU{
  private:
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    int PC = 0;
    RAM_Hardware RAM = RAM_Hardware(65535);
    Registers_Hardware Registers;
    void load_data_to_RAM(const bool& logging, const std::vector<uint8_t>& data){
      for (size_t i = 0; i < data.size(); i++){
        if (i < RAM.getSize()){
          RAM.write(logging, i, data[i]);
        }
      }
    }

    void kernel_print(const bool& logging, const uint32_t& message_location, const uint32_t& length, const uint32_t& mode){}
    void kernel_userinput(const bool& logging){} // input src, writing address
    void kernel_filewrite(const bool& logging){} // filename, data}
    void kernel_fileread(const bool& logging){} // filename, byte count
    void ALUInternals_math(const bool& logging){}
    void __UpdFlg(uint32_t res){
      if ((int)res < 0){Zero = false; Carry = false; Sign = true;}
      else if ((int)res > 0){Zero = false; Carry = (res > INT_MAX) ? true : false; Sign = false;}
      else if ((int)res == 0){Zero = true; Carry = false; Sign = false;}
    }

  public:
    void ALU(const bool& logging){}
    void jmp(const bool& logging){}
    void jeq(const bool& logging){}
    void jlt(const bool& logging){}
    void jgt(const bool& logging){}
    void cmp(const bool& logging){}
    void interrupt(const bool& logging, const uint32_t& MD, const uint32_t intcode){
      switch (intcode & 0xff){

        case 0b0:{ // Print
          uint32_t message_location; // Register 0
          uint32_t length; // Register 1
          uint32_t mode; // Register 2
          kernel_print(logging, message_location, length, mode);
          break;
        }

        default:{
          throw std::invalid_argument("Invalid Interrupt: This interrupt does not exist.");
          break;
        }
      }
    }
    void halt(const bool& logging){}
    void sdl_graphics(const bool& logging){}
    void ens(const bool& logging){}
    void mov(const bool& logging){}

    void run(const bool& logging, const std::vector<uint8_t>& PRG){
      load_data_to_RAM(logging, PRG);
      bool running = true;
      while ((running) && (PC < (int)RAM.getSize())){
        running = false;
      }
    }
};

int main(){
  CPU computer;
  bool logging = true;
  std::vector<uint8_t> PRG = {
    0b1
  };
  computer.run(logging, PRG);
  return 0;
}