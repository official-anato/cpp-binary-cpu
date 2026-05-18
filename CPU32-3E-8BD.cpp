/*
>> CURRENTLY WIP PROJECT : Expect bugs.
>> Dubbed "ANA8x32" - aka ANA256, for the lazy people out there.
>> Written by Anato.

>> This is the 3rd edition of ANA32, as a derivative of the archived 8-bit edition.
 >> The goal for this edition is to refactor the code to handle 32 bit addressing/values
 >> without reinventing the wheel.
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

    std::vector<uint8_t> getRAM() const {
      return RAM;
    }

    size_t getSize() const {
      return RAM.size();
    }

    void write(const bool logging, const uint32_t address, const uint32_t value){
      if (address < RAM.size()){
        RAM[address] = value;
      }
    }

    uint8_t read(const bool logging, const uint32_t address){
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
    std::vector<uint32_t> Registers;

  public:
    Registers_Hardware() : Registers(32, 0) {}

    std::vector<uint32_t> getRegisters() const {
      return Registers;
    }

    uint8_t getSize() const {
      return Registers.size();
    }

    void write(const bool logging, const uint32_t address, const uint32_t value){
      if (address < Registers.size()){
        Registers[address] = value;
      }
    }

    uint8_t read(const bool logging, const uint32_t address){
      if (address < Registers.size()){
        uint32_t value = Registers[address];
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

class Cpu{
  private:
    bool Zero = false;
    bool Carry = false;
    bool Sign = false;
    bool sdl_running = false;
    int PC = 0;
    bool internal_logging = false;
    RAM_Hardware RAM = RAM_Hardware(65535);
    Registers_Hardware Registers;
    SDL_GRAPHICS Graphics;

    void load_data_to_RAM(const bool logging, const std::vector<uint8_t>& data){
      for (size_t i = 0; i < data.size(); i++){
        if (i < RAM.getSize()){
          RAM.write(logging, i, data[i]);
        }
      }
    }

    void kernel_print(const bool logging, const uint32_t message_location, const uint32_t length, const uint8_t mode){
      for (int i = message_location; i <= message_location + (length-1); i++){ // Starting at message_location and stopping at length.
        std::cout << RAM.read(logging, i) << std::endl;
      }
    }

    void kernel_userinput(const bool logging, uint8_t src, uint32_t addr){
    }

    void kernel_filewrite(const bool logging, uint32_t filename_location, uint32_t data, uint8_t mode){}
    void kernel_fileread (const bool logging, uint32_t filename_location, uint32_t byte_start, uint32_t byte_stop){}
    uint32_t _math(uint32_t A, uint32_t B, uint8_t operation){
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
    
  public:
    bool save_output = false;
    std::string save_filename = "halt_finish.txt";
    
    // Underscore signifies helper functions.
    uint8_t _fetch(const bool logging){
      uint8_t data = RAM.read(logging, PC);
      PC++;
      return data;
    }

    uint16_t _fetch16(const bool logging){
      uint8_t val1 = _fetch(logging);
      uint8_t val2 = _fetch(logging);
      return (val2 << 8) | val1;
    }

    uint32_t _fetch24(const bool logging){
      uint8_t val1 = _fetch(logging);
      uint8_t val2 = _fetch(logging);
      uint8_t val3 = _fetch(logging);
      return (val3 << 16) | (val2 << 8) | val1;
    }

    uint32_t _fetch32(const bool logging){
      uint8_t val1 = _fetch(logging);
      uint8_t val2 = _fetch(logging);
      uint8_t val3 = _fetch(logging);
      uint8_t val4 = _fetch(logging);
      return (val4 << 24) | (val3 << 16) | (val2 << 8) | val1;
    }
    
    void _0b11exception(std::string func_name){
      throw std::invalid_argument("0b00000011 is not accepted as a parameter for " + func_name);
    }
    
    /*
    // This function will be rewritten to make sure that it's global
    // And doesn't destroy the hard drive/SSD.
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
    */

    uint8_t _get_value(const bool logging, uint8_t MD, uint32_t A, const std::string& func_name){
      switch (MD){
        case 0b00:{
          return A;
          break;
        }
        
        case 0b01:{
          return Registers.read(logging, (int)A);
          break;
        }
        
        case 0b10:{
          return RAM.read(logging, (int)A);
          break;
        }
        
        default:{
          _0b11exception(func_name);
          break;
        }
      }
      return 0;
    }
    
    void _writeram(const bool logging, uint32_t R, uint32_t value){
      RAM.write(logging, R, value);
    }
    
    void _writeregister(const bool logging, uint32_t R, uint32_t value){
      Registers.write(logging, R, value);
    }
    
    void _writedata(const bool logging, uint32_t loc, uint8_t MD3, uint32_t value){
      switch (MD3){
        case 0b00:{
          throw std::invalid_argument("Invalid Parameter : Immediate cannot be used for Result parameter.");
          break;
        }
        
        case 0b01:{
          _writeregister(logging, loc, value);
          break;
        }
        
        case 0b10:{
          _writeram(logging, loc, value); // Create a detecion mechanism for 32 bit values to start chunking aka split-store.
          break;
        }
        
        default:{
          _0b11exception("'Add'");
          break;
        }
      }
    }
   
   void __UpdFlg(const bool logging, uint32_t res){
    if ((int)res < 0){Zero = false; Carry = false; Sign = true;}
    else if ((int)res > 0){Zero = false; Carry = (res > 4294967295U) ? true : false; Sign = false;}
    else if ((int)res == 0){Zero = true; Carry = false; Sign = false;}
   }
   
   void _UpdFlg(const bool logging, uint8_t MD, uint32_t R){
     if (MD == 0b01){
       __UpdFlg(logging, Registers.read(logging, R));
     }
     
     else if (MD == 0b10){
       __UpdFlg(logging, RAM.read(logging, R));
     }
     
     else{
       throw std::invalid_argument("Flag Error: Flag update failure. Parameter is not register or address.");
     }
   }
    
    void ALU(const bool logging, uint8_t MD, uint32_t A, uint32_t B, uint32_t R, uint8_t arithmetic){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t MD3 = (MD >> 4) & 0b11;
      uint32_t X = _get_value(logging, MD1, A, "'A'");
      uint32_t Y = _get_value(logging, MD2, B, "'B'");
      uint32_t res = _math(X, Y, arithmetic);
      _writedata(logging, R, MD3, res);
      _UpdFlg(logging, MD3, R);
    }
    
    void jmp(const bool logging, uint8_t MD, uint32_t value){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t relabs = (MD >> 2) & 0b11;
      switch(relabs){
        case 0b00:{
          PC = _get_value(logging, MD1, value, "'A'");
          break;
        }
        
        case 0b01:{
          PC += _get_value(logging, MD1, value, "'A'");
          break;
        }
      }
    }
    
    void jeq(const bool logging, uint8_t MD, uint32_t value){
      if (Zero == true){
        jmp(logging, MD, value);
      }
    }
    
    void jlt(const bool logging, uint8_t MD, uint32_t value){
      if (Sign == true){
        jmp(logging, MD, value);
      }
    }
    
    void jgt(const bool logging, uint8_t MD, uint32_t value){
      if (Sign == false && Zero == false){
        jmp(logging, MD, value);
      }
    }
    
    void cmp(const bool logging, const uint8_t MD, const uint32_t A, const uint32_t B){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t X = _get_value(logging, MD1, A, "'A'");
      uint8_t Y = _get_value(logging, MD2, B, "'B'");
      uint8_t res = X-Y;
      __UpdFlg(logging, res);
    }
    
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
    
    void halt(const bool logging){
      if (save_output){
        // Declare file
        std::fstream file(save_filename, std::ios::app);
        
        if (file.is_open()){
          // Formatting for the file output
          file  << "RAM (0 - 65535): [";
          
          // Iterate through items in RAM and append them one by one.
          for (const auto& str : RAM.getRAM()){file << str << ", ";}
          file << "]\nPC: " << PC << "\nRegisters (R0 - R31): [";
          
          // Do the same for registers as well
          for (const auto& str : Registers.getRegisters()){file << str << ", ";}
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
    
    void ens(const bool logging){
      if (save_output == true){ save_output = false;}
      else{save_output = true;}
    }
    
    void mov(const bool logging, const uint8_t MD, const uint32_t A, const uint32_t B){
      uint8_t MD1 = (MD) & 0b11;
      uint8_t MD2 = (MD >> 2) & 0b11;
      uint8_t source_value = _get_value(logging, MD1, A, "'A'");
      _writedata(logging, B, MD2, source_value);
    }

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
    
    void run(const bool& logging, const std::vector<uint8_t>& PRG){
      bool running = true;
      internal_logging = logging;
      load_data_to_RAM(logging, PRG);
      while ((running) && (PC < (int)RAM.getSize())){
        uint8_t opcode = RAM.read(logging, PC);
        switch(opcode){
          case 0b0:{
            halt(logging);
            running = false;
            break;
          }
            
          case 0b1: { // Add
            uint8_t MD = _fetch(logging);
            uint32_t A = _fetch32(logging);
            uint32_t B = _fetch32(logging);
            uint32_t R = _fetch32(logging);
            ALU(logging, MD, A, B, R, 0b00);
            break;
          }
          
          case 0b10:{ // Sub
            uint8_t MD = _fetch(logging);
            uint32_t A = _fetch32(logging);
            uint32_t B = _fetch32(logging);
            uint32_t R = _fetch32(logging);
            ALU(logging, MD, A, B, R, 0b01);
            break;
          }
          
          case 0b11:{ // Mul
            uint8_t MD = _fetch(logging);
            uint32_t A = _fetch32(logging);
            uint32_t B = _fetch32(logging);
            uint32_t R = _fetch32(logging);
            ALU(logging, MD, A, B, R, 0b10);
            break;
          }
          
          case 0b100:{ // Div
            uint8_t MD = _fetch(logging);
            uint32_t A = _fetch32(logging);
            uint32_t B = _fetch32(logging);
            uint32_t R = _fetch32(logging);
            ALU(logging, MD, A, B, R, 0b11);
            break;
          }
          
          case 0b101:{ // Mod
            uint8_t MD = _fetch(logging);
            uint32_t A = _fetch32(logging);
            uint32_t B = _fetch32(logging);
            uint32_t R = _fetch32(logging);
            ALU(logging, MD, A, B, R, 0b100);
            break;
          }
          
          // Here, MD has 2 uses.
          // The first 2 bits are either 01 and 10
          // referring to register or address,
          // the next 2 bits are 00 and 01
          // 00 being a relative jump;
          // and 01 being absolute.
          case 0b110:{
            uint8_t MD = _fetch(logging);
            uint32_t value = _fetch32(logging);
            jmp(logging, MD, value);
            break;
          }
          
          case 0b111:{
            uint8_t MD = _fetch(logging);
            uint32_t value = _fetch32(logging);
            jeq(logging, MD, value);
            break;
          }
          
          case 0b1000:{
            uint8_t MD = _fetch(logging);
            uint32_t value = _fetch32(logging);
            jlt(logging, MD, value);
            break;
          }
          
          case 0b1001:{
            uint8_t MD = _fetch(logging);
            uint32_t value = _fetch32(logging);
            jgt(logging, MD, value);
            break;
          }
          
          case 0b1010:{
            uint8_t MD = _fetch(logging);
            uint32_t A = _fetch32(logging);
            uint32_t B = _fetch32(logging);
            cmp(logging, MD, A, B);
            break;
          }
          
          case 0b1011:{
            uint8_t intcode = _fetch(logging);
            sdl_system(logging, intcode); // This command will function like interrupt(), but will communicate with SDL instead.
            break;
          }
          
          case 0b1100:{
            PC++;
            // Temporarily, this opcode will be used a testing ground.
            // Write to RAM starting from address 1.
            ALU(logging, 0b100000, 0b01000000, 0b1, 0b1, 0b00);
            
            // Write to Registers.
            mov(logging, 0b0100, 0b1, 0b00); // loc
            mov(logging, 0b0100, 0b1, 0b01); // len
            mov(logging, 0b0100, 0b1, 0b10); // mode // Ignore this one for now.

            // Call interrupt()
            interrupt(logging, 0b0, 0b0);

            // ens(logging);
            halt(logging);
            break;
          }
          
          case 0b1101:{
            uint8_t MD = _fetch(logging);
            uint32_t A = _fetch32(logging);
            uint32_t B = _fetch32(logging);
            mov(logging, MD, A, B);
            break;
          }

          case 0b1110:{
            uint8_t MD = _fetch(logging);
            uint32_t value = _fetch32(logging);
            interrupt(logging, MD, value);
            break;
          }

          default:{
            std::cout << "Invalid Opcode!" << std::endl;
            break;
          }
        }
      }
    }
    
};

int main(int argc, char* argv[]) {
  std::vector<uint8_t> PRG = {};
  const char* os = 
  #ifdef _WIN32
    "Windows";
  #elif __APPLE__
    "Mac";
  #elif __linux__
    "Linux";
  #else
    "Unknown";
  #endif
  std::cout << os << std::endl;

  // argv[0] is the program name; Ignore.
  // argv[1] is the option ("Compile" or "File")
  // argv[2] is the program filename.
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <Compile|File> <filename>" << std::endl;
    return 1;
  }

  if (argc > 3) {
    std::cerr << "Error: Too many arguments provided." << std::endl;
    return 1;
  }

  std::string option = argv[1];
  std::string filename = argv[2];
  std::string target_filename;

  if (option == "Compile") {
    std::string command = (os == "Windows") ? "ASM32 \"" + filename + "\"" : "./ASM32 \"" + filename + "\"";
    
    std::cout << "Compiling: " << filename << "..." << std::endl;
    
    // std::system executes the command and halts this program until it finishes
    int exit_code = std::system(command.c_str());
    if (exit_code != 0) {
      std::cerr << "Error: ASM32 compilation failed." << std::endl;
      return 1;
    }

    // Appends .bin to the exact filename.
    // e.g., "asm.txt" becomes "asm.txt.bin"
    target_filename = filename + ".bin";
  } 
  else if (option == "File") {
    // Just read the file provided directly
    target_filename = filename;
  } 
  else {
    std::cerr << "Error: Invalid option '" << option << "'. Expected 'Compile' or 'File'." << std::endl;
    return 1;
  }

  // Check if the target file exists
  if (!fs::exists(target_filename)) {
    std::cerr << "Error: File '" << target_filename << "' does not exist." << std::endl;
    return 1;
  }

  std::ifstream file(target_filename, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file '" << target_filename << "'." << std::endl;
    return 1;
  }

  // Read the file byte by byte and store it in PRG.
  // NOTES: Credit to Github Copilot for suggesting this code.
  // I do not understand this yet, but I will eventually learn.
  uint8_t byte;
  while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
    PRG.push_back(byte);
  }
  file.close();

  // Assuming 'Cpu' is defined elsewhere in your project
  Cpu computer;
  bool logging = true;
  computer.run(logging, PRG);
  return 0;
}