/*
>> CURRENTLY WIP PROJECT : Expect bugs.
>> Anato's Virtual Machine in 32 bits!
>> Dubbed ANA32
>> Written by Anato.
*/

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <functional>

class memory{
  public:
    std::vector<uint8_t> RAM = std::vector<uint8_t>(65535, 0);
    void write(const bool& logging, const uint8_t& start_addr, const std::vector<uint8_t>& data){
      for (size_t i = 0; i < data.size(); i++){
        if (start_addr + i < RAM.size()){
          RAM.at(start_addr + i) = data[i];
        }
        else{
          throw std::runtime_error("RAM Writing Error: Location being written to is beyond 65,535.");
        }
      }
    }
    
    std::vector<uint8_t> read(const bool& logging, const uint8_t& start_addr, const uint8_t& stop_addr){
      std::vector<uint8_t> data;
      for (size_t i = 0; i < stop_addr; i++){
        data.push_back(RAM.at(start_addr + i));
      }
      return data;
    }
};

class reg{
  public:
    std::vector<uint8_t> registers;
    void write(const bool& logging, const uint8_t* data){
    }

    std::vector<uint8_t> read(const bool& logging, const uint8_t& start_addr, const uint8_t& stop_addr){
      std::vector<uint8_t> data;
      return data;
    }
};

class SDL_GRAPHICS{}; // Currently empty class; to be filled in later.

class CPU{
  private:
    // These functions will be called by interrupt(), and NOTHING else.
    void kernel_print(const bool& logging){} // addr, data, byte count
    void kerne_userinput(const bool& logging){} // input src, writing address
    void kerne_filewrite(const bool& logging){} // filename, data
    void kerne_fileread(const bool& logging){} // filename, byte count

    void ALUInternals_math(const bool& logging){}

    memory RAM;
    reg Register;
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    int PC = 0;

  public:
    /*
    void ALU(const bool& logging){}
    void jmp(const bool& logging){}
    void jeq(const bool& logging){}
    void jlt(const bool& logging){}
    void jgt(const bool& logging){}
    void cmp(const bool& logging){}
    void interrupt(const bool& logging){}
    void halt(const bool& logging){}
    void sdl_graphics(const bool& logging){}
    void ens(const bool& logging){}
    void mov(const bool& logging){}
    */
    void run(const bool& logging, const std::vector<uint8_t>& PRG){
      bool running = true;
      while (running && PC < (int)PRG.size()){
      }
    }
};

int main(){
  CPU computer;
  bool Logging = true;
  std::vector<uint8_t> PRG = {
  };
  
  computer.run(Logging, PRG);
  return 0;
}
