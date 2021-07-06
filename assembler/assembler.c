/*
 * assembler.c: assembler for an imaginary CPU
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include "../opcodes.h"

struct label_t {
  char *name;
  char pos;
  int len;
};

int main(int argc, char **argv){
  FILE *fp, *fout;
  char buf[512],
       inst[16],
       arg1_s[16],
       arg2_s[16],
       outname[32],
       ip = sizeof(struct program_t),
       *tmp;
  int i, n,
      lind = 0,
      line = 0,
      arg1,
      arg2,
      arg1_t,
      arg2_t,
      found_end = 0;
  struct program_t prog;
  struct label_t labels[256];

  if(argc < 2){
    printf("Usage: assembler [file.S]\n");
    return 1;
  }

  sprintf(outname, "%s.bin", argv[1]);
  fout = fopen(outname, "w");

  fseek(fout, sizeof(struct program_t), SEEK_SET);

  /* First pass: Find label definitions */
  fp = fopen(argv[1], "r");
  while(fgets(buf, sizeof(buf), fp) != NULL){
    if((tmp=strchr(buf, ';')) != NULL){
      tmp[0] = '\0';
    }
    n = sscanf(buf, "%s %s %s", &inst, &arg1_s, &arg2_s);
    if(n <= 0) continue;

    if(inst[0] == '@'){
      labels[lind].name = strdup(buf); /* TODO: This leaks memory */
      labels[lind].len = strlen(buf)-2;
      labels[lind].pos = ip;
      labels[lind].name[labels[lind].len] = '\0';
      lind++;
    } else ip += n;
  }

  /* Second pass: Assemble */
  fseek(fp, 0, SEEK_SET);
  while(fgets(buf, sizeof(buf), fp) != NULL){
    line++;
    arg1_t = NONE;
    arg2_t = NONE;
    memset(arg1_s, 0, sizeof(arg1_s));
    memset(arg2_s, 0, sizeof(arg2_s));

    if((tmp=strchr(buf, ';')) != NULL){
      tmp[0] = '\0';
    }

    n = sscanf(buf, "%s %s %s", &inst, &arg1_s, &arg2_s);
   
    if(inst[0] == '@' || n <= 0) continue;

    if(arg1_s[0] == '@'){
      for(i=0;i<LENGTH(labels);i++){
        if(strncmp(labels[i].name, arg1_s, labels[i].len) == 0){
          arg1_t = VAL;
          arg1 = labels[i].pos;
          goto write;
        }
      }
      printf("Error: Unable to find label \"%s\" referenced on line %i.\n", arg1_s, line);
      fclose(fp);
      fclose(fout);
      exit(1);
    }

    arg1_t = (n == 1 ? NONE : (arg1_s[0] == 'r' ? REG : VAL));
    arg2_t = (n == 1 || n == 2 ? NONE : (arg2_s[0] == 'r' ? REG : VAL));

    sscanf(arg1_s, (arg1_t == REG ? "r%i" : "%i"), &arg1);
    sscanf(arg2_s, (arg2_t == REG ? "r%i" : "%i"), &arg2);

write:;
    for(i=0;i<LENGTH(instructions);i++){
      if(instructions[i].arg1 == arg1_t &&
         instructions[i].arg2 == arg2_t &&
         strcasecmp(instructions[i].inst, inst) == 0){
        fputc(instructions[i].opcode, fout);
        if(arg1_t != NONE) fputc(arg1, fout);
        if(arg2_t != NONE) fputc(arg2, fout);

        if(instructions[i].opcode == END) found_end = 1;
        break;
      }
    }
  }

  if(!found_end) printf("Warning: Did not find end instruction in program, execution may continue indefinitely.\n");

  fseek(fout, 0, SEEK_SET);
  prog.text_start = 0;
  prog.text_length = 0;
  prog.data_start = 0;
  prog.data_length = 0;
  for(i=0;i<lind;i++){
    if(strncmp(labels[i].name, "@start", labels[i].len) == 0){
      prog.entry_point = labels[i].pos;
      fwrite(&prog, sizeof(struct program_t), 1, fout);
      goto found_start;
    }
  }

  printf("Warning: Did not find @start label, defaulting entry point to 0x05.\n");
  prog.entry_point = 0x05;
  fwrite(&prog, sizeof(struct program_t), 1, fout);

found_start:;
  printf("Successfully assembled %s.bin.\n", argv[1]);
  fclose(fp);
  fclose(fout);

  return 0;
}
