#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "instruction.h"
#include "printRoutines.h"

#define ERROR_RETURN -1
#define SUCCESS 0

#define MAX_LINE 256


static void addBreakpoint(uint64_t address, breakPointLL* l);
static void deleteBreakpoint(uint64_t address, breakPointLL* l);
static void deleteAllBreakpoints(breakPointLL* l);
static int  hasBreakpoint(uint64_t address, breakPointLL* l);
bool validIntParam(char * a, uint64_t size);
bool needParams(int a);
bool isEmptyString(char* c);
int eatInput(char* command);

// Will have to have a datastructure to store the locations for the breakpoints. 
// Could be an array of addresses? size would be state.programSize 
// Maybe a linked list of the addresses

// typedef enum input{
//   I_QUIT = 0x0,
//   I_EXIT = 0x1,
//   I_STEP = 0x2,
//   I_RUN = 0x3,
//   I_NEXT = 0X4,
//   //below take param EXCEPT for regis
//   I_JUMP = 0X5,
//   I_BREAK = 0X6,
//   I_DELETE = 0X7,
//   I_REGIS = 0X8,
//   I_EXAM = 0X9,
// } debuggerInputs;


int main(int argc, char **argv) {


  breakPointLL* bpList = malloc(sizeof(breakPointLL));
  bpList->head = NULL;
  bpList->tail = NULL;

  int fd;
  struct stat st;
  
  machine_state_t state;
  y86_instruction_t nextInstruction;
  memset(&state, 0, sizeof(state));

  char line[MAX_LINE + 1], previousLine[MAX_LINE + 1] = "";
  char *command, *parameters;
  int c;

  // Verify that the command line has an appropriate number of
  // arguments
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage: %s InputFilename [startingPC]\n", argv[0]);
    return ERROR_RETURN;
  }

  // First argument is the file to read, attempt to open it for
  // reading and verify that the open did occur.
  fd = open(argv[1], O_RDONLY);

  if (fd < 0) {
    fprintf(stderr, "Failed to open %s: %s\n", argv[1], strerror(errno));
    return ERROR_RETURN;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(stderr, "Failed to stat %s: %s\n", argv[1], strerror(errno));
    close(fd);
    return ERROR_RETURN;
  }

  state.programSize = st.st_size;

  // If there is a 2nd argument present it is an offset so convert it
  // to a numeric value.
  if (3 <= argc) {
    errno = 0;
    state.programCounter = strtoul(argv[2], NULL, 0);
    if (errno != 0) {
      perror("Invalid program counter on command line");
      close(fd);
      return ERROR_RETURN;
    }
    if (state.programCounter > state.programSize) { 
      fprintf(stderr, "Program counter on command line (%lu) "
	      "larger than file size (%lu).\n",
	      state.programCounter, state.programSize);
      close(fd);
      return ERROR_RETURN;     
    }
  }

  // Maps the entire file to memory. This is equivalent to reading the
  // entire file using functions like fread, but the data is only
  // retrieved on demand, i.e., when the specific region of the file
  // is needed.
  state.programMap = mmap(NULL, state.programSize, PROT_READ | PROT_WRITE,
			  MAP_PRIVATE, fd, 0);
  if (state.programMap == MAP_FAILED) {
    fprintf(stderr, "Failed to map %s: %s\n", argv[1], strerror(errno));
    close(fd);
    return ERROR_RETURN;
  }

  // Move to first non-zero byte
  while (!state.programMap[state.programCounter]) state.programCounter++;
  
  printf("# Opened %s, starting PC 0x%lX\n", argv[1], state.programCounter);



  fetchInstruction(&state, &nextInstruction);


  printInstruction(stdout, &nextInstruction);

  int inputCode;
  bool paramReq;
  unsigned long int param;
  int validFlag;
  uint64_t stackPointer;

  while(1) {

    
    // Show prompt, but only if input comes from a terminal
    if (isatty(STDIN_FILENO))
      printf("> ");

    // Read one line, if EOF break loop
    if (!fgets(line, sizeof(line), stdin))
      break;

    // If line could not be read entirely
    if (!strchr(line, '\n')) {
      // Read to the end of the line
      while ((c = fgetc(stdin)) != EOF && c != '\n');
      if (c == '\n') {
	printErrorCommandTooLong(stdout);
	continue;
      }
      else {
	// In this case there is an EOF at the end of a line.
	// Process line as usual.
      }
    }

    // Obtain the command name, separate it from the arguments.
    command = strtok(line, " \t\n\f\r\v");
    // If line is blank, repeat previous command.
    if (!command) {
      strcpy(line, previousLine);
      command = strtok(line, " \t\n\f\r\v");
      // If there's no previous line, do nothing.
      if (!command) continue;
    }

    // Get the arguments to the command, if provided.
    parameters = strtok(NULL, "\n\r");

    sprintf(previousLine, "%s %s\n", command, parameters ? parameters : "");

    /* THIS PART TO BE COMPLETED BY THE STUDENT */

    //take in char* command and translate into a uniform string 
    //all in lower case etc.

    //use string to get a numeric value that is mapped to a specific input 
    //defined in the enumeration above

    //put a switch statement that deals with each case 
    //default case is when the string is not a proper command 

    //have to think about parameters as well 
    // usual

    inputCode = eatInput(command);
    paramReq = needParams(inputCode);
  
    //Checking invalid command
    if(paramReq){
      if(isEmptyString(parameters)){
        inputCode = 10;
      }else{
        param = strtoul(parameters, NULL, 0);
      }
    }else{
      if(!isEmptyString(parameters)){
        inputCode = 10;
      }
    }

    switch(inputCode){
      //I_QUIT
      case 0: 
        deleteAllBreakpoints(bpList);
        munmap(state.programMap, state.programSize);
        close(fd);
        return SUCCESS;
      //I_EXIT
      case 1:
        deleteAllBreakpoints(bpList);
        munmap(state.programMap, state.programSize);
        close(fd);
        return SUCCESS;
      //I_STEP
      case 2:

        validFlag = executeInstruction(&state, &nextInstruction); //NEED TO OUTPUT INSTRUCTION IS INVALID IF IT IS
        if(validFlag == 1){
          fetchInstruction(&state, &nextInstruction);
          printInstruction(stdout, &nextInstruction);
        }else{
          if(nextInstruction.icode == I_TOO_SHORT){
            printErrorShortInstruction(stdout, &nextInstruction);
          }
          if(nextInstruction.icode == I_INVALID){
            printErrorInvalidInstruction(stdout, &nextInstruction);
          }
        }
        break;
      //I_RUN
      case 3:
        while(1){
          if(hasBreakpoint(state.programCounter, bpList)){
            break;
          }
        validFlag = executeInstruction(&state, &nextInstruction); 
          if(validFlag == 1){
            if(nextInstruction.icode == I_HALT){
              break;
            }
            fetchInstruction(&state, &nextInstruction);
            printInstruction(stdout, &nextInstruction);
          }else{
            if(nextInstruction.icode == I_TOO_SHORT){
              printErrorShortInstruction(stdout, &nextInstruction);
              break;
            }
            if(nextInstruction.icode == I_INVALID){
              printErrorInvalidInstruction(stdout, &nextInstruction);
              break;
            }
          }
        }
        break;
      //I_NEXT
      case 4:
      if(nextInstruction.icode == I_CALL){
        stackPointer = state.registerFile[R_RSP];
        while(1){
          if(hasBreakpoint(state.programCounter, bpList)){
            break;
          }
        validFlag = executeInstruction(&state, &nextInstruction); 
          if(validFlag == 1){
            if(nextInstruction.icode == I_HALT){
              break;
            }
            if(state.registerFile[R_RSP] == stackPointer){
              break;
            }
            fetchInstruction(&state, &nextInstruction);
            printInstruction(stdout, &nextInstruction);

          }else{
            if(nextInstruction.icode == I_TOO_SHORT){
              printErrorShortInstruction(stdout, &nextInstruction);
              break;
            }
            if(nextInstruction.icode == I_INVALID){
              printErrorInvalidInstruction(stdout, &nextInstruction);
              break;
            }
          }
        }
      }else{
        validFlag = executeInstruction(&state, &nextInstruction); //NEED TO OUTPUT INSTRUCTION IS INVALID IF IT IS
        if(validFlag == 1){
          if(nextInstruction.icode == I_CALL){

          }
          fetchInstruction(&state, &nextInstruction);
          printInstruction(stdout, &nextInstruction);
        }else{
          if(nextInstruction.icode == I_TOO_SHORT){
            printErrorShortInstruction(stdout, &nextInstruction);
          }
          if(nextInstruction.icode == I_INVALID){
            printErrorInvalidInstruction(stdout, &nextInstruction);
          }
        }
        break;
      }
      //NEED PARAMS
      //I_JUMP
      break;
      case 5:
        state.programCounter = (uint64_t)param;
        fetchInstruction(&state, &nextInstruction);
        printInstruction(stdout, &nextInstruction);
        break;
      //NEED PARAMS
      //I_BREAK
      case 6:
        addBreakpoint((uint64_t)param, bpList);
        break;
      //NEED PARAMS
      //I_DELETE
      case 7:
        deleteBreakpoint((uint64_t)param, bpList);
        break;
      //I_REGIS
      case 8:
        for (y86_register_t rf = R_RAX; rf < R_NONE; rf++) {
            printRegisterValue(stdout, &state, rf);
        }
        break;
      //NEED PARAMS
      //I_EXAM
      case 9:
        printMemoryValueQuad(stdout, &state, (uint64_t)param);
        break;
        //INVALID INSTRUCTION
      default:
        printErrorInvalidCommand(stdout, command, parameters);
    }

    
  }

  deleteAllBreakpoints(bpList);
  munmap(state.programMap, state.programSize);
  close(fd);
  return SUCCESS;
}


// bool validIntParam(char * a, uint64_t size){
//   int val = strcmp(x, t);
//   if(val == 0){
//     return true;
//   }else{
//     val = atoi(a);
//     if(val <= 0 || val >= size){
//       return false;
//     }else{
//       return true;
//     }
//   }
// }


bool needParams(int a){
  if(a == 5 || a == 6 || a == 7 || a == 9){
    return true;
  }else{
    return false;
  }
}

bool isEmptyString(char* c){
  return(c == 0x0);
  // if ((c != NULL) && (c[0] == '\0')) {
  //  return true;
  // }else{
  //   return false;
  // }
}
//
//returns an int that correspond to debuggerInnputs
//issue
int eatInput(char* command){
  char quit[20] = "quit";
  char exit[20] = "exit";
  char step[20] = "step";
  char run[20] = "run";
  char next[20] = "next";

  char jump[20] = "jump";
  char brek[20] = "break";
  char del[20] = "delete";
  char reg[20] = "registers";
  char exam[20] = "examine";

  for (char * t = command; *t != '\0'; t++) {
    *t = tolower(*t);
  }
  //useless comment

  if(strcmp(command, quit) == 0){
    return 0;
  }
  if(strcmp(command, exit) == 0){
    return 1;
  }
  if(strcmp(command, step) == 0){
    return 2;
  }
  if(strcmp(command, run) == 0){
    return 3;
  }
  if(strcmp(command, next) == 0){
    return 4;
  }
  if(strcmp(command, jump) == 0){
    return 5;
  }
  if(strcmp(command, brek) == 0){
    return 6;
  }
  if(strcmp(command, del) == 0){
    return 7;
  }
  if(strcmp(command, reg) == 0){
    return 8;
  }
  if(strcmp(command, exam) == 0){
    return 9;
  }
  return 10;
}


/* Adds an address to the list of breakpoints. If the address is
 * already in the list, it is not added again. */

//assume that the address of the breakpoint is correct
//if head == NULL tail should also 
static void addBreakpoint(uint64_t address, breakPointLL* l) {
  if(hasBreakpoint(address, l)){
    return;
  }
  if(l->head == NULL){
    breakPointNode* t = malloc(sizeof(breakPointNode));
    t->address = address;
    t->next = NULL;
    l->head = t;
    l->tail = t;
    l->size = 1;
  }else{
    breakPointNode* t = malloc(sizeof(breakPointNode));
    t->address = address;
    t->next = NULL;
    l->tail->next = t;
    l->tail = t;
    l->size = l->size + 1;
  }
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
}

/* Deletes an address from the list of breakpoints. If the address is
 * not in the list, nothing happens. */

//loop through link list when you find the node with the address
//reassign some pointers so that the link can be 
static void deleteBreakpoint(uint64_t address, breakPointLL* l) {
  breakPointNode * prev = NULL;
  breakPointNode * t = l->head;
  if(l->size == 0){
    return;
  }
  if(!hasBreakpoint(address, l)){
    return;
  }
  for(int i = 0; i < l->size; i++){
    if(t->address == address){
      if(prev == NULL){
        l->head = t->next;
        free(t);
        t = NULL;
        l->size--;
        return;
      }else if(t == l->tail){
        prev->next = t->next;
        free(t);
        t = NULL;
        l->size--;
        l->tail = prev;
        return;
      }
      else{
        prev->next = t->next;
        free(t);
        t = NULL;
        l->size--;
        return;
      }
    }
    else{
      prev = t;
      t = prev->next;
    }
  }
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
}



/* Deletes and frees all breakpoints. */
static void deleteAllBreakpoints(breakPointLL * l) {
  breakPointNode * curr = l->head;
  breakPointNode * temp = NULL;

  for(int i = 0; i < l->size; i++){
    temp = curr->next;
    free(curr);
    curr = temp;
    l->size--;
  }
  /* THIS PART TO BE COMPLETED BY THE STUDENT */
}

/* Returns true (non-zero) if the address corresponds to a breakpoint
 * in the list of breakpoints, or false (zero) otherwise. */
static int hasBreakpoint(uint64_t address, breakPointLL * l) {
  breakPointNode* curr = l->head;
  for(int i = 0; i < l->size ; i++){
    if(curr->address == address){
      return 1;
    }else{
      curr = curr->next;
    }
  }
  return 0;
}