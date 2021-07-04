/*
 * cpu.c: an interpreter for an imaginary CPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "opcodes.h"

#define IS_DEBUG_BUILD

#ifdef IS_DEBUG_BUILD
#  define debug(str, ...) printf(str, __VA_ARGS__)
#else
#  define debug(str, ...) do {} while(0)
#endif

int cpu(struct program_t *prog, char *buf, int len){
  int i, ret;
  reg_t ip = prog->entry_point,
        lr = ip,
        fr = FLAG_NONE,
        regs[16];
  memset(regs, 0, 16);

  debug("Starting execution at entry point 0x%02hhx.\n\n", prog->entry_point);

  for(;;){
    switch(buf[ip]){
      case MOV_REG_REG:
        debug("MOV r%u r%u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] = regs[buf[ip+2]];
        ip += 3;
        break;
      case MOV_REG_VAL:
        debug("MOV r%u 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] = buf[ip+2];
        ip += 3;
        break;
      case ADD_REG_REG:
        debug("ADD r%u r%u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] += regs[buf[ip+2]];
        ip += 3;
        break;
      case ADD_REG_VAL:
        debug("ADD r%u 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] += buf[ip+2];
        ip += 3;
        break;
      case JMP_REG:
        debug("JMP r%u\n", buf[ip+1]);
        lr = ip+2;
        ip = regs[buf[ip+1]];
        break;
      case JMP_VAL:
        debug("JMP 0x%02hhx\n", buf[ip+1]);
        lr = ip+2;
        ip = buf[ip+1];
        break;
      case CMP_REG_REG:
        debug("CMP r%u r%u\n", buf[ip+1], buf[ip+2]);
        if(regs[buf[ip+1]] == regs[buf[ip+2]]) fr = FLAG_EQUAL;
        else fr = FLAG_NONE;
        ip += 3;
        break;
      case CMP_REG_VAL:
        debug("CMP r%u 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        if(regs[buf[ip+1]] == buf[ip+2]) fr = FLAG_EQUAL;
        else fr = FLAG_NONE;
        ip += 3;
        break;
      case JE_REG:
        debug("JE r%u\n", buf[ip+1]);
        ip += 2;
        if(fr == FLAG_EQUAL){
          lr = ip;
          ip = regs[buf[ip+1]];
        }
        break;
      case JE_VAL:
        debug("JE 0x%02hhx\n", buf[ip+1]);
        ip += 2;
        if(fr == FLAG_EQUAL){
          ip = buf[ip+1];
        }
        break;
      case RET:
        debug("RET\n", 0);
        ip = lr;
        break;
      case END:
        debug("END\n", 0);
        ret = 0;
        goto done;
      default:
        debug("Bad opcode 0x%02hhx.\n", buf[ip]);
        ret = 1;
        goto done;
    }
  }

done:;
  printf("\nRegisters at end of execution:\n");
  for(i=0;i<16;i++){
    printf("  r%u = %02x\n", i, regs[i]);
  }

  return ret;
}

int main(int argc, char **argv){
  FILE *fp;
  char *buf;
  int   len;
  struct program_t *prog;

  if(argc < 2){
    printf("Usage: cpu [program]\n");
    return 1;
  }

  fp = fopen(argv[1], "r");
  fseek(fp, 0, SEEK_END);
  buf = malloc((len=ftell(fp)));
  fseek(fp, 0, SEEK_SET);
  fread(buf, len, 1, fp);
  fclose(fp);

  prog = (struct program_t*)buf;

  debug("Executed with return value %u.\n", cpu(prog, buf, len));

  free(buf);

  return 0;
}
