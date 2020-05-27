#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>

#include "printRoutines.h"

static const char *instrName[256][256] = {
  [I_HALT]   = {"halt"},
  [I_NOP]    = {"nop"},
  [I_RRMVXX] = {
    [C_NC] = "rrmovq",
    [C_LE] = "cmovle",
    [C_L]  = "cmovl",
    [C_E]  = "cmove",
    [C_NE] = "cmovne",
    [C_GE] = "cmovge",
    [C_G]  = "cmovg"},
  [I_IRMOVQ] = {"irmovq"},
  [I_RMMOVQ] = {"rmmovq"},
  [I_MRMOVQ] = {"mrmovq"},
  [I_OPQ]    = {
    [A_ADDQ] = "addq",
    [A_SUBQ] = "subq",
    [A_ANDQ] = "andq",
    [A_XORQ] = "xorq",
    [A_MULQ] = "mulq",
    [A_DIVQ] = "divq",
    [A_MODQ] = "modq"
  },
  [I_JXX]    = {
    [C_NC] = "jmp",
    [C_LE] = "jle",
    [C_L]  = "jl",
    [C_E]  = "je",
    [C_NE] = "jne",
    [C_GE] = "jge",
    [C_G]  = "jg"},
  [I_CALL]   = {"call"},
  [I_RET]    = {"ret"},
  [I_PUSHQ]  = {"pushq"},
  [I_POPQ]   = {"popq"}
};

static const char *regName[R_NONE] = {
  [R_RAX] = "%rax",
  [R_RCX] = "%rcx",
  [R_RDX] = "%rdx",
  [R_RBX] = "%rbx",
  [R_RSI] = "%rsi",
  [R_RDI] = "%rdi",
  [R_RSP] = "%rsp",
  [R_RBP] = "%rbp",
  [R_R8]  = "%r8",
  [R_R9]  = "%r9",
  [R_R10] = "%r10",
  [R_R11] = "%r11",
  [R_R12] = "%r12",
  [R_R13] = "%r13",
  [R_R14] = "%r14"
};

static inline int printRegister(FILE *file, y86_register_t reg) {
  
  assert(reg < R_NONE);
  return fprintf(file, "%s", regName[reg]);
} 

static inline int printValC(FILE *file, uint64_t val) {
  
  return fprintf(file, "0x%lx", val);
} 

int printErrorCommandTooLong(FILE *file) {

  return fprintf(file, "    # Command is too long, ignored.\n");
}

int printErrorInvalidCommand(FILE *file, char *command, char *parameter) {

  return fprintf(file, "    # Invalid command or parameters: %s %s\n",
		 command, parameter ? parameter : "");
}

int printErrorInvalidInstruction(FILE *file, y86_instruction_t *instr) {

  return fprintf(file, "    # Invalid instruction                 "
		 "PC = 0x%lx\n", instr->location);
}

int printErrorShortInstruction(FILE *file, y86_instruction_t *instr) {

  return fprintf(file, "    # Instruction is incomplete           "
		 "PC = 0x%lx\n", instr->location);
}

int printErrorInvalidMemoryLocation(FILE *file, y86_instruction_t *instr, uint64_t address) {

  return fprintf(file, "    # Invalid memory access: 0x%16lx", address) +
    (instr ? fprintf(file, "      PC = 0x%lx\n", instr->location) :
     fprintf(file, "\n"));
}

int printInstruction(FILE *file, y86_instruction_t *instr) {

  if (instr->icode == I_INVALID)
    return printErrorInvalidInstruction(file, instr);
  else if (instr->icode == I_TOO_SHORT)
    return fprintf(file, "    # Instruction is incomplete           "
		   "PC = 0x%lx\n", instr->location);
  
  assert(*instrName[instr->icode][instr->ifun]);
  int chars = fprintf(file, "    %-8s",
		      instrName[instr->icode][instr->ifun]);
  
  switch (instr->icode) {
  case I_IRMOVQ:
    chars += fprintf(file, "$");
    chars += printValC(file, instr->valC);
    chars += fprintf(file, ", ");
    chars += printRegister(file, instr->rB);
    break;
  case I_PUSHQ:
  case I_POPQ:
    chars += printRegister(file, instr->rA);
    break;
  case I_CALL:
  case I_JXX:
    chars += printValC(file, instr->valC);
    break;
  case I_RMMOVQ:
    chars += printRegister(file, instr->rA);
    chars += fprintf(file, ", ");
    chars += printValC(file, instr->valC);
    chars += fprintf(file, "(");
    chars += printRegister(file, instr->rB);
    chars += fprintf(file, ")");
    break;
  case I_MRMOVQ:
    chars += printValC(file, instr->valC);
    chars += fprintf(file, "(");
    chars += printRegister(file, instr->rB);
    chars += fprintf(file, "), ");
    chars += printRegister(file, instr->rA);
    break;
  case I_RRMVXX:
  case I_OPQ:
    chars += printRegister(file, instr->rA);
    chars += fprintf(file, ", ");
    chars += printRegister(file, instr->rB);
    break;
  default:
    break;
  }

  while (chars < 40) {
    fputc(' ', file);
    chars++;
  }

  chars += fprintf(file, "# PC = 0x%lx\n", instr->location);
  return chars;
}

int printRegisterValue(FILE *file, machine_state_t *state,
		       y86_register_t reg) {
  
  assert(reg < R_NONE);
  return fprintf(file, "    # R[%-4s] = 0x%lx\n", regName[reg],
		 state->registerFile[reg]);
}

int printMemoryValueByte(FILE *file, machine_state_t *state, uint64_t addr) {

  uint8_t value;
  if (memReadByte(state, addr, &value))
    return fprintf(file, "    # M_1[0x%lx] = 0x%x\n", addr, value);
  else
    return printErrorInvalidMemoryLocation(file, NULL, addr);
}

int printMemoryValueQuad(FILE *file, machine_state_t *state, uint64_t addr) {

  uint64_t value;
  if (memReadQuadLE(state, addr, &value))
    return fprintf(file, "    # M_8[0x%lx] = 0x%lx\n", addr, value);
  else
    return printErrorInvalidMemoryLocation(file, NULL, addr);
}
