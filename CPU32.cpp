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
#include <climits>

// This function will be called everytime a log must be written.
std::vector<std::string> log_data;
void log(const bool& logging, const std::string& data){
  if (logging){
    log_data.push_back(data + "\n");
  }
}

void log_complete(const bool& logging){
  std::fstream file("vm-log.txt", std::fstream::app); // Define an fstream
  if ((file.is_open() & logging) == true){ // Open file
    for (const auto& item : log_data){
      file << item;
    }
    file.close(); // Close file
  }
};

class memory{
  public:
    std::vector<uint8_t> RAM = std::vector<uint8_t>(65535, 0);
    void write(const bool& logging, const uint32_t& start_addr, const std::vector<uint8_t>& data){
      for (size_t i = 0; i < data.size(); i++){
        if (start_addr + i < RAM.size()){
          log(logging, "");
          RAM.at(start_addr + i) = data[i];
          log(logging, "");
        }
        else{
          log(logging, "[RAM] : Violation detected, cannot writen to that address!");
          throw std::runtime_error("RAM Writing Error: Location being written to is beyond 65,535.");
        }
      }
    }

    void write32(const bool& logging, const uint32_t start_addr, const uint32_t data){ // Writes 1 word to RAM
      // Get the 4 bytes
      log(logging, "[RAM] : Writing MSB.");
      uint8_t MSB = (data >> 24) & 0b11111111;
      log(logging, "[RAM] : Written MSB successfully!");
      log(logging, "[RAM] : Value = " + (int)MSB);

      log(logging, "[RAM] : Writing mid MSB.");
      uint8_t mid_high_byte = (data >> 16) & 0b11111111;
      log(logging, "[RAM] : Written mid MSB successfully!");
      log(logging, "[RAM] : Value = " + (int)mid_high_byte);

      log(logging, "[RAM] : Writing mid LSB.");
      uint8_t mid_low_byte = (data >> 8) & 0b11111111;
      log(logging, "[RAM] : Written mid LSB successfully!");
      log(logging, "[RAM] : Value = " + (int)mid_low_byte);

      log(logging, "[RAM] : Writing LSB.");
      uint8_t LSB = (data) & 0b11111111;
      log(logging, "[RAM] : Written LSB successfully!");
      log(logging, "[RAM] : Value = " + (int)LSB);

      // Put them in a vector
      std::vector<uint8_t> word = {LSB, mid_low_byte, mid_high_byte, MSB};

      // Call the function.
      log(logging, "");
      write(logging, start_addr, word);
      log(logging, "");
    }
    
    std::vector<uint8_t> memory_read(const bool& logging, const uint32_t& start_addr, const uint32_t& stop_addr){
      std::vector<uint8_t> data;
      log(logging, "[RAM] : Retrieving all bytes...");
      for (size_t i = 0; i < stop_addr; i++){
        data.push_back(RAM.at(start_addr + i));
        log(logging, "[RAM] : Retrieved " + (int)RAM.at(start_addr + i));
      }
      log(logging, "[RAM] : Retrieved successfully!");
      return data;
    }

    std::vector<uint32_t> memory_read32(const bool& logging, const uint32_t& start_addr, const uint32_t& stop_addr){
      std::vector<uint32_t> data;
      log(logging, "[RAM] : Retrieving data");
      std::vector<uint8_t> data8 = memory_read(logging, start_addr*4, stop_addr*4);

      for(size_t i = 0; i < data8.size(); i++){
        uint8_t LSB = data8[0 + i];
        uint8_t mid_LSB = data8[1 + i];
        uint8_t mid_MSB = data8[2 + i];
        uint8_t MSB = data8[3 + i];
        uint32_t chunk32 = (LSB) | (mid_LSB << 8) | (mid_MSB << 16) | (MSB << 24);
        data.push_back(chunk32);
      }
      log(logging, "");
      return data;
    }
};

class reg{
  public:
    std::vector<uint8_t> registers;
    void write(const bool& logging, const uint8_t& data){
    }

    std::vector<uint8_t> read(const bool& logging, const uint8_t& start_addr, const uint8_t& stop_addr){
      std::vector<uint8_t> data;
      log(logging, "");
      // Code here
      log(logging, "");
      return data;
    }
};

class SDL_GRAPHICS{}; // Currently empty class; to be filled in later.

class CPU{
  private:
    // These functions will be called by interrupt(), and NOTHING else.
    // For now they will do nothing, but they will be written later.
    void kernel_print(const bool& logging){ // addr, data, byte count
      log(logging, "");
    }

    void kerne_userinput(const bool& logging){ // input src, writing address
      log(logging, "");
    } 

    void kerne_filewrite(const bool& logging){ // filename, data
      log(logging, "");
    } 

    void kerne_fileread(const bool& logging){ // filename, byte count
      log(logging, "");
    } 

    void ALUInternals_math(const bool& logging){
      log(logging, "");
    }

    void __UpdFlg(uint32_t res){
      if ((int)res < 0){Zero = false; Carry = false; Sign = true;}
      else if ((int)res > 0){Zero = false; Carry = (res > INT_MAX) ? true : false; Sign = false;}
      else if ((int)res == 0){Zero = true; Carry = false; Sign = false;}
    }
    
    memory RAM;
    reg Register;
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    int PC = 0;
  public:
    void ALU(const bool& logging){
      log_complete(logging);
    }

    void jmp(const bool& logging){
      log_complete(logging);
    }

    void jeq(const bool& logging){
      log_complete(logging);
    }

    void jlt(const bool& logging){
      log_complete(logging);
    }

    void jgt(const bool& logging){
      log_complete(logging);
    }

    void cmp(const bool& logging){
      log_complete(logging);
    }

    void interrupt(const bool& logging){}

    void halt(const bool& logging){
      log_complete(logging);
    }
    
    void sdl_graphics(const bool& logging){
      log_complete(logging);
    }

    void ens(const bool& logging){
      log_complete(logging);
    }

    void mov(const bool& logging){
      log_complete(logging);
    }
    void run(const bool& logging, const std::vector<uint8_t>& PRG){
      bool running = true;
      log(logging, "[OS] : ANA32 successfully booted! Awaiting instructions...");
      // For now, code will be hardcoded to test. Will add the FDE cycle later.
      log_complete(logging);
      while (running && PC < (int)PRG.size()){
      }
    }
};

int main(){
  // Delete previous contents of vm-log.txt on boot.
  // std::fstream file("vm-log.txt", std::fstream::out);
  // file << "";

  CPU computer;
  bool logging = true;
  log(logging, "[BIOS] : Initializing instructions...");
  std::vector<uint8_t> PRG = {
  }; 
  log(logging, "[BIOS] : Initialization successful!");
  
  log(logging, "[BIOS] : Sending instructions...");
  computer.run(logging, PRG);
  return 0;
}