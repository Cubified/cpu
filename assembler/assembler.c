/*
 * assembler.c: assembler for an imaginary CPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include "../opcodes.h"

int main(int argc, char **argv){
  FILE *fp, *fout;
  char buf[512],
       inst[16],
       outname[32],
       *ptr;
  int i,
      arg1, arg1_t,
      arg2, arg2_t,
      pos;
  struct program_t prog;

  if(argc < 2){
    printf("Usage: assembler [file.S]\n");
    return 1;
  }

  sprintf(outname, "%s.bin", argv[1]);
  fout = fopen(outname, "w");

  prog.text_start = 0;
  prog.text_length = 0;
  prog.data_start = 0;
  prog.data_length = 0;
  prog.entry_point = 0x05;
  fwrite(&prog, sizeof(struct program_t), 1, fout);

  fp = fopen(argv[1], "r");
  while(fgets(buf, sizeof(buf), fp) != NULL){
    ptr = malloc(512);
    memcpy(ptr, buf, sizeof(buf));
    i = 0;
    arg1 = 0;
    arg2 = 0;

    while(*ptr == ' ') ptr++;
    while(*ptr != ' ' && *ptr != '\n' && *ptr != '\0') inst[i++] = *ptr++;
    inst[i] = '\0';
    while(*ptr == ' ') ptr++;
    if(*ptr == '\n' || *ptr == '\0'){
      arg1_t = NONE;
      arg2_t = NONE;
      goto search;
    }
    if(*ptr == 'r'){
      arg1_t = REG;
      ptr++;
    } else arg1_t = VAL;
    sscanf(ptr, "%i%n", &arg1, &pos);
    ptr += pos;
    while(*ptr == ' ' || *ptr == ',') ptr++;
    if(*ptr == '\n' || *ptr == '\0'){
      arg2_t = NONE;
      goto search;
    }
    if(*ptr == 'r'){
      arg2_t = REG;
      ptr++;
    } else arg2_t = VAL;
    sscanf(ptr, "%i", &arg2);

search:;
    for(i=0;i<LENGTH(instructions);i++){
      if(instructions[i].arg1 == arg1_t &&
         instructions[i].arg2 == arg2_t &&
         strcasecmp(instructions[i].inst, inst) == 0){
        fputc(instructions[i].opcode, fout);
        if(arg1_t != NONE) fputc(arg1, fout);
        if(arg2_t != NONE) fputc(arg2, fout);
        break;
      }
    }
  }
  fclose(fp);
  fclose(fout);

  return 0;
}
