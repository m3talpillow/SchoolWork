// Jared Johansson and Tyler Cudnohufsky
// Has all functions needed to implement pmachine.c

#ifndef PM_H_
#define PM_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "compiler.h"


//Prototypes
void fetch(void);
void execute(void);
void printLine(char[]);
int findBase(int);
void printStack(void);
void printReg(void);

FILE *ofp;

short PMACHINE = 0;

// Machine start
int pmachine(void)
{
  char *temp = appendFileName(TEMP_FILE, "-v");
  char *outputFile = attachDir(temp);

  ofp = fopen(outputFile, "w");
  free(temp);
  rx = 0; // verify register point at 0th index

  if (checkFlag(SYSTEM_MESSAGES))
    PMACHINE = 1;

  if (PMACHINE) printf("PMACHINE: Begin virtual machine\n");
  fprintf(ofp, "%3s %3s %3s %3s %3s\t\t%3s %3s\t\t\t%-47s\t\t\t%s\n",
              "PC", "OP", "R", "L", "M", "BP", "SP", "REG", "STACK");

  pc = 0;
  while (!hault)
  {
    // Fetch Cycle - Read in next command
    fetch();

    // Execute Cycle - Do opcode
    execute();
  }

  if (PMACHINE) printf("PMACHINE: End virtual machine\n\n");

  fclose(ofp);
  if (checkFlag(PRINT_VM))
    appendOutput("Virtual Machine:", outputFile);

  if (!checkFlag(KEEP_TEMPS) || !checkFlag(PRINT_VM))
    remove(outputFile);

  free(outputFile);
  return 0;
}

// Fetch instruction and increment pc
void fetch(void)
{
  IR = CODE[pc++];
}

// Huge switch case for op codes
void execute(void)
{
  char i, len, buffer[MAX_BUFFER_SIZE], input = 1;

  switch(IR->op)
  {
    case LIT: // R[i] = M;
      REG[IR->r] = IR->m;
      rx++;
      printLine("LIT");
      break;

    case RTN: // Return from stack level
      sp = bp;
      bp = STACK[sp + 2]; // Retrieve old bp
      pc = STACK[sp + 3]; // Retrieve old pc
      printLine("RTN");
      break;

    case LOD: // R[i] = stack[bp + M];
      REG[IR->r] = STACK[findBase(IR->l) + IR->m];
      rx++;
      printLine("LOD");
      break;

    case STO: // stack[bp + M] = R[i];
      STACK[findBase(IR->l) + IR->m] = REG[IR->r];
      rx--;
      printLine("STO");
      break;

    case CAL: // Call new function
      printLine("CAL");
      STACK[sp + 0] = 0;                         // Return value
      STACK[sp + 1] = (IR->l)?findBase(IR->l):1; // static link
      STACK[sp + 2] = bp;                        // dynamic link
      STACK[sp + 3] = pc;                        // return address
      bp = sp;
      pc = IR->m; // pc = M;
      break;

    case INC: // sp = sp + M;
      sp = sp + IR->m;
      printLine("INC");
      break;

    case JMP: // pc = M;
      printLine("JMP");
      pc = IR->m;
      break;

    case JPC: // check REG[r] == 0, true? pc = M;
      printLine("JPC");
      if (REG[IR->r] == 0)
        pc = IR->m;
      rx--;
      break;

    case SIO: // 3 Parts
      switch (IR->m)
      {
        case 1: // Write (print to console)
          printf("%d\n", REG[IR->r]);
          rx--;
          break;

        case 2: // REG[r] = scanf();
          while (input)
          {
            printf("%s>", SYMBOL_TABLE[IR->l]->name);
            scanf("%s", buffer);
            input = 0;
            len = strlen(buffer);
            for (i = 0; i < len; i++)
              if (!isdigit(buffer[i]))
                input = 1;
          }
          REG[IR->r] = toDigit(buffer);
          break;

        case 3: // Hault the program
          sp = 1;
          hault = 1;
        }
      printLine("SIO");
      break;

    case NEG: // Make negative
      REG[IR->r] = REG[IR->r] * (-1);
      printLine("NEG");
      break;

    case ADD: // R[i] = R[j] + R[k]
      REG[IR->r] = REG[IR->l] + REG[IR->m];
      rx--;
      printLine("ADD");
      break;

    case SUB: // R[i] = R[j] - R[k]
      REG[IR->r] = REG[IR->l] - REG[IR->m];
      rx--;
      printLine("SUB");
      break;

    case MUL: // R[i] = R[j] * R[k]
      REG[IR->r] = REG[IR->l] * REG[IR->m];
      rx--;
      printLine("MUL");
      break;

    case DIV: // R[i] = R[j] / R[k]
      REG[IR->r] = REG[IR->l] / REG[IR->m];
      rx--;
      printLine("DIV");
      break;

    case ODD: // R[i] % 2
      REG[IR->r] = REG[IR->r] % 2;
      rx--;
      printLine("ODD");
      break;

    case MOD: // R[i] = R[j] % R[k]
      REG[IR->r] = REG[IR->l] % REG[IR->m];
      rx--;
      printLine("MOD");
      break;

    case EQL: // R[i] = (R[j] == R[k]
      if (REG[IR->l] == REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      rx--;
      printLine("EQL");
      break;

    case NEQ: // R[i] = (R[j] != R[k])
      if (REG[IR->l] != REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      rx--;
      printLine("NEQ");
      break;

    case LSS: // R[i] = (R[j] < R[k])
      if (REG[IR->l] < REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      rx--;
      printLine("LSS");
      break;

    case LEQ: // R[r] = (R[l] <= R[m])
      if (REG[IR->l] <= REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      rx--;
      printLine("LEQ");
      break;

    case GTR: // R[i] = (R[j] > R[k])
      if (REG[IR->l] > REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      rx--;
      printLine("GTR");
      break;

    case GEQ: // R[i] = (R[j] >= R[k])
      if (REG[IR->l] >= REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      rx--;
      printLine("GEQ");
      break;

    default:
      printf("ERROR: Op code could not be found: %d\n", IR->op);
  }

  if (PMACHINE) printf("PMACHINE: Op code entered: %d\n", IR->op);

  fprintf(ofp, "%3d %3d\t\t\t", bp, sp);
  printReg();
  printStack();
}

// Find base pointer when going down lexigraphical levels
int findBase(int levels)
{
  int oldBase;

  // No descent, current base pointer
  // (minus one for correct addressing)
  if (levels == 0)
    return bp - 1;

  // Descending, retrieve last base pointer from AR
  for (oldBase = bp; levels > 0; levels--)
    oldBase = STACK[oldBase + 2];

  if (PMACHINE) printf("PMACHINE: Base found: %d\n", oldBase);

  return oldBase -1;
}

// Prints current stack
void printStack(void)
{
  int i, prevBase = bp, newBase;

  // Go through entire stack
  for (i = 1; i < sp; i++)
    fprintf(ofp, "%2d ", STACK[i]);
  fprintf(ofp, "\n");

  if (PMACHINE) printf("PMACHINE: Stack printed.\n");
}

// Prints contents of register stack
void printReg(void)
{
  int i;

  // All indexes after rx are not "saved", so show 0 for simplicity
  for (i = 0; i < MAX_REG_SIZE; i++)
    fprintf(ofp, "%2d ", (i < rx) ? REG[i] : 0);
  fprintf(ofp, "\t\t");

  if (PMACHINE) printf("PMACHINE: Registers printed.\n");
}

// Easy print function
void printLine(char s[])
{
  fprintf(ofp, "%3d %3s %3d %3d %3d\t\t", pc - 1, s, IR->r, IR->l, IR->m);
}

#endif // PM_H_
