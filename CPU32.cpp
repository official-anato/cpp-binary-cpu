/*
>> CURRENTLY WIP PROJECT : Expect bugs.
>> Anato's Virtual Machine in 32 bits!
>> Dubbed ANA32
>> Written by Anato.
*/

#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <map>
#include <functional>
#include <climits>
#include <filesystem>
namespace fs = std::filesystem;

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

void log_clear(const bool& logging){
  if ((!fs::exists("vm-log.txt")) && logging){
    std::fstream file("vm-log.txt", std::fstream::out);
    file << "";
  }
};


class memory{
  public:
    std::array<uint8_t, 65535> RAM;
    void write(const bool& logging, const uint32_t& start_addr, const std::vector<uint8_t>& data){
      for (size_t i = 0; i < data.size(); i++){
        if (start_addr + i < RAM.size()){
          log(logging, "[RAM] : Writing to RAM address " + std::to_string(start_addr + i));
          RAM.at(start_addr + i) = data[i];
          log(logging, "[RAM] : Data = " + std::to_string(data[i]));
          log(logging, "[RAM] : Written data successfully!");
        }
        else{
          log(logging, "[RAM] : Violation detected, cannot writen to that address!");
          throw std::runtime_error("RAM Writing Error: Location being written to is beyond 65,535.");
        }
      }
    }

    void write32(const bool& logging, const uint32_t start_addr, const uint32_t data){ // Writes 1 word to RAM
      // Get the 4 bytes
      log(logging, "[RAM] : Writing LSB.");
      uint8_t LSB = (data) & 0xFF;
      log(logging, "[RAM] : Written LSB successfully!");
      log(logging, "[RAM] : Value = " + std::to_string(LSB));

      log(logging, "[RAM] : Writing mid LSB.");
      uint8_t mid_low_byte = (data >> 8) & 0xFF;
      log(logging, "[RAM] : Written mid LSB successfully!");
      log(logging, "[RAM] : Value = " + std::to_string(mid_low_byte));

      log(logging, "[RAM] : Writing mid MSB.");
      uint8_t mid_high_byte = (data >> 16) & 0xFF;
      log(logging, "[RAM] : Written mid MSB successfully!");
      log(logging, "[RAM] : Value = " + std::to_string(mid_high_byte));
      
      log(logging, "[RAM] : Writing MSB.");
      uint8_t MSB = (data >> 24) & 0xFF;
      log(logging, "[RAM] : Written MSB successfully!");
      log(logging, "[RAM] : Value = " + std::to_string(MSB));

      // Put them in a vector
      std::vector<uint8_t> word = {LSB, mid_low_byte, mid_high_byte, MSB};

      // Call the function.
      log(logging, "[RAM] : Writing 4 byte word to RAM...");
      write(logging, start_addr, word);
      log(logging, "[RAM] : Writing finished.");
    }
    
    std::vector<uint8_t> memory_read(const bool& logging, const uint32_t& start_addr, const uint32_t& stop_addr){
      std::vector<uint8_t> data;
      log(logging, "[RAM] : Retrieving all bytes...");
      if (start_addr > RAM.size()){
        for (size_t i = 0; i < stop_addr; i++){
          data.push_back(RAM.at(start_addr + i));
          log(logging, "[RAM] : Retrieved " + std::to_string(RAM.at(start_addr + i)));
        }
        log(logging, "[RAM] : Retrieved successfully!");
        return data;
      };
    }

    std::vector<uint32_t> memory_read32(const bool& logging, const uint32_t& start_addr, const uint32_t& stop_addr){
      std::vector<uint32_t> data;
      log(logging, "[RAM] : Retrieving data");
      std::vector<uint8_t> data8 = memory_read(logging, start_addr*4, stop_addr*4);

      for(size_t i = 0; i < data8.size(); i += 4){
        log(logging, "[RAM] : Retrieving LSB...");
        uint8_t LSB = data8[0 + i];
        log(logging, "[RAM] : Retrieved LSB successfully!: \n[RAM] : Value = " + std::to_string(LSB));

        log(logging, "[RAM] : Retrieving mid LSB...");
        uint8_t mid_LSB = data8[1 + i];
        log(logging, "[RAM] : Retrieved mid LSB successfully!: \n[RAM] : Value = " + std::to_string(mid_LSB));

        log(logging, "[RAM] : Retrieving mid MSB...");
        uint8_t mid_MSB = data8[2 + i];
        log(logging, "[RAM] : Retrieved mid MSB successfully!: \n[RAM] : Value = " + std::to_string(mid_MSB));

        log(logging, "[RAM] : Retrieving MSB...");
        uint8_t MSB = data8[3 + i];
        log(logging, "[RAM] : Retrieved MSB successfully!: \n[RAM] : Value = " + std::to_string(MSB));

        uint32_t chunk32 = (LSB) | (mid_LSB << 8) | (mid_MSB << 16) | (MSB << 24);
        data.push_back(chunk32);
      }
      log(logging, "");
      return data;
    }
};

class reg{
  public:
    std::array<uint32_t, 32> registers{};
    // Arrays can be changed using `.at()` like vectors.
    void write(const bool& logging, const uint8_t addr, const std::vector<uint8_t>& data){
      log(logging, "[REG] : Writing to register address " + std::to_string(addr));
      if (addr < registers.size() && data.size() == 4){
        registers.at(addr) = (data[0]) << 24|(data[1]) << 16|(data[2]) << 8|(data[3]);
        log(logging, "[REG] : data = " + std::to_string(registers.at(addr)));
      };
      log(logging, "[REG] : Finished writing!");
    }

    std::vector<uint32_t> read(const bool& logging, const uint8_t& addr){
      std::vector<uint32_t> data;
      log(logging, "[REG] : Retrieving all bytes...");
      if (addr < registers.size()){
        data.push_back(registers.at(addr));
        log(logging, "[REG] : Retrieved " + std::to_string(registers.at(addr)));
        log(logging, "[REG] : Retrieved successfully!");
      }
      return data;
    }
};

class SDL_GRAPHICS{}; // Currently empty class; to be filled in later.

class CPU{
  private:
    memory RAM;
    reg Register;
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    int PC = 0;

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

    void interrupt(const bool& logging, const uint32_t& MD, const uint32_t intcode){
      //
      switch (intcode & 0xff){

        case 0b0:{ // Print
          uint8_t message_location; // Register 0
          uint8_t length; // Register 1
          uint8_t mode; // Register 2
          kernel_print(logging);
          break;
        }

        default:{
          throw std::invalid_argument("Invalid Interrupt: This interrupt does not exist.");
          break;
        }
      }
    }

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
      // For now, code will be hardcoded to test. Will add the FDE cycle later.
      //while ((running) && (PC < (int)PRG.size())){
      log(logging, "[OS] : ANA32 successfully booted! Awaiting instructions...");
      Register.write(logging, 0, {0b00000000, 0b00000000, 0b00000000, 0b01000001});
      std::cout << Register.read(logging, 0)[0];
      log_complete(logging);
      //}
    }
};

int main(){

  CPU computer;
  bool logging = true;
  log_clear(logging);
  log(logging, "[BIOS] : Initializing instructions...");
  std::vector<uint8_t> PRG = {
  };
  log(logging, "[BIOS] : Initialization successful!");
  log(logging, "[BIOS] : Sending instructions...");
  computer.run(logging, PRG);
  return 0;
}