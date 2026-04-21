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
    // These two functions will be written later.
    // For now they will be boilerplate.
    // A blueprint if you will.
    void write(const uint8_t& start_addr, const std::vector<uint8_t>& data){
      int stop_byte = data.size();
      int current_byte = 0;
      while (current_byte < stop_byte){
        RAM.insert(RAM.begin() + start_addr + current_byte, data.begin(), data.end());
        current_byte++;
      }
    }
    
    std::vector<uint8_t> read(const uint8_t& start_addr, const uint8_t& stop_addr){

      std::vector<uint8_t> data;
      return data;
    }
};

class reg{
  public:
    std::vector<uint8_t> registers;
    void write(const uint8_t* data){
    }

    std::vector<uint8_t> read(const uint8_t& start_addr, const uint8_t& stop_addr){
      std::vector<uint8_t> data;
      return data;
    }
};

class SDL_GRAPHICS{}; // Currently empty class to be filled in later.

class CPU{
  private:
    void kernel_print(){} // addr, data, byte count
    void kerne_userinput(){} // input src, writing address
    void kerne_filewrite(){} // filename, data
    void kerne_fileread(){} // filename, byte count

    memory RAM;
    reg Register;
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    int PC = 0;

  public:
    void add(){}
    void sub(){}
    void mul(){}
    void div(){}
    void mod(){}
    void jmp(){}
    void jeq(){}
    void jlt(){}
    void jgt(){}
    void cmp(){}
    void interrupt(){}
    void halt(){}
    void sdl_graphics(){}
    void ens(){}
    void mov(){}
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