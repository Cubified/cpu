/*
 * assembler.c: assembler for an imaginary CPU
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <libgen.h>

#include "../opcodes.h"

/*
 * LOGGING
 */
#define warn(msg, ...) printf("%s:%i: \x1b[33mwarning: \x1b[0m" msg "\n", name, line, __VA_ARGS__)
#define err(msg, ...) printf("%s:%i: \x1b[31merror: \x1b[0m" msg" \n", name, line, __VA_ARGS__)

/*
 * ASSEMBLER
 */
struct label_t {
  char name[64];
  int len;
  addr_t pos;
};

int ip = sizeof(struct program_t),
    found_end = 0,
    lind = 0,
    pragmaind = 0;
char *pragma[64];
struct program_t prog;
struct label_t labels[256];

int check_pragma(char *str){
  int i;
  for(i=0;i<pragmaind;i++){
    if(strcmp(pragma[i], str) == 0) return 0;
  }
  return 1;
}

void assemble_file(char *name, FILE *fout){
  FILE *fp;
  char buf[512],
       inst[64],
       arg1_s[64],
       arg2_s[64],
       *tmp;
  int i, n,
      line = 0,
      arg1,
      arg2,
      arg1_t,
      arg2_t;
  
  /* First pass: Find label definitions */
  fp = fopen(name, "r");
  if(fp == NULL){
    err("unable to open file for reading.", 0);
    exit(2);
  }
  while(fgets(buf, sizeof(buf), fp) != NULL){
    line++;

    if((tmp=strchr(buf, ';')) != NULL){
      tmp[0] = '\0';
    }
    n = sscanf(buf, "%s %s %s", inst, arg1_s, arg2_s);
    if(n <= 0) continue;

    if(n == 2 && arg1_s[0] != 'r') n = 3;

    if(inst[0] == '@'){
      strncpy(labels[lind].name, buf, (labels[lind].len=strlen(buf)-2));
      labels[lind].pos = ip;
      labels[lind].name[labels[lind].len] = '\0';
      lind++;
    } else if(inst[0] == '#'){
      if(strcmp(inst, "#include") == 0){
        if(check_pragma(arg1_s)) assemble_file(arg1_s, fout);
      } else if(strcmp(inst, "#pragma") == 0){
        pragma[pragmaind++] = strdup(name);
      } else warn("ignoring unrecognized preprocessor directive \"%s\".", inst);
    } else ip += n;
  }

  /* Second pass: Assemble */
  line = 0;
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

    n = sscanf(buf, "%s %s %s", inst, arg1_s, arg2_s);

    if(inst[0] == '@' || n <= 0 || inst[0] == '#') continue;

    if(arg1_s[0] == '@'){
      for(i=0;i<lind;i++){
        if(strcmp(labels[i].name, arg1_s) == 0){
          arg1_t = VAL;
          arg1 = labels[i].pos;
          goto write;
        }
      }
      err("unable to find referenced label \"%s\".", arg1_s);
      fclose(fp);
      fclose(fout);
      exit(3);
    }

    if(n == 1) arg1_t = NONE;
    else switch(arg1_s[0]){
      case 'r':
        arg1_t = REG;
        break;
      case '#':
        arg1_t = MEM;
        break;
      default:
        arg1_t = VAL;
        break;
    }

    if(n == 1 || n == 2) arg2_t = NONE;
    else switch(arg2_s[0]){
      case 'r':
        arg2_t = REG;
        break;
      case '#':
        arg2_t = MEM;
        break;
      default:
        arg2_t = VAL;
        break;
    }

    sscanf(arg1_s, (arg1_t == REG ? "r%i" : (arg1_t == MEM ? "#%i" : "%i")), &arg1);
    sscanf(arg2_s, (arg2_t == REG ? "r%i" : (arg2_t == MEM ? "#%i" : "%i")), &arg2);

write:;
    for(i=0;i<LENGTH(instructions);i++){
      if(instructions[i].arg1 == arg1_t &&
         instructions[i].arg2 == arg2_t &&
         strcasecmp(instructions[i].inst, inst) == 0){
        fputc(instructions[i].opcode, fout);
        if(arg1_t != NONE){
          if(arg2_t == NONE) fputc(arg1 >> 8, fout);
          fputc(arg1 & 0xff, fout);
        }
        if(arg2_t != NONE) fputc(arg2, fout);

        if(instructions[i].opcode == END) found_end = 1;
        goto success;
      }
    }

    err("invalid opcode/operand combination \"%s %s %s\"", inst, arg1_s, arg2_s);
    exit(4);

success:;
    continue;
  }
}

/*
 * MAIN
 */
int main(int argc, char **argv){
  FILE *fout;
  char outname[32],
       *path, *dir;
  int i;

  if(argc < 2){
    printf("Usage: assembler [file.S]\n");
    return 1;
  }

  path = realpath(argv[1], NULL);
  dir = dirname(path);
  chdir(dir);
  free(path);
  free(dir);

  sprintf(outname, "%s.bin", argv[1]);
  fout = fopen(outname, "w");

  fseek(fout, sizeof(struct program_t), SEEK_SET);
  assemble_file(argv[1], fout);

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

  printf("%s: \x1b[33mwarning: \x1b[0mdid not find @start label, defaulting entry point to 0x0a.\n", argv[1]);
  prog.entry_point = 0x0a;
  fwrite(&prog, sizeof(struct program_t), 1, fout);
  
found_start:;
  fclose(fout);
  if(!found_end) printf("%s: \x1b[33mwarning: \x1b[0mdid not find end instruction in program, execution may continue indefinitely.\n", argv[1]);
  printf("Successfully assembled %s.\n", outname);

  for(i=0;i<pragmaind;i++){
    free(pragma[i]);
  }

  return 0;
}
