/*
 * opcodes.h: CPU opcodes and other constants
 */

#pragma once

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

enum opcodes {
  MOV_REG_REG = 0x01,
  MOV_REG_VAL = 0x02,
  ADD_REG_REG = 0x13,
  ADD_REG_VAL = 0x14,
  SUB_REG_REG = 0x15,
  SUB_REG_VAL = 0x16,
  MUL_REG_REG = 0x17,
  MUL_REG_VAL = 0x18,
  DIV_REG_REG = 0x19,
  DIV_REG_VAL = 0x1a,
  JMP_REG     = 0x25,
  JMP_VAL     = 0x26,
  CMP_REG_REG = 0x37,
  CMP_REG_VAL = 0x38,
  JE_REG      = 0x49,
  JE_VAL      = 0x4a,
  JNE_REG     = 0x4b,
  JNE_VAL     = 0x4c,
  JGT_REG     = 0x4d,
  JGT_VAL     = 0x4e,
  JLT_REG     = 0x4f,
  JLT_VAL     = 0x50,
  RET         = 0x5b,
  END         = 0x60
};

enum flags {
  FLAG_NONE    = 0,
  FLAG_EQUAL   = 1,
  FLAG_GREATER = 2,
  FLAG_LESS    = 3
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
  { SUB_REG_REG, "sub", REG, REG },
  { SUB_REG_VAL, "sub", REG, VAL },
  { MUL_REG_REG, "mul", REG, REG },
  { MUL_REG_VAL, "mul", REG, VAL },
  { DIV_REG_REG, "div", REG, REG },
  { DIV_REG_VAL, "div", REG, VAL },
  { JMP_REG, "jmp", REG, NONE },
  { JMP_VAL, "jmp", VAL, NONE },
  { CMP_REG_REG, "cmp", REG, REG },
  { CMP_REG_VAL, "cmp", REG, VAL },
  { JE_REG, "je", REG, NONE },
  { JE_VAL, "je", VAL, NONE },
  { JNE_REG, "jne", REG, NONE },
  { JNE_VAL, "jne", VAL, NONE },
  { JGT_REG, "jgt", REG, NONE },
  { JGT_VAL, "jgt", VAL, NONE },
  { JLT_REG, "jlt", REG, NONE },
  { JLT_VAL, "jlt", VAL, NONE },
  { RET, "ret", NONE, NONE },
  { END, "end", NONE, NONE }
};
