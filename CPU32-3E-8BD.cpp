/*
>> CURRENTLY WIP PROJECT : Expect bugs.
>> Dubbed "ANA8x32" - aka ANA256, for the lazy people out there.
>> Written by Anato.

>> This is the 3rd edition of ANA32, as a derivative of the archived 8-bit edition.
 >> The goal for this edition is to refactor the code to handle 32 bit addressing/values
 >> without reinventing the wheel.
*/

// #include <source_location> // Re-enable if you ever need to debug - NEEDS C++20 or newer!
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <climits>
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;

class motherboard{
  private:
    // Logging functions.
    bool logging_enabled = false;
    std::vector<std::string> logs;
    void log(){}
    void clear_logs(){}
    void SL2D(){ // S(ave) L(ogs) 2(to) D(isk)
    }

    // Classes
    class BIOS{
      private:
        // No contents yet
      public:
        void change_clock(){}
    };

    class clock{
      private: 
        // No contents yet.
      public:
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    };

    class Storage{
      private:
        std::vector<int> VHD;
      public:
        bool drivehead_ready = false;
        void activate_drivehead(){}
        void write(){}
        void read(){}
        void save_VHD(){}
    };

    class RAM_Hardware {
      private:
        std::vector<uint8_t> RAM;
    
      public:
        motherboard* board;

        RAM_Hardware(motherboard* b) {
            board = b; 
        }

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
    
          else {
            throw std::runtime_error("Attempted to write to an invalid RAM address.");
          }
        }
    
        void write32(const bool& logging, const uint32_t start_addr, const uint32_t data){ // Writes 1 word to RAM
          // Get the 4 bytes
          uint8_t LSB = (data) & 0xFF;
          uint8_t mid_LSB = (data >> 8) & 0xFF;
          uint8_t mid_MSB = (data >> 16) & 0xFF;
          uint8_t MSB = (data >> 24) & 0xFF;
    
          // Call the function.
          write(board->logging_enabled, start_addr, LSB);
          write(board->logging_enabled, start_addr+1, mid_LSB);
          write(board->logging_enabled, start_addr+2, mid_MSB);
          write(board->logging_enabled, start_addr+3, MSB);
        }
    
        uint8_t read(const bool logging, const uint32_t address){
          if (address < RAM.size()){
            uint8_t value = RAM[address];
            return value;
          }
    
          else{
            std::cout << (int)address << std::endl;
            throw std::runtime_error("Error: Attempted to read from an invalid RAM address.");
          }
        }
    
        uint32_t read32(const bool logging, const uint32_t start_addr){
          uint8_t LSB = read(board->logging_enabled, start_addr + 0);
          uint8_t mid_LSB = read(board->logging_enabled, start_addr + 1);
          uint8_t mid_MSB = read(board->logging_enabled, start_addr + 2);
          uint8_t MSB = read(board->logging_enabled, start_addr + 3);
          uint32_t chunk32 = LSB | (mid_LSB << 8) | (mid_MSB << 16) | (MSB << 24);
          return chunk32;
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
    
          else {
            throw std::runtime_error("Attempted to write to an invalid RAM address.");
          }
        }
    
        uint32_t read(const bool logging, const uint32_t address){
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
        void draw_image(){} // Creates an image based on the data present in the VRAM.
    }; // Currently empty class; to be filled in the future.
    
    class Cpu{
      private:
        void kernel_print(const bool logging, const uint32_t message_location, const uint32_t length, const uint8_t MD){
          uint8_t mode = (MD) & 0b11; // ADR (0b10) or REG (0b01)
          uint8_t mode2 = (MD >> 2) & 0b11; // CHAR (0b00) or INT (0b01)
          for (int i = message_location; i <= message_location + (length-1); i++){ // Starting at message_location and stopping at length.
            if (mode == 0b10){
              if (mode2 == 0x0){
                std::cout << (char)board->RAM.read(board->logging_enabled, i); // <--
              }
              else{
                std::cout << (int)board->RAM.read(board->logging_enabled, i);
              }
            }
    
            else if (mode == 0b01){
              if (mode2 == 0x0){
                std::cout << (char)Registers.read(board->logging_enabled, i);
              }
              else{
                std::cout << (int)Registers.read(board->logging_enabled, i);
              }
            }
    
            else{
              throw std::runtime_error("Invalid mode for kernel_print.");
            }
          }
        }
    
        void kernel_userinput(const bool logging, uint8_t src, uint32_t addr){}
    
        void kernel_filewrite(const bool logging, uint32_t filename_location, uint32_t filename_len_char, uint32_t byte_location, uint32_t byte_amount, uint8_t MD1, uint8_t MD2){
          uint8_t mode = (MD1) & 0b11; // 2 modes - 0x1 indicates the filename location is in the registers, 0x0 indicates RAM address.
          uint32_t mode2 = (MD2) && 0b11; // 2 modes - 0x1 means 32 bit chunking, 0x0 means 8 bit chunking.
          std::string filename;
    
          // Utilize for loops to read the filename.
          for (uint32_t i = filename_location; i >= (filename_location + (byte_amount-1)); i++){
            if (mode == 0x1){
              if (mode2 == 0x0){
              }
    
              else if (mode2 == 0x1){
              }
    
              else {
                throw std::runtime_error("");
              }
            }
    
            else if (mode == 0x0){
              if (mode2 == 0x0){
              }
    
              else if (mode2 == 0x1){
              }
    
              else {
                throw std::runtime_error("");
              }
            }
    
            else {
              throw std::runtime_error("");
            }
          }
          // std::fstream target(filename, std::ios::app);
        }
    
        void kernel_fileread (const bool logging, uint32_t filename_location, uint32_t filename_len_char, uint32_t byte_location, uint32_t byte_amount, uint8_t MD1, uint8_t MD2){
        }
        
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
              throw std::runtime_error("Syntax Error: Invalid mathematical operation!");
              break;
            }
          }
        }

        void kernel_init_shutdown(const bool logging){
          board->powered_on = false;
        }
        
      public:
        Registers_Hardware Registers;
        bool Zero = false;
        bool Carry = false;
        bool Sign = false;
        bool sdl_running = false;
        int PC = 0;
        bool internal_logging = false;
        motherboard* board;

        Cpu(motherboard* b) {
            board = b; 
        }
        
        // Underscore signifies helper functions.
        uint8_t _fetch(const bool logging){
          uint8_t data = board->RAM.read(board->logging_enabled, PC);
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
          throw std::runtime_error("0b00000011 is not accepted as a parameter for " + func_name);
        }
    
        uint32_t _get_value(const bool logging, uint8_t MD, uint32_t A, const std::string& func_name){
          switch (MD){
            case 0b00:{
              return A;
              break;
            }
            
            case 0b01:{
              return Registers.read(board->logging_enabled, A);
              break;
            }
            
            case 0b10:{
              return board->RAM.read32(board->logging_enabled, A);
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
          board->RAM.write(board->logging_enabled, R, value);
        }
        
        void _writeregister(const bool logging, uint32_t R, uint32_t value){
          Registers.write(board->logging_enabled, R, value);
        }
        
        void _writedata(const bool logging, uint32_t loc, uint8_t MD3, uint32_t value){
          switch (MD3){
            case 0b00:{
              throw std::runtime_error("Invalid Parameter : Immediate cannot be used for Result parameter.");
              break;
            }
            
            case 0b01:{
              _writeregister(board->logging_enabled, loc, value);
              break;
            }
            
            case 0b10:{
              _writeram(board->logging_enabled, loc, value); // Create a detecion mechanism for 32 bit values to start chunking aka split-store.
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
           __UpdFlg(board->logging_enabled, Registers.read(board->logging_enabled, R));
         }
         
         else if (MD == 0b10){
           __UpdFlg(board->logging_enabled, board->RAM.read(board->logging_enabled, R));
         }
         
         else{
           throw std::runtime_error("Flag Error: Flag update failure. Parameter is not register or address.");
         }
       }
        
        void ALU(const bool logging, uint8_t MD, uint32_t A, uint32_t B, uint32_t R, uint8_t arithmetic){
          uint8_t MD1 = (MD) & 0b11;
          uint8_t MD2 = (MD >> 2) & 0b11;
          uint8_t MD3 = (MD >> 4) & 0b11;
          uint32_t X = _get_value(board->logging_enabled, MD1, A, "'A'");
          uint32_t Y = _get_value(board->logging_enabled, MD2, B, "'B'");
          uint32_t res = _math(X, Y, arithmetic);
          _writedata(board->logging_enabled, R, MD3, res);
          _UpdFlg(board->logging_enabled, MD3, R);
        }
        
        void jmp(const bool logging, uint8_t MD, uint32_t value){
          uint8_t MD1 = (MD) & 0b11;
          uint8_t relabs = (MD >> 2) & 0b11;
          switch(relabs){
            case 0b00:{
              PC = _get_value(board->logging_enabled, MD1, value, "'A'");
              break;
            }
            
            case 0b01:{
              PC += _get_value(board->logging_enabled, MD1, value, "'A'");
              break;
            }
          }
        }
        
        void jeq(const bool logging, uint8_t MD, uint32_t value){
          if (Zero == true){
            jmp(board->logging_enabled, MD, value);
          }
        }
        
        void jlt(const bool logging, uint8_t MD, uint32_t value){
          if (Sign == true){
            jmp(board->logging_enabled, MD, value);
          }
        }
        
        void jgt(const bool logging, uint8_t MD, uint32_t value){
          if (Sign == false && Zero == false){
            jmp(board->logging_enabled, MD, value);
          }
        }

        void cmp(const bool logging, const uint8_t MD, const uint32_t A, const uint32_t B){
          uint8_t MD1 = (MD) & 0b11;
          uint8_t MD2 = (MD >> 2) & 0b11;
          uint32_t X = _get_value(board->logging_enabled, MD1, A, "'A'");
          uint32_t Y = _get_value(board->logging_enabled, MD2, B, "'B'");
          uint32_t res = X-Y;
          __UpdFlg(board->logging_enabled, res);
        }
        
        void sdl_system(const bool logging, const uint32_t intcode){
          if (!sdl_running){
            board->Graphics.init_sys();
            sdl_running = true;
            sdl_system(board->logging_enabled, intcode); // Re-run the function to execute the command after initializing SDL.
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
                throw std::runtime_error("Invalid SDL Code: This code does not exist.");
                break;
              }
          }
          }
        }
        
        int halt(const bool logging){
          kernel_init_shutdown(logging); // This is temporary!!!!
          return 0;
        }
        
        void ens(const bool logging){
          if (board->save_output == true){ board->save_output = false;}
          else{board->save_output = true;}
        }
        
        void mov(const bool logging, const uint8_t MD, const uint32_t A, const uint32_t B){
          uint8_t MD1 = (MD) & 0b11;
          uint8_t MD2 = (MD >> 2) & 0b11;
          uint32_t source_value = _get_value(board->logging_enabled, MD1, A, "'A'");
          _writedata(board->logging_enabled, B, MD2, source_value);
        }
    
        void interrupt(const bool logging, const uint32_t MD, const uint32_t intcode){
          switch (intcode & 0xff){
    
            case 0b0:{ // Print
              uint32_t message_location = Registers.read(board->logging_enabled, 0); // Register 0
              uint32_t length = Registers.read(board->logging_enabled, 1); // Register 1
              uint32_t mode = Registers.read(board->logging_enabled, 2); // Register 2
              kernel_print(board->logging_enabled, message_location, length, mode);
              break;
            }
    
            case 0b1: { // User Input
              uint32_t userinput = Registers.read(board->logging_enabled, 0); // Input source - R0
              uint32_t address = Registers.read(board->logging_enabled, 1); // Writing address - R1
              break;
            }
    
            case 0b10: { // File Write
              uint32_t filename_address = Registers.read(board->logging_enabled, 0); // Filename location - R0
              uint32_t filename_length = Registers.read(board->logging_enabled, 1); // Filename char len - R1
              uint32_t data_address = Registers.read(board->logging_enabled, 2); // Byte Location - R2
              uint32_t data_length = Registers.read(board->logging_enabled, 3); // Byte amounr - R3
              uint8_t mode1 = Registers.read(board->logging_enabled, 4); // mode 1 - R4 - RAM or Registers
              uint8_t mode2 = Registers.read(board->logging_enabled, 5); // mode 2 - R5 - 32 bit chunking or 8 bit chunking
              kernel_filewrite(board->logging_enabled, filename_address, filename_length, data_address, data_length, mode1, mode2);
              break;
            }
    
            case 0b11: { // File Read
              uint32_t filename_address = Registers.read(board->logging_enabled, 0); // Filename location - R0
              uint32_t filename_length = Registers.read(board->logging_enabled, 1); // Filename char len - R1
              uint32_t data_address = Registers.read(board->logging_enabled, 2); // Byte Location - R2
              uint32_t data_length = Registers.read(board->logging_enabled, 3); // Byte amounr - R3
              uint8_t mode1 = Registers.read(board->logging_enabled, 4); // mode 1 - R4 - RAM or Registers
              uint8_t mode2 = Registers.read(board->logging_enabled, 5); // mode 2 - R5 - 32 bit chunking or 8 bit chunking
              kernel_fileread(board->logging_enabled, filename_address, filename_length, data_address, data_length, mode1, mode2);
              break;
            }
    
            default:{
              throw std::runtime_error("Invalid Interrupt: This interrupt does not exist.");
              break;
            }
          }
        }
        
        void step(const bool& logging){
          if (PC < (int)board->RAM.getSize()){
            uint8_t opcode = _fetch(logging);
            switch(opcode){
              case 0b0:{
                halt(logging);
                break;
              }
              
              case 0b1: { // Add
                uint8_t MD = _fetch(logging);
                uint32_t A = _fetch32(logging);
                uint32_t B = _fetch32(logging);
                uint32_t R = _fetch32(logging);
                ALU(board->logging_enabled, MD, A, B, R, 0b00);
                break;
              }
            
              case 0b10:{ // Sub
                uint8_t MD = _fetch(logging);
                uint32_t A = _fetch32(logging);
                uint32_t B = _fetch32(logging);
                uint32_t R = _fetch32(logging);
                ALU(board->logging_enabled, MD, A, B, R, 0b01);
                break;
              }
            
              case 0b11:{ // Mul
                uint8_t MD = _fetch(logging);
                uint32_t A = _fetch32(logging);
                uint32_t B = _fetch32(logging);
                uint32_t R = _fetch32(logging);
                ALU(board->logging_enabled, MD, A, B, R, 0b10);
                break;
              }
            
              case 0b100:{ // Div
                uint8_t MD = _fetch(logging);
                uint32_t A = _fetch32(logging);
                uint32_t B = _fetch32(logging);
                uint32_t R = _fetch32(logging);
                ALU(board->logging_enabled, MD, A, B, R, 0b11);
                break;
              }
            
              case 0b101:{ // Mod
                uint8_t MD = _fetch(logging);
                uint32_t A = _fetch32(logging);
                uint32_t B = _fetch32(logging);
                uint32_t R = _fetch32(logging);
                ALU(board->logging_enabled, MD, A, B, R, 0b100);
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
                jmp(board->logging_enabled, MD, value);
                break;
              }

              case 0b111:{
                uint8_t MD = _fetch(logging);
                uint32_t value = _fetch32(logging);
                jeq(board->logging_enabled, MD, value);
                break;
              }
            
              case 0b1000:{
                uint8_t MD = _fetch(logging);
                uint32_t value = _fetch32(logging);
                jlt(board->logging_enabled, MD, value);
                break;
              }
            
              case 0b1001:{
                uint8_t MD = _fetch(logging);
                uint32_t value = _fetch32(logging);
                jgt(board->logging_enabled, MD, value);
                break;
              }
            
              case 0b1010:{
                uint8_t MD = _fetch(logging);
                uint32_t A = _fetch32(logging);
                uint32_t B = _fetch32(logging);
                cmp(board->logging_enabled, MD, A, B);
                break;
              }
            
              case 0b1011:{
                uint8_t intcode = _fetch(logging);
                sdl_system(board->logging_enabled, intcode); // This command will function like interrupt(), but will communicate with SDL instead.
                break;
              }
            
              case 0b1100:{
                ens(logging);
                break;
              }
            
              case 0b1101:{
                uint8_t MD = _fetch(logging);
                uint32_t A = _fetch32(logging);
                uint32_t B = _fetch32(logging);
                mov(board->logging_enabled, MD, A, B);
                break;
              }
  
              case 0b1110:{
                uint8_t MD = _fetch(logging);
                uint32_t value = _fetch32(logging);
                interrupt(board->logging_enabled, MD, value);
                break;
              }
  
              default:{
                std::cout << "Invalid Opcode!" << std::endl;
                break;
              }
            }
          }

          else{
            throw std::runtime_error("PC Error: Program Counter has exceeded " + std::to_string((int)board->RAM.getSize()) + "bytes, which is the amount of RAM currently installed.");
          }
        }
    };

    // Internal variables
    std::vector<uint8_t> PRG;
    bool powered_on = false;
    bool save_output = false;
    std::string save_filename = "halt_finish.txt";

    // Establish hardware
    RAM_Hardware RAM = RAM_Hardware(65535);
    SDL_GRAPHICS Graphics;
    Cpu CPU = Cpu(this);
    
    void load_data_to_RAM(const std::vector<uint8_t>& data){
      for (size_t i = 0; i < data.size(); i++){
        if (i < RAM.getSize()){
          RAM.write(logging_enabled, i, data[i]);
        }
      }
    }

  public:
    void power_on(const bool logging, const std::vector<uint8_t> PRG){
      powered_on = true;
      logging_enabled = logging;
      load_data_to_RAM(PRG);
      while (powered_on){ // This is the update cycle.
        // this line here would be the call for updating user input.
        Graphics.draw_image(); // Currently just being called for future-ready-proofing.
        CPU.step(logging); // Let CPU run one opcode. A step.
      }

      if (save_output){
        // Declare file and erase previous logs.
        std::fstream file1(save_filename, std::ios::out);
        if (file1.is_open()){file1 << "";}

        // Declare file
        std::fstream file(save_filename, std::ios::app);
            
        if (file.is_open()){
          // Formatting for the file output
          file  << "RAM (0 - 65535): [";
              
          // Iterate through items in RAM and append them one by one.
          for (const auto& str : RAM.getRAM()){file << (int)str << ", ";}
          file << "]\nPC: " << CPU.PC << "\nRegisters (R0 - R31): [";
              
          // Do the same for registers as well
          for (const auto& str : CPU.Registers.getRegisters()){file << (int)str << ", ";}
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

  if ((option == "Compile") || (option == "compile")) {
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
  else if ((option == "File") || (option == "file")) {
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
  // I don't understand this yet, but I will eventually learn.
  uint8_t byte;
  while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
    PRG.push_back(byte);
  }
  file.close();

  motherboard computer;
  bool logging = true;
  computer.power_on(logging, PRG);
  return 0;
}