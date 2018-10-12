// System Software - 2017
// Jared Johansson and Tyler Cudnohufsky

#include <stdio.h>
#include <stdlib.h>

typedef struct Instr{
  int op; // Operation code
  int r;  // Register
  int l;  // Level or Register
  int m;  // Variable options
} Instr;

// Constants
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define MAX_REG 16

// Memory, stack, and cache
Instr* CODE[MAX_CODE_LENGTH] = {NULL};
int STACK[MAX_STACK_HEIGHT] = {0};
int REG[MAX_REG] = {0};

// Stack pointers
int sp = 0;
int bp = 1;
int pc = 0;
Instr* IR; // IR.OP, IR.R, IR.L, IR.M
int hault = 0;
int answer = 0;

// Enums for op codes
typedef enum
{
  NUL, // Null, not to be used
  LIT, RTN, LOD,
  STO, CAL, INC,
  JMP, JPC, SIO,
  NEG, ADD, SUB,
  MUL, DIV, ODD,
  MOD, EQL, NEQ,
  LSS, LEQ, GTR,
  GEQ
} opCode;

//Prototypes
void readLines(void);
void fetch(void);
void execute(void);
void printLine(char[]);
int base(int, int);
void printStack(void);


// Machine start
int main(void)
{
  // Read in all instructions
  readLines();
  printf("Inital Values    pc bp sp Stack\n");

  while(!hault)
  {
    // Fetch Cycle - Read in next command
    fetch();

    // Execute Cycle - Do opcode
    execute();
  }
  printf("Factorial Output: %d\n", answer);

  return 0;
}

void execute(void)
{
  switch(IR->op)
  {
    case LIT: // R[i] = M;
      REG[IR->r] = IR->m;
      printLine("LIT");
      break;

    case RTN: // Return from stack level
      printLine("RTN");
      sp = bp-1;
      pc = STACK[sp + 4]; // Retrieve old pc
      bp = STACK[sp + 3]; // Retrieve old bp

      break;

    case LOD: // R[i] = stack[base(L, bp) + M];
      //printf("IR->l = %d\n", IR->l);
      REG[IR->r] = STACK[base(IR->l, bp) + IR->m];
      printLine("LOD");
      break;

    case STO: // stack[base(L, bp) + M] = R[i];
      STACK[base(IR->l, bp) + IR->m] = REG[IR->r];
      printLine("STO");
      break;

    case CAL: // Call new function
      printLine("CAL");
      STACK[sp + 1] = 0;
      STACK[sp + 2] = base(IR->l, bp); // base(L, bp);
      STACK[sp + 3] = bp;
      STACK[sp + 4] = pc;
      bp = sp + 1;
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

    case JPC: // check REG[i] == 0, true? pc = M;
      printLine("JPC");
      if(REG[IR->r] == 0)
        pc = IR->m;
      break;

    case SIO: // 3 Parts
      switch (IR->m)
      {
        case 1: // Print
          answer = REG[IR->r];
          break;

        case 2: // Read
          //read(REG[ir.r]);
          break;

        case 3: // Hault
          sp = 0;
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
      printLine("ADD");
      break;

    case SUB: // R[i] = R[j] - R[k]
      REG[IR->r] = REG[IR->l] - REG[IR->m];
      printLine("SUB");
      break;

    case MUL: // R[i] = R[j] * R[k]
      REG[IR->r] = REG[IR->l] * REG[IR->m];
      printLine("MUL");
      break;

    case DIV: // R[i] = R[j] / R[k]
      REG[IR->r] = REG[IR->l] / REG[IR->m];
      printLine("DIV");
      break;

    case ODD: // R[i] % 2
      REG[IR->r] = REG[IR->r] % 2;
      printLine("ODD");
      break;

    case MOD: // R[i] = R[j] % R[k]
      REG[IR->r] = REG[IR->l] % REG[IR->m];
      printLine("MOD");
      break;

    case EQL: // R[i] = (R[j] == R[k]
      if (REG[IR->l] == REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      printLine("EQL");
      break;

    case NEQ: // R[i] = (R[j] != R[k])
      if (REG[IR->l] != REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      printLine("NEQ");
      break;

    case LSS: // R[i] = (R[j] < R[k])
      if (REG[IR->l] < REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      printLine("LSS");
      break;

    case LEQ: // R[i] = (R[j] <= R[k])
      if (REG[IR->l] <= REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      printLine("LEQ");
      break;

    case GTR: // R[i] = (R[j] > R[k])
      if (REG[IR->l] > REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      printLine("GTR");
      break;

    case GEQ: // R[i] = (R[j] >= R[k])
      if (REG[IR->l] >= REG[IR->m])
        REG[IR->r] = 1;
      else
        REG[IR->r] = 0;
      printLine("GEQ");
      break;
  }
  printf("%2d %2d %2d ", pc, bp, sp);
  printStack();
}

int base(int lvl, int bp)
{
  if (lvl > MAX_LEXI_LEVELS)
    lvl = MAX_LEXI_LEVELS;

  int b1 = bp, hi = 0;
  while (lvl > 0)
  {
    hi = STACK[b1+1];
    //printf("Base(%d, bp: %d) = Stack[%d] = %d\n", lvl, bp, b1+2, hi);
    b1 = hi;
    lvl--;
  }

  return b1;
}

// Prints current stack
void printStack(void)
{
  int i = 1, b = bp;
  int jumps[3] = {0};

  jumps[0] = b;
  while(b != 1)
  {
    b = base(1, b+1);
    jumps[i] = b;
    i++;
  }

  i = 1;
  while (i <= sp)
  {
    if (i != 1 && (i == jumps[0] || i == jumps[1] || i == jumps[2]))
      printf("|");

    printf("%2d ", STACK[i]);
    i++;
  }
  printf("\n");
  printf("RF: ");
  for(i = 0; i < 16; i++)
  //prints contents of all registers after every line
  {
      printf("%d", REG[i]);
      if(i != 15)
        printf(", ");
  }
  printf("\n");
}

// Easy print function
void printLine(char s[])
{
  printf("%2d  %s %2d %2d %2d ", pc, s, IR->r, IR->l, IR->m);
}

// Fetch instruction and increment pc
void fetch(void)
{
  IR = CODE[pc];
  pc++;
}

// Read all instructions, place into CODE
void readLines(void)
{
  int i = 0;
  FILE* fp = fopen("pm_input.txt", "r");

  // File format is 4 ints per line
  while (!feof(fp) && i < MAX_CODE_LENGTH)
  {
    // Create new instruction and fill it
    Instr* instr = calloc(1, sizeof(Instr));
    fscanf(fp, "%d %d %d %d ", &instr->op, &instr->r, &instr->l, &instr->m);
    //printf("CODE[%d] = %d %d %d %d\n", i, instr->op, instr->r, instr->l, instr->m);
    // Save instruction in CODE memory
    CODE[i] = instr;
    i++;
  }
  fclose(fp);
}
