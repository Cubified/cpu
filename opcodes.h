/*
 * opcodes.h: CPU opcodes and other constants
 */

#pragma once

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

enum opcodes {
  MOV_REG_REG = 0x01,
  MOV_REG_VAL = 0x02,
  ADD_REG_REG = 0x03,
  ADD_REG_VAL = 0x04,
  JMP_REG     = 0x05,
  JMP_VAL     = 0x06,
  CMP_REG_REG = 0x07,
  CMP_REG_VAL = 0x08,
  JE_REG      = 0x09,
  JE_VAL      = 0x0a,
  RET         = 0x0b,
  END         = 0x0c
};

enum flags {
  FLAG_NONE  = 0,
  FLAG_EQUAL = 1
};

enum args {
  NONE = 0,
  REG  = 1,
  VAL  = 2
};

struct instr_t {
  int opcode;
  char *inst;
  int arg1;
  int arg2;
};

typedef uint8_t reg_t;
typedef uint8_t addr_t;
typedef uint8_t opcode_t;

struct program_t {
  addr_t text_start;
  addr_t text_length;

  addr_t data_start;
  addr_t data_length;

  addr_t entry_point;
};

struct instr_t instructions[] = {
  { MOV_REG_REG, "mov", REG, REG },
  { MOV_REG_VAL, "mov", REG, VAL },
  { ADD_REG_REG, "add", REG, REG },
  { ADD_REG_VAL, "add", REG, VAL },
  { JMP_REG, "jmp", REG, NONE },
  { JMP_VAL, "jmp", VAL, NONE },
  { CMP_REG_REG, "cmp", REG, REG },
  { CMP_REG_VAL, "cmp", REG, VAL },
  { JE_REG, "je", REG, NONE },
  { JE_VAL, "je", VAL, NONE },
  { RET, "ret", NONE, NONE },
  { END, "end", NONE, NONE }
};
