/*
 * cpu.c: an interpreter for an imaginary CPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "opcodes.h"

#define debug(str, ...) if(dbg) printf(str, __VA_ARGS__)

#define pack_addr() dest=buf[ip+1];dest<<=8;dest|=buf[ip+2]

struct termios tio, raw;
struct winsize ws;

void paint(addr_t mem[256]){
  int x = (ws.ws_col / 2) - 16,
      y = (ws.ws_row / 2) - 8,
      xi, yi, i = 0;
  
  for(yi=0;yi<16;yi++){
    for(xi=0;xi<32;xi+=2){
      printf("\x1b[%i;%iH\x1b[48;5;%um  ", y+yi, x+xi, mem[i++]);
    }
  }
  puts("");
}

int input(){
  struct timeval tv = {0, 0};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv);
}

int cpu(struct program_t *prog, unsigned char *buf, int len, int dbg){
  int i, ret;
  reg_t fr = FLAG_NONE,
        regs[16];
  addr_t ip = prog->entry_point,
         lr = ip,
         isr = 0,
         mem[256], inp[16],
         dest;

  memset(regs, 0, sizeof(regs));
  memset(mem,  0, sizeof(mem));

  debug("Starting execution at entry point 0x%04hx.\n\n", prog->entry_point);

  for(;;){
    /* TODO: This discards other inputs in the buffer (but does not hang) */
    if(isr > 0 && input() && read(STDIN_FILENO, inp, sizeof(inp)) > 0){
      debug("Got key press: %c\n", inp[0]);
      regs[0] = inp[0];
      lr = ip;
      ip = isr;
    }

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
      case MOV_MEM_REG:
        debug("mov #%02hhx, r%u\n", buf[ip+1], buf[ip+2]);
        mem[regs[buf[ip+1]]] = regs[buf[ip+2]];
        ip += 3;
        break;
      case MOV_MEM_VAL:
        debug("mov #%02hhx, 0x%02hhx\n", buf[ip+1], buf[ip+2]);
        mem[regs[buf[ip+1]]] = buf[ip+2];
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
        ip = regs[buf[ip+1]];
        break;
      case JMP_VAL:
        pack_addr();
        debug("jmp 0x%04hx\n", dest);
        ip = dest;
        break;
      case CMP_REG_REG:
        debug("cmp r%u (%u), r%u\n", buf[ip+1], regs[buf[ip+1]], buf[ip+2]);
        if(regs[buf[ip+1]] == regs[buf[ip+2]]) fr = FLAG_EQUAL;
        else if(regs[buf[ip+1]] > regs[buf[ip+2]]) fr = FLAG_GREATER;
        else if(regs[buf[ip+1]] < regs[buf[ip+2]]) fr = FLAG_LESS;
        ip += 3;
        break;
      case CMP_REG_VAL:
        debug("cmp r%u (%u), 0x%02hhx\n", buf[ip+1], regs[buf[ip+1]], buf[ip+2]);
        if(regs[buf[ip+1]] == buf[ip+2]) fr = FLAG_EQUAL;
        else if(regs[buf[ip+1]] > buf[ip+2]) fr = FLAG_GREATER;
        else if(regs[buf[ip+1]] < buf[ip+2]) fr = FLAG_LESS;
        ip += 3;
        break;
      case JE_REG:
        debug("je r%u\n", buf[ip+1]);
        if(fr == FLAG_EQUAL){
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JE_VAL:
        pack_addr();
        debug("je 0x%04hx\n", dest);
        if(fr == FLAG_EQUAL){
          ip = dest;
        } else ip += 3;
        break;
      case JNE_REG:
        debug("jne r%u\n", buf[ip+1]);
        if(fr != FLAG_EQUAL){
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JNE_VAL:
        pack_addr();
        debug("jne 0x%04hx\n", dest);
        if(fr != FLAG_EQUAL){
          ip = dest;
        } else ip += 3;
        break;
      case JGT_REG:
        debug("jgt r%u\n", buf[ip+1]);
        if(fr == FLAG_GREATER){
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JGT_VAL:
        pack_addr();
        debug("jgt 0x%04hx\n", dest);
        if(fr == FLAG_GREATER){
          ip = dest;
        } else ip += 3;
        break;
      case JLT_REG:
        debug("jlt r%u\n", buf[ip+1]);
        if(fr == FLAG_LESS){
          ip = regs[buf[ip+1]];
        } else ip += 2;
        break;
      case JLT_VAL:
        pack_addr();
        debug("jlt 0x%04hx\n", dest);
        if(fr == FLAG_LESS){
          ip = dest;
        } else ip += 3;
        break;
      case CALL:
        pack_addr();
        debug("call 0x%04hx\n", dest);
        lr = ip+3;
        ip = dest;
        break;
      case RET:
        debug("ret\n", 0);
        ip = lr;
        break;
      case END:
        debug("end\n", 0);
        ret = 0;
        goto done;
      case SLP:
        pack_addr();
        debug("slp 0x%04hx\n", dest);
        usleep(dest*1000);
        ip += 3;
        break;
      case PNT:
        debug("pnt\n", 0);
        if(!dbg) paint(mem);
        ip += 1;
        break;
      case ISR:
        pack_addr();
        debug("isr 0x%04hx\n", dest);
        isr = dest;
        ip += 3;
        break;
      default:
        debug("Bad opcode 0x%02hhx.\n", buf[ip]);
        ret = 1;
        goto done;
    }
  }

done:;
  debug("\nRegisters at end of execution:\n", 0);
  for(i=0;i<16;i++){
    debug("  r%u = %02x\n", i, regs[i]);
  }

  return ret;
}

void gfx(int on){
  if(on){
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    tcgetattr(STDIN_FILENO, &tio);
    raw = tio;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    printf("\x1b[?1049h\x1b[?25l\n");
  } else tcsetattr(STDIN_FILENO, TCSAFLUSH, &tio);
}

int main(int argc, char **argv){
  FILE *fp;
  unsigned char *buf;
  int   len, dbg = (argc > 2);
  struct program_t *prog;

  if(argc < 2){
    printf("Usage: cpu [program] [--debug]\n");
    return 1;
  }

  fp = fopen(argv[1], "r");
  if(fp == NULL){
    printf("Error: Unable to open file \"%s\" for reading.\n", argv[1]);
    return 2;
  }
  fseek(fp, 0, SEEK_END);
  buf = malloc((len=ftell(fp)));
  fseek(fp, 0, SEEK_SET);
  fread(buf, len, 1, fp);
  fclose(fp);

  gfx(1);

  prog = (struct program_t*)buf;
  printf("%sExecuted with return value %u.\n", (dbg ? "" : "\x1b[?1049l\x1b[?25h"), cpu(prog, buf, len, dbg));

  gfx(0);

  free(buf);

  return 0;
}
