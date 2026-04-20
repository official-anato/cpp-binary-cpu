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
  std::vector<uint8_t> RAM = std::vector<uint8_t>(65535, 0);
  // These two functions will be written later.
  // For now they will be boilerplate.
  // A blueprint if you will.
  void write(const uint8_t& start_addr, const std::vector<uint8_t>& data){
  }

  std::vector<uint8_t> read(const uint8_t& start_addr, const uint8_t& stop_addr){
    std::vector<uint8_t> data;
    return data;
  }
};

class reg{
  std::vector<uint8_t> registers;
  void write(const uint8_t* data){
  }

  std::vector<uint8_t> read(const uint8_t& start_addr, const uint8_t& stop_addr){
    std::vector<uint8_t> data;
    return data;
  }
};

class kernel{
  void read(const uint8_t& params){} // addr, data, byte count
  void user_input(const uint8_t& params){} // input src, writing address
  void filewrite(const uint8_t& params){} // filename, data
  void fileread(const uint8_t& params){} // filename, byte count
  std::map<std::string, std::function<void(uint8_t params)>> a;
};

class SDL_GRAPHICS{};

class CPU{
  private:
    memory RAM;
    reg Register;
    kernel Kernel;
  public:
    void run(const bool& logging, const std::vector<uint8_t> prg){
      // Code
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