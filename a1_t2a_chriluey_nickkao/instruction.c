#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#include "instruction.h"
#include "printRoutines.h"

/* Reads one byte from memory, at the specified address. Stores the
   read value into *value. Returns 1 in case of success, or 0 in case
   of failure (e.g., if the address is beyond the limit of the memory
   size). */
int memReadByte(machine_state_t *state,	uint64_t address, uint8_t *value) {

  if (address >= 0 && address < state->programSize) {
    *value = state->programMap[address];
    return 1;
  } else {
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
    return 0;
  }
}

/* Reads one quad-word (64-bit number) from memory in little-endian
   format, at the specified starting address. Stores the read value
   into *value. Returns 1 in case of success, or 0 in case of failure
   (e.g., if the address is beyond the limit of the memory size). */
int memReadQuadLE(machine_state_t *state, uint64_t address, uint64_t *value) {
  if (address >= 0 && address + 7 < state->programSize) {
    uint8_t *pMap = state->programMap;
//    uint16_t *word1, *word2, *word3, *word4;
    uint16_t* word1 = malloc(sizeof(uint16_t));
    uint16_t* word2 = malloc(sizeof(uint16_t));
    uint16_t* word3 = malloc (sizeof(uint16_t));
    uint16_t* word4 = malloc(sizeof(uint16_t));
    makeOneWord(pMap[address], pMap[address + 1], word1);
    makeOneWord(pMap[address + 2], pMap[address + 3], word2);
    makeOneWord(pMap[address + 4], pMap[address + 5], word3);
    makeOneWord(pMap[address + 6], pMap[address + 7], word4);
    *value = ((uint64_t) *word4 << 48) | ((uint64_t) *word3 << 32) | ((uint64_t) *word2 << 16) | *word1;
    free(word1);
    free(word2);
    free(word3);
    free(word4);
    return 1;
  } else {
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
    return 0;
  }
}

/* Stores the specified one-byte value into memory, at the specified
   address. Returns 1 in case of success, or 0 in case of failure
   (e.g., if the address is beyond the limit of the memory size). */
int memWriteByte(machine_state_t *state,  uint64_t address, uint8_t value) {
  if (address >= 0 && address < state->programSize) {
    state->programMap[address] = value;
    return 1;
  } else {
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
    return 0;
  }
}

/* Stores the specified quad-word (64-bit) value into memory, at the
   specified start address, using little-endian format. Returns 1 in
   case of success, or 0 in case of failure (e.g., if the address is
   beyond the limit of the memory size). */
int memWriteQuadLE(machine_state_t *state, uint64_t address, uint64_t value) {
  if (address >= 0 && address + 7 < state->programSize) {
    for (int i = 0; i < 8; i++) {
        state->programMap[address + i] = (uint8_t)(value & 0x00000000000000FF);
        value >>= 8;
    }
    return 1;
  } else {
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
    return 0;
  }
}

/* Fetches one instruction from memory, at the address specified by
   the program counter. Does not modify the machine's state. The
   resulting instruction is stored in *instr. Returns 1 if the
   instruction is a valid non-halt instruction, or 0 (zero)
   otherwise. */

  //register checking, below are the things that need to be true to be valid
    // irmovq rA == F
    // pushq rB == F
    // popq rB == F
  
int fetchInstruction(machine_state_t *state, y86_instruction_t *instr) {
    /* THIS PART TO BE COMPLETED BY THE STUDENT */

    //extract op code from adr
      
      uint8_t* opcode = malloc(sizeof(uint8_t));
      int opcodeID = memReadByte(state, state->programCounter, opcode);
      int instructSize = 0;

      if(opcodeID == 0){
        fetchShort(state, instr);
        return 0;
      }

      uint8_t icode = getUpperNib(*opcode);
      uint8_t ifun = getLowerNib(*opcode);

      uint8_t* reg = malloc(sizeof(uint8_t));
      int regID;
      
      if(requireReg(icode)){
        regID = memReadByte(state, state->programCounter+1, reg);
        if(regID == 0){
        fetchShort(state, instr);
        return 0;
        }
      }
      
    
      uint8_t rA = determineRegA(icode, *reg);
      uint8_t rB = determineRegB(icode, *reg);

    

      uint64_t* valueC = malloc(sizeof(uint64_t));
   
      // 0 = out of memory 
      // 1 = success
      // 2 = valC not part of comand 
    
      int valCID = determineValC(state, valueC, icode);     
      uint64_t valC;
      if(valCID == 0){
        fetchShort(state, instr);
        return 0;
      }else if(valCID == 1){
        valC = *valueC;
      }else if(valCID == 2){
        valC = 0x0;
      }
 
      instructSize = determineSize(icode);
 
      
      switch(icode){
        //size == 1
        //ifun == 0
        case 0x0:
          if(ifun != 0){
            fetchInvalid(state, instr);
            return 0;
          }
          break;
        //size == 1
        //ifun == 0
        case 0x1:
          if(ifun != 0){
            fetchInvalid(state, instr);
            return 0;
          }
          break;
        //size == 2
        //ifun == 0 
        case 0x2:
       
          if(ifun > 6 || rA == 0xF || rB == 0xF){
            fetchInvalid(state, instr);
            return 0;
          }
          break;


        //size == 10
        //ifun == 0
        //rA == F
        case 0x3:
          if(ifun != 0 || rA != 0xF || rB ==  0xF){
            fetchInvalid(state, instr);
            return 0;
          }
          break;

        //size == 10
        //ifun == 0
        case 0x4:
          if(ifun != 0 || rA == 0xF || rB == 0xF){
            fetchInvalid(state, instr);
            return 0;
          }    
          break;
        //size == 10
        //ifun == 0 
        
        case 0x5:
          if(ifun != 0 || rA == 0xF || rB == 0xF){
            fetchInvalid(state, instr);
            return 0;
          }
          break;
     
        //size == 2 
        //ifun == VARIABLE
        case 0x6:
          if(ifun > 6 || rA == 0xF || rB == 0xF){
            fetchInvalid(state, instr);
            return 0;
          }
          break;

        //size == 9
        //ifun VARIABLE
        case 0x7:
          if(ifun > 6){
            fetchInvalid(state, instr);
            return 0;
          }
          break;

        //size == 9
        //ifun == 0
        case 0x8:
          if(ifun != 0){
            fetchInvalid(state, instr);
            return 0;
          }
          break;

        //size == 1
        //ifun == 0
        case 0x9:
          if(ifun != 0){
            fetchInvalid(state, instr);
            return 0;
          }
          break;

        //size == 2
        //ifun ==0
        //rB == F
        case 0xA:
          if(ifun != 0 || rA == 0xF || rB != 0xF){
            fetchInvalid(state, instr);
            return 0;
          }
          break;
        //size == 2
        //ifun ==0
        //rB == F
        case 0xB:
          if(ifun != 0 || rA == 0xF || rB != 0xF){
            fetchInvalid(state, instr);
            return 0;
          }
        case 0x10: 
          fetchInvalid(state, instr);
          return 0;
        case 0x11:
          fetchShort(state, instr);
          return 0;
        default: 
          fetchInvalid(state, instr);
          return 0;
      }





  instr->icode = icode; //y86_icode_t
  instr->ifun = ifun; //uint8_t
  instr->rA = rA; //y86_register_t
  instr->rB = rB; //y86_register_t
  instr->valC = valC; //uint64_t
  instr->location = state->programCounter; //uint64_t
  instr->valP = state->programCounter + instructSize; //uint64_t
  return 1;
}


void fetchShort(machine_state_t *state, y86_instruction_t *instr){
  instr->icode = I_TOO_SHORT; //y86_icode_t
  instr->ifun = 0x0; //uint8_t
  instr->rA = 0xF; //y86_register_t
  instr->rB = 0xF; //y86_register_t
  instr->valC = 0x0; //uint64_t
  instr->location = state->programCounter; //uint64_t
  instr->valP = state->programCounter; //uint64_t
}

void fetchInvalid(machine_state_t *state, y86_instruction_t *instr){
  

  instr->icode = I_INVALID; //y86_icode_t
  instr->ifun = 0x0; //uint8_t
  instr->rA = 0xF; //y86_register_t
  instr->rB = 0xF; //y86_register_t
  instr->valC = 0x0; //uint64_t
  instr->location = state->programCounter; //uint64_t
  instr->valP = state->programCounter; //uint64_t
}
/* Executes the instruction specified by *instr, modifying the
   machine's state (memory, registers, condition codes, program
   counter) in the process. Returns 1 if the instruction was executed
   successfully, or 0 if there was an error. Typical errors include an
   invalid instruction or a memory access to an invalid address. */
int executeInstruction(machine_state_t *state, y86_instruction_t *instr) {
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
//  uint64_t (*rfpointer)[16] = &state->registerFile;
  int i;
  switch (instr->icode) {
    case I_HALT: // halt
        return 1;
        break;
    case I_NOP: // nop, no operation
        state->programCounter = instr->valP;
        return 1;
    case I_RRMVXX: // rrmovq with 0 ifun, cmovXX with 1-6
//        uint64_t *rfpointer = state->registerFile;
        executeMOV(instr->ifun, instr->rA, instr->rB, state);
        state->programCounter = instr->valP;
        return 1;
    case I_IRMOVQ: // irmovq
        state->registerFile[instr->rB] = instr->valC;
        state->programCounter = instr->valP;
        return 1;
    case I_RMMOVQ: // rmmovq ra, D(rB) register to memory with offset
//        uint64_t valB = state->registerFile[instr->rB];
//        state->programMap[valB + instr->valC] = state->registerFile[instr->rA];
        state->programCounter = instr->valP;
        return memWriteQuadLE(state, state->registerFile[instr->rB] + instr->valC, state->registerFile[instr->rA]);
    case I_MRMOVQ: // mrmovq D(rA), rB memory with offset to register
//        uint64_t valA = state->registerFile[instr->rA];
//        state->registerFile[instr->rB] = state->programMap[valA + instr->valC];
        state->programCounter = instr->valP;
        return memReadQuadLE(state, state->registerFile[instr->rA] + instr->valC, &state->registerFile[instr->rB]);
    case I_OPQ:
//        uint64_t *rfpointer = state->registerFile;
        state->conditionCodes = executeOPq(instr->ifun, instr->rA, instr->rB, state);
        state->programCounter = instr->valP;
        return 1;
    case I_JXX:
        executeJXX(instr->ifun, instr->valC, state);
        if (state->programCounter == instr->location) {
            state->programCounter = instr->valP;
        }
        return 1;
    case I_CALL: // %rsp = 0x4
        state->registerFile[0x4] -= 0x8;
//        state->programMap[state->registerFile[0x4]] = state->programCounter;
        i = memWriteQuadLE(state, state->registerFile[0x4], state->programCounter);
        if (i == 1 && instr->valC < state->programSize && instr->valC >= 0) {
          state->programCounter = instr->valC;
        } else {
          i = 0;
        }
        return i;
    case I_RET: // popq PC
//        state->programCounter = state->programMap[state->registerFile[0x4]];
        i = memReadQuadLE(state, state->registerFile[0x4], &state->programCounter);
        state->registerFile[0x4] += 0x8;
        if (i == 1 && (state->programCounter >= state->programSize || state->programCounter < 0)) {
          i = 0;
        }
        return i;
    case I_PUSHQ: // pushq rA
        state->registerFile[0x4] -= 0x8;
//        state->programMap[state->registerFile[0x4]] = state->registerFile[instr->rA];
        i = memWriteQuadLE(state, state->registerFile[0x4], state->registerFile[instr->rA]);
        state->programCounter = instr->valP;
        return i;
    case I_POPQ: // popq rA
//        state->registerFile[instr->rA] = state->programMap[state->registerFile[0x4]];
        i = memReadQuadLE(state, state->registerFile[0x4], &state->registerFile[instr->rA]);
        state->registerFile[0x4] += 0x8;
        state->programCounter = instr->valP;
        return i;
    case I_INVALID:
        state->programCounter = instr->valP;
        return 0;
    case I_TOO_SHORT:
        state->programCounter = instr->valP;
        return 0;
    default:
        return 0;
  }
}

// executes MOV functions for RRMOV AND CMOV and returns rB
int executeMOV(uint8_t ifun, y86_register_t rA, y86_register_t rB, machine_state_t *state) {
    switch (ifun) {
        case 0x0: //rrmovq
            state->registerFile[rB] = state->registerFile[rA];
            break;
        case 0x1: // le
            if (state->conditionCodes == 1 || state->conditionCodes == 2) {
                state->registerFile[rB] = state->registerFile[rA];
            }
            break;
        case 0x2: // l
            if (state->conditionCodes == 2) {
                state->registerFile[rB] = state->registerFile[rA];
            }
            break;
        case 0x3: // e
            if (state->conditionCodes == 1) {
                state->registerFile[rB] = state->registerFile[rA];
            }
            break;
        case 0x4: // ne
            if (state->conditionCodes != 1) {
                state->registerFile[rB] = state->registerFile[rA];
            }
            break;
        case 0x5: // ge
            if (state->conditionCodes == 1 || state->conditionCodes != 2) {
                state->registerFile[rB] = state->registerFile[rA];
            }
            break;
        case 0x6: // g
            if (state->conditionCodes != 1 && state->conditionCodes != 2) {
                state->registerFile[rB] = state->registerFile[rA];
            }
            break;
        default:
            return 0;
    }
    return 1;
}

// executes OPq functions for ifun 0-6, returns updated condition codes
uint8_t executeOPq(uint8_t ifun, y86_register_t rA, y86_register_t rB, machine_state_t* state) {
    switch (ifun) {
        case 0x0:
            state->registerFile[rB] += state->registerFile[rA];
            break;
        case 0x1:
            state->registerFile[rB] -= state->registerFile[rA];
            break;
        case 0x2:
            state->registerFile[rB] &= state->registerFile[rA];
            break;
        case 0x3:
            state->registerFile[rB] ^= state->registerFile[rA];
            break;
        case 0x4:
            state->registerFile[rB] *= state->registerFile[rA];
            break;
        case 0x5:
            state->registerFile[rB] /= state->registerFile[rA];
            break;
        case 0x6:
            state->registerFile[rB] %= state->registerFile[rA];
            break;
        default:
            break;
    }
    if (state->registerFile[rB] == 0) {
        state->conditionCodes = 0x0;
    } else if (state->registerFile[rB] < 0) {
        state->conditionCodes = 0x2;
    } // add carry and overflow cases if possible?
    return state->conditionCodes;
}

int executeJXX(uint8_t ifun, uint64_t valC, machine_state_t* state) {
    switch (ifun) {
        case 0x0:
            state->programCounter = valC;
            break;
        case 0x1:
            if (state->conditionCodes == 1 || state->conditionCodes == 2) {
                state->programCounter = valC;
            }
            break;
        case 0x2: // l
            if (state->conditionCodes == 2) {
                state->programCounter = valC;
            }
            break;
        case 0x3: // e
            if (state->conditionCodes == 1) {
                state->programCounter = valC;
            }
            break;
        case 0x4: // ne
            if (state->conditionCodes != 1) {
                state->programCounter = valC;
            }
            break;
        case 0x5: // ge
            if (state->conditionCodes == 1 || state->conditionCodes != 2) {
                state->programCounter = valC;
            }
            break;
        case 0x6: // g
            if (state->conditionCodes != 1 && state->conditionCodes != 2) {
                state->programCounter = valC;
            }
            break;
         default:
            return 0;
    }
    return state->programCounter < state->programSize && state->programCounter >= 0;
}

//returns the upper 4 bits as 8 bit 
//11110000
uint8_t getUpperNib(uint8_t a){
  uint8_t mask1 = 0x0F;
  a = a >> 4;
  a = a & mask1;
  return a;
  
}
//returns the lower 4 bits as 8 bit 
//00001111
uint8_t getLowerNib(uint8_t a){
  uint8_t mask = 0xF;
  a = a & mask; 
  return a;
}

bool requireReg(uint8_t icode){
  if(icode == 0x2 || icode == 0x3 || icode == 0x4 || icode == 0x5 || icode == 0x6 || icode == 0xA ||icode == 0xB){
    return true;
  }else{
    return false;
  }
}

bool requireValC(uint8_t icode){
  if(icode == 0x3 || icode == 0x4 || icode == 0x5 || icode == 0x7 || icode == 0x8){
    return true;
  }else{
    return false;
  }
}

/* Makes one word (16-bit integer) from two 8-bit integers.
*/
int makeOneWord(uint8_t int1, uint8_t int2, uint16_t *word) {
  *word = ((uint16_t)int2 << 8) | int1;
  return 0;
}


int determineSize(uint8_t icode){
  switch(icode){
    case 0x0:
      return 1;
    case 0x1:
      return 1;
    case 0x2: 
      return 2;
    case 0x3:
      return 10;
    case 0x4:
      return 10;
    case 0x5:
      return 10;
    case 0x6:
      return 2;
    case 0x7:
      return 9;
    case 0x8:
      return 9;
    case 0x9:
      return 1;
    case 0xA:
      return 2;
    case 0xB:
      return 2;
    default: 
      return 0;
  }
}

int determineValC(machine_state_t *state, uint64_t* ret, uint8_t icode){


  switch(icode){
    case 0x0:
      return 2;
    case 0x1:
      return 2;
    case 0x2: 
      return 2;
    case 0x3:
      return memReadQuadLE(state, state->programCounter + 2, ret);
    case 0x4:
      return memReadQuadLE(state, state->programCounter + 2, ret);
    case 0x5:
      return memReadQuadLE(state, state->programCounter + 2, ret);
    case 0x6:
      return 2;
    case 0x7:
      return memReadQuadLE(state, state->programCounter + 1, ret);
    case 0x8:
      return memReadQuadLE(state, state->programCounter + 1, ret);
    case 0x9:
      return 2;
    case 0xA:
      return 2;
    case 0xB:
      return 2;
    default: 
      return 2;
  }
}

uint8_t determineRegA(uint8_t icode, uint8_t reg){
  switch(icode){
    case 0x0:
      return 0xF;
    case 0x1:
      return 0xF;
    case 0x2: 
      return getUpperNib(reg);
    case 0x3:
      return getUpperNib(reg);
    case 0x4:
      return getUpperNib(reg);
    case 0x5:
      return getUpperNib(reg);
    case 0x6:
      return getUpperNib(reg);
    case 0x7:
      return 0xF;
    case 0x8:
      return 0xF;
    case 0x9:
      return 0xF;
    case 0xA:
      return getUpperNib(reg);
    case 0xB:
      return getUpperNib(reg);
    default: 
      return 0xF;
  }
}

uint8_t determineRegB(uint8_t icode, uint8_t reg){
  switch(icode){
    case 0x0:
      return 0xF;
    case 0x1:
      return 0xF;
    case 0x2: 
      return getLowerNib(reg);
    case 0x3:
      return getLowerNib(reg);
    case 0x4:
      return getLowerNib(reg);
    case 0x5:
      return getLowerNib(reg);
    case 0x6:
      return getLowerNib(reg);
    case 0x7:
      return 0xF;
    case 0x8:
      return 0xF;
    case 0x9:
      return 0xF;
    case 0xA:
      return getLowerNib(reg);
    case 0xB:
      return getLowerNib(reg);
    default: 
      return 0xF;
  }
}