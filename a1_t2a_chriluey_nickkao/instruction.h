/* This file contains the prototypes and constants needed to use the
   routines defined in instruction.c
*/

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum y86_icode {
  I_HALT      = 0x0,
  I_NOP       = 0x1,
  I_RRMVXX    = 0x2,
  I_IRMOVQ    = 0x3,
  I_RMMOVQ    = 0x4,
  I_MRMOVQ    = 0x5,
  I_OPQ       = 0x6,
  I_JXX       = 0x7,
  I_CALL      = 0x8,
  I_RET       = 0x9,
  I_PUSHQ     = 0xA,
  I_POPQ      = 0xB,
  I_INVALID   = 0x10,
  I_TOO_SHORT = 0x11
} y86_icode_t;

typedef enum y86_reg {
  R_RAX = 0x0,
  R_RCX = 0x1,
  R_RDX = 0x2,
  R_RBX = 0x3,
  R_RSP = 0x4,
  R_RBP = 0x5,
  R_RSI = 0x6,
  R_RDI = 0x7,
  R_R8  = 0x8,
  R_R9  = 0x9,
  R_R10 = 0xA,
  R_R11 = 0xB,
  R_R12 = 0xC,
  R_R13 = 0xD,
  R_R14 = 0xE,
  R_NONE = 0xF
} y86_register_t;

typedef enum y86_condition {
  C_NC = 0x0,
  C_LE = 0x1,
  C_L  = 0x2,
  C_E  = 0x3,
  C_NE = 0x4,
  C_GE = 0x5,
  C_G  = 0x6
} y86_condition_t;

typedef enum y86_operation {
  A_ADDQ = 0x0,
  A_SUBQ = 0x1,
  A_ANDQ = 0x2,
  A_XORQ = 0x3,
  A_MULQ = 0x4,
  A_DIVQ = 0x5,
  A_MODQ = 0x6
} y86_operation_t;

typedef struct instruction {

  y86_icode_t    icode;  // determines what op
  uint8_t        ifun;   // the different "types" of op
  y86_register_t rA;    // R_RXX
  y86_register_t rB;    
  uint64_t       valC;  // holds the extra part 8 byte part
  
  uint64_t       location;  //adderss of instruction 
  uint64_t       valP;    //the address of the next instrunction based on the size of thew current instrunction + lcoation 
} y86_instruction_t;

#define CC_ZERO_MASK     0x1
#define CC_SIGN_MASK     0x2
#define CC_CARRY_MASK    0x4
#define CC_OVERFLOW_MASK 0x8

typedef struct machine_state {
  
  uint8_t *programMap;
  uint64_t programSize;
  
  uint64_t programCounter;

  uint64_t registerFile[16];

  uint8_t conditionCodes;
  
} machine_state_t;

typedef struct bp{
  uint64_t address; //current location
  struct breakPointNode* next; //next "bp" in list
} breakPointNode;

typedef struct bps{
  breakPointNode* head;
  breakPointNode* tail;
  int size;
} breakPointLL;

int fetchInstruction(machine_state_t *state, y86_instruction_t *instr);
int executeInstruction(machine_state_t *state, y86_instruction_t *instr);

int memReadByte(machine_state_t *state,	uint64_t address, uint8_t *value);
int memReadQuadLE(machine_state_t *state, uint64_t address, uint64_t *value);
int memWriteByte(machine_state_t *state,  uint64_t address, uint8_t value);
int memWriteQuadLE(machine_state_t *state, uint64_t address, uint64_t value);
uint8_t getUpperNib(uint8_t a);
uint8_t getLowerNib(uint8_t a);
int makeOneWord(uint8_t int1, uint8_t int2, uint16_t *word);
bool errICode(uint8_t icode);
int executeMOV(uint8_t ifun, y86_register_t rA, y86_register_t rB, machine_state_t* state);
uint8_t executeOPq(uint8_t ifun, y86_register_t rA, y86_register_t rB, machine_state_t *state);
int executeJXX(uint8_t ifun, uint64_t valC, machine_state_t* state);
int determineSize(uint8_t icode);
int determineValC(machine_state_t *state, uint64_t* ret, uint8_t icode);
uint8_t determineRegA(uint8_t icode, uint8_t reg);
uint8_t determineRegB(uint8_t icode, uint8_t reg);
void fetchShort(machine_state_t *state, y86_instruction_t *instr);
void fetchInvalid(machine_state_t *state, y86_instruction_t *instr);
bool requireValC(uint8_t icode);
bool requireReg(uint8_t icode);


#endif /* INSTRUCTION */
