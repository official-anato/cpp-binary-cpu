import sys
from dataclasses import dataclass

@dataclass
class opcode:
  binary_number: int
  parameter_count: int
  size_bytes: int

unified_opcodes = {
  "HLT": opcode(0x0, 0, 1),
  "ADD": opcode(0x1, 3, 14),
  "SUB": opcode(0x2, 3, 14),
  "MUL": opcode(0x3, 3, 14),
  "DIV": opcode(0x4, 3, 14),
  "MOD": opcode(0x5, 3, 14),
  "JMP": opcode(0x6, 1, 6),
  "JEQ": opcode(0x7, 1, 6),
  "JLT": opcode(0x8, 1, 6),
  "JGT": opcode(0x9, 1, 6),
  "CMP": opcode(0xA, 2, 10),
  "SDL": opcode(0xB, 0, 0),
  "ENS": opcode(0xC, 0, 1),
  "MOV": opcode(0xD, 2, 10),
  "INT": opcode(0xE, 1, 6)
}

def gen_binary_params(params: list):
  binary_parameters = []
  for item in params:
    # 0 is a placeholder value.
    if (item[0] == 'R' or item[0] == 'r' or item[0] == '@'): # RAM and Registers
      data = int(item[1:])
      LSB = 0
      MLSB = 0
      MMSB = 0
      MSB = 0
      binary_parameters.append(LSB)
      binary_parameters.append(MLSB)
      binary_parameters.append(MMSB)
      binary_parameters.append(MSB)

    # Constants and Variables are WIP
    elif (item[0] == '*' and item[1] == '_' and item[-1] == '_'): # Constants
      data = int(item[2:-2])
      LSB = 0
      MLSB = 0
      MMSB = 0
      MSB = 0
      binary_parameters.append(LSB)
      binary_parameters.append(MLSB)
      binary_parameters.append(MMSB)
      binary_parameters.append(MSB)

    elif (item[0] == '_' and item[-1] == '_'): # Variables
      data = int(item[1:-2])
      LSB = 0
      MLSB = 0
      MMSB = 0
      MSB = 0
      binary_parameters.append(LSB)
      binary_parameters.append(MLSB)
      binary_parameters.append(MMSB)
      binary_parameters.append(MSB)

    elif(item[0] == ';'): # Labels
      data = label_list[item[1:]]
      LSB = 0
      MLSB = 0
      MMSB = 0
      MSB = 0
      binary_parameters.append(LSB)
      binary_parameters.append(MLSB)
      binary_parameters.append(MMSB)
      binary_parameters.append(MSB)

    else: 
      data = int(item)
      LSB = 0
      MLSB = 0
      MMSB = 0
      MSB = 0
      binary_parameters.append(LSB)
      binary_parameters.append(MLSB)
      binary_parameters.append(MMSB)
      binary_parameters.append(MSB)

  return binary_parameters

if __name__ == "__main__":
  # establish parameters
  output = []
  byte_counter = 0
  starting_variable_reg = 6
  label_list = {} # format: (label_name, byte_count)

  # Get filename of source
  args = sys.argv[1:]
  argc = len(sys.argv)
  if argc < 2:
    raise IndexError("ASM32 : Error: Missing filename! Please provide one!")

  # Take every line of ASM written by the user and put them in a list.
  asm_list = []
  with open(args[0], "r") as f:
    for line in f:
      if not line.strip():
        continue

      asm_list.append(line)

  # > Pass one:
  for line in asm_list:
    if line.startswith("!"):
      continue

    # Note to self:
    # Prepare code for variables, and constants.
    # Variables are names surrounded by underscores.
    # Constants are the same, with an asterisk as a prefix.

    if (line[0] == '*' or line[0] == '_'):
      continue

    if (line[0] == ';'): # Check if a line is a label
      if len(line) <= 1:
        raise RuntimeError("ASM32 : Error: Label lacks a name!")

      elif (line[1] == ' '): # Accounts for a space
        label_list[line[2:]] = byte_counter
        continue

      else: # No space handler
        label_list[line[1:]] = byte_counter
        continue

    else: # If all checks fail, it's an opcode.
      # Increment the byte_counter to progress through the program.
      byte_counter += unified_opcodes[line.strip().split()[0]].size_bytes

  byte_counter = 0

  # > Pass two:
  for line in asm_list:
    if line.startswith(';') or (line.startswith(';') and line[1] == ' ') or line.startswith("!") or line.startswith("*"):
      continue

    if line.startswith('_'):
      # this line is being separated from the rest because it's going to be different.
      # For now, we just need functioning code.

      continue

    # Retrieve opcode
    op = line.strip().split()[0]
    try:
      byte_counter += unified_opcodes[op].size_bytes # increase byte counter based off opcode
    except KeyError as e:
      raise RuntimeError("ASM32 : Error: An invalid opcode was found in your source.")

    # Remove opcode from line
    line_copy = line[4:]

    # Remove comments
    commentpos = line_copy.find("!")
    if commentpos != -1:
      line_copy = line_copy[0:commentpos]

    # Separate parameters
    normal_params = line_copy.split()

    # Check if opcode has less OR more parameters than it should.
    if unified_opcodes[op].parameter_count > len(normal_params):
      raise RuntimeError("ASM32 : opcode '" + op + "' has less parameters than it should")

    if unified_opcodes[op].parameter_count < len(normal_params):
      raise RuntimeError("ASM32 : opcode '" + op + "' has more parameters than it should!")

    final_params = []
    # Generate binary parameters
    if unified_opcodes[op].parameter_count > 0:
      final_params = gen_binary_params(normal_params)

    # Generate MD byte
    MD = 0
    MD_available = False

    # Write to output
    output.append(unified_opcodes[op].binary_number) # Opcode
    if MD_available: output.append(MD) # MD byte
    if unified_opcodes[op].parameter_count > 0:
      for i in final_params: output.append(i) # Parameters

  # > Writing to file:
  if len(output) > 0:
    with open(args[0]+".bin", 'wb') as outFile:
      outFile.write(bytearray(output))
      print("ASM32 : File written successfully.")

  else:
    raise RuntimeError("ASM32 : Error: Your source filename is empty!")