/* This file contains the prototypes and constants needed to use the
   routines defined in printRoutines.c
*/

#ifndef _PRINTROUTINES_H_
#define _PRINTROUTINES_H_

#include <stdio.h>

#include "instruction.h"

int printInstruction(FILE *file, y86_instruction_t *instr);

int printRegisterValue(FILE *file, machine_state_t *state,
		       y86_register_t reg);
int printMemoryValueByte(FILE *file, machine_state_t *state, uint64_t addr);
int printMemoryValueQuad(FILE *file, machine_state_t *state, uint64_t addr);

int printErrorCommandTooLong(FILE *file);
int printErrorInvalidCommand(FILE *file, char *command, char *parameters);
int printErrorInvalidInstruction(FILE *file, y86_instruction_t *instr);
int printErrorShortInstruction(FILE *file, y86_instruction_t *instr);
int printErrorInvalidMemoryLocation(FILE *file, y86_instruction_t *instr,
				    uint64_t address);


#endif /* PRINTROUTINES */
