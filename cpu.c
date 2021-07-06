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

int cpu(struct program_t *prog, unsigned char *buf, int len){
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
        debug("mov r%u, r%u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] = regs[buf[ip+2]];
        ip += 3;
        break;
      case MOV_REG_VAL:
        debug("mov r%u, 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] = buf[ip+2];
        ip += 3;
        break;
      case ADD_REG_REG:
        debug("add r%u, r%u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] += regs[buf[ip+2]];
        ip += 3;
        break;
      case ADD_REG_VAL:
        debug("add r%u, 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] += buf[ip+2];
        ip += 3;
        break;
      case SUB_REG_REG:
        debug("sub r%u, r%u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] -= regs[buf[ip+2]];
        ip += 3;
        break;
      case SUB_REG_VAL:
        debug("sub r%u, %u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] -= buf[ip+2];
        ip += 3;
        break;
      case MUL_REG_REG:
        debug("mul r%u, r%u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] *= regs[buf[ip+2]];
        ip += 3;
        break;
      case MUL_REG_VAL:
        debug("mul r%u, 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] *= buf[ip+2];
        ip += 3;
        break;
      case DIV_REG_REG:
        debug("div r%u, r%u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] /= regs[buf[ip+2]];
        ip += 3;
        break;
      case DIV_REG_VAL:
        debug("div r%u, %u\n", buf[ip+1], buf[ip+2]);
        regs[buf[ip+1]] /= buf[ip+2];
        ip += 3;
        break;
      case JMP_REG:
        debug("jmp r%u\n", buf[ip+1]);
        lr = ip+2;
        ip = regs[buf[ip+1]];
        break;
      case JMP_VAL:
        debug("jmp 0x%02hhx\n", buf[ip+1]);
        lr = ip+2;
        ip = buf[ip+1];
        break;
      case CMP_REG_REG:
        debug("cmp r%u, r%u\n", buf[ip+1], buf[ip+2]);
        if(regs[buf[ip+1]] == regs[buf[ip+2]]) fr = FLAG_EQUAL;
        else if(regs[buf[ip+1]] > regs[buf[ip+2]]) fr = FLAG_GREATER;
        else if(regs[buf[ip+1]] < regs[buf[ip+2]]) fr = FLAG_LESS;
        ip += 3;
        break;
      case CMP_REG_VAL:
        debug("cmp r%u, 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        if(regs[buf[ip+1]] == buf[ip+2]) fr = FLAG_EQUAL;
        else if(regs[buf[ip+1]] > buf[ip+2]) fr = FLAG_GREATER;
        else if(regs[buf[ip+1]] < buf[ip+2]) fr = FLAG_LESS;
        ip += 3;
        break;
      case JE_REG:
        debug("je r%u\n", buf[ip+1]);
        if(fr == FLAG_EQUAL){
          lr = ip+2;
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JE_VAL:
        debug("je 0x%02hhx\n", buf[ip+1]);
        if(fr == FLAG_EQUAL){
          lr = ip+2;
          ip = buf[ip+1];
        } else ip += 2;
        break;
      case JNE_REG:
        debug("jne r%u\n", buf[ip+1]);
        if(fr != FLAG_EQUAL){
          lr = ip+2;
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JNE_VAL:
        debug("jne 0x%02hhx\n", buf[ip+1]);
        if(fr != FLAG_EQUAL){
          lr = ip+2;
          ip = buf[ip+1];
        } else ip += 2;
        break;
      case JGT_REG:
        debug("jgt r%u\n", buf[ip+1]);
        if(fr == FLAG_GREATER){
          lr = ip+2;
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JGT_VAL:
        debug("jgt 0x%02hhx\n", buf[ip+1]);
        if(fr == FLAG_GREATER){
          lr = ip+2;
          ip = buf[ip+1];
        } else ip += 2;
        break;
      case JLT_REG:
        debug("jlt r%u\n", buf[ip+1]);
        if(fr == FLAG_LESS){
          lr = ip+2;
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JLT_VAL:
        debug("jlt 0x%02hhx\n", buf[ip+1]);
        if(fr == FLAG_LESS){
          lr = ip+2;
          ip = buf[ip+1];
        } else ip += 2;
        break;
      case RET:
        debug("ret\n", 0);
        ip = lr;
        break;
      case END:
        debug("end\n", 0);
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
  unsigned char *buf;
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

  printf("Executed with return value %u.\n", cpu(prog, buf, len));

  free(buf);

  return 0;
}
