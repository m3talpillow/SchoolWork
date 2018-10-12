// Jared Johansson and Tyler Cudnohufsky
// File to hold all global variables for Console.c operations

#ifndef COMP_H_
#define COMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/******************************************************************************/
/*                          PROGRAM GLOBALS                                   */
/******************************************************************************/

// Global Strings
char *DIR_CURRENT;              // Current file directory
char *DIR_INPUT;                // Directory of input file
char *DIR_OUTPUTS;              // Directory for output files
char *DIR_MASTER_OUT;           // Directory of master output file
char *TEMP_FILE;                // Combination of input file and TEMPFILE
char TEMPFILE[] = "-TEMP";      // Addition to temp files

// Directory buffer
#define MAX_BUFFER_SIZE 1024

// Boolean flags
int FLAGS = 0;
typedef enum
{
  PRINT_FILE,         // 0
  PRINT_LEXEME,
  PRINT_LEXEME_TABLE,
  PRINT_ASSEMBLY,
  PRINT_VM,
  KEEP_TEMPS,        // 5
  SYSTEM_MESSAGES
} Flags;




/******************************************************************************/
/*                          SCANNER GLOBALS                                   */
/******************************************************************************/

#define NUM_RESERVED_WORDS 19     // Number of reserved words
#define NUM_RESERVED_SYM 19       // Number of reserved symbols
#define NUM_TOKENS 39             // Number of unique tokens
#define MAX_ID_SIZE 11            // Max length of variable names
#define MAX_LEXLIST_SIZE 2000     // Size of lexList

// Token values
typedef enum
{
  nulsym = 1,  identsym,    numbersym,   // 3
  plussym,     minussym,    multsym,     // 6
  slashsym,    oddsym,      eqlsym,      // 9
  neqsym,      lessym,      leqsym,      // 12
  gtrsym,      geqsym,      lparentsym,  // 15
  rparentsym,  commasym,    semicolonsym,// 18
  periodsym,   becomessym,  beginsym,    // 21
  endsym,      ifsym,       thensym,     // 24
  whilesym,    dosym,       callsym,     // 27
  constsym,    varsym,      procsym,     // 30
  writesym,    readsym,     elsesym,     // 33
  returnsym,   typesym,     intsym,      // 36
  boolsym,     arraysym,    end          // 39
} Token;
// Token identities
char TOKEN_NAMES[NUM_TOKENS + 1][13] = { "NULL",
  "nulsym", "identsym", "numbersym", "plussym", "minussym",
  "multsym", "slashsym", "oddsym", "eqlsym", "neqsym", "lessym",
  "leqsym", "gtrsym", "geqsym", "lparentsym", "rparentsym",
  "commasym","semicolonsym", "periodsym", "becomessym", "beginsym",
  "endsym", "ifsym", "thensym", "whilesym", "dosym", "callsym",
  "constsym", "varsym", "procsym", "writesym", "readsym", "elsesym",
  "returnsym", "typesym", "intsym", "boolsym", "arraysym", "end" };

// Specially reserved words
char RESERVED_WORD[NUM_RESERVED_WORDS][10] = {
  "const", "var", "procedure", "call", "begin", "end", "if",
  "then", "else", "while", "do", "read", "write", "odd", "null",
  "return", "int", "bool", "array" };
// Links reserved word to its token value
int RW_VAL[NUM_RESERVED_WORDS] = {
  constsym, varsym, procsym, callsym, beginsym, endsym, ifsym,
  thensym, elsesym, whilesym, dosym, readsym, writesym, oddsym, nulsym,
  returnsym, intsym, boolsym, arraysym };

// Specially reserved symbols
char RESERVED_SYM[NUM_RESERVED_SYM][3] = {
  "+", "-", "*", "/", "(", ")", "=", ",", ".", "<", "<=", "<>",
  ">", ">=", ";", ":", ":=", "?", "!" };
// Links to symbol token value
int RS_VAL[NUM_RESERVED_SYM] = {
  plussym, minussym, multsym, slashsym, lparentsym, rparentsym,
  eqlsym, commasym, periodsym, lessym, leqsym, neqsym, gtrsym,
  geqsym, semicolonsym, typesym, becomessym, readsym, writesym };

// Hold all lexeme info
typedef struct
{
  char name[MAX_ID_SIZE + 1];
  int tokenID;
  int value;
} Lexeme;

// Lexeme storage
int lx = 0;   // Index to travel lexList
Lexeme *lexList[MAX_LEXLIST_SIZE] = {NULL};




/******************************************************************************/
/*                         PARSER/ICG GLOBALS                                 */
/******************************************************************************/

#define MAX_DIGIT_SIZE 5          // Max length of digits
#define MAX_SYMBOL_TABLE_SIZE 500 // Symbol table size
#define MAX_CODE_SIZE 500         // Max lines of code
#define MAX_LEXI_LEVELS 300       // Max levels able to create/ descend
#define MAX_REG_SIZE 16           // Number of registers

// Symbol type
typedef enum
{
  CONST = 1, VAR, PROC, INT, BOOL
} SymbolType;

// Symbol Table
typedef struct
{
  char name[MAX_ID_SIZE + 1];   // name up to 11 chars
  int kind;                     // const = 1, var = 2, proc = 3
  int type;                     // variable type
  int val;                      // number (consts)
  int level;                    // L level
  int addr;                     // M address (vars/proc stack)
} Symbol;

// Instructions from code
typedef struct Instr
{
  int op;           // Operation code
  int r;            // Register
  int l;            // Level or Register
  int m;            // Variable options
} Instr;

int sx = 0;                          // Index to travel symbol table
Symbol* SYMBOL_TABLE[MAX_SYMBOL_TABLE_SIZE] = {NULL};

int cx = 0;                          // Index to travel code
Instr* CODE[MAX_CODE_SIZE] = {NULL}; // Read from ICG output (pmachine in)

int rx = 0;                          // Register pointer
int REG[MAX_REG_SIZE] = {0};         // Virtual registers

// Storage for getNextToken
int token = 0;                       // Token ID
int tokenValue = 0;                  // numsym's value
char tokenName[MAX_ID_SIZE + 1];     // identsym's name




/******************************************************************************/
/*                       VIRTUAL MACHINE GLOBALS                              */
/******************************************************************************/

#define MAX_STACK_SIZE 2000      // Max size of stack

// Enum for op codes
typedef enum name
{
  ZERO, // Used to show we don't care about a parameter
  LIT, RTN, LOD, STO, CAL, INC,  // 6
  JMP, JPC, SIO, NEG, ADD, SUB,  // 12
  MUL, DIV, ODD, MOD, EQL, NEQ,  // 18
  LSS, LEQ, GTR, GEQ,            // 22
  END  // For looping purposes
} opCode;

int STACK[MAX_STACK_SIZE] = {0}; // Virtual stack
int sp = 1;                      // Stack pointer
int bp = 1;                      // Base pointer

Instr* IR;                       // Instruction register
int pc = 0;                      // Program counter
int hault = 0;                   // Hault boolean


// Prototypes
char *appendFileName(char *, char *);
void appendOutput(char *, char *);
void clearOldFiles(char *);
void printFile(char *);
int checkFlag(int);
int toDigit(char *);

// Appends text to end of fileName and returns its address
char *appendFileName(char *fileName, char *append)
{
  char *name;
  int i, j, k, length;

  length = strlen(fileName) + strlen(append) + 1;
  if ((name = malloc(sizeof(char) * length)) == NULL)
    exit(0);

  // Copy file name until '.' reached
  for (i = 0; ((fileName[i] != '.') && fileName[i]); i++)
    name[i] = fileName[i];
  k = i;

  // Append new text
  for (j = 0; append[j]; j++, i++)
    name[i] = append[j];

  // Add file extention
  for (k; fileName[k]; k++, i++)
    name[i] = fileName[k];
  name[i] = '\0';

  return name;
}

// Adds current directory to file
char *attachDir(char *fileName)
{
  int length = strlen(DIR_OUTPUTS);
  char *name;

  length += strlen(fileName) + 1;
  if ((name = malloc(sizeof(char) * length)) == NULL)
    exit(0);

  strcpy(name, DIR_OUTPUTS);
  strcat(name, fileName);

  return name;
}

// Append everything from given file to end of master OUTPUT
void appendOutput(char *title, char *fileName)
{
  char c;
  FILE *mfp = fopen(DIR_MASTER_OUT, "a+"),
       *ifp = fopen(fileName, "r");

  // Add given title
  fprintf(mfp, title);
  fprintf(mfp, "\n");

  // Copys contents of file
  while ((c = fgetc(ifp)) != EOF)
    fputc(c, mfp);
  fprintf(mfp, "\n\n");

  fclose(ifp);
  fclose(mfp);
}

// Clears previous files that would have the same name
void clearOldFiles(char *tag)
{
  char *file, *temp = appendFileName(TEMP_FILE, tag);;
  FILE *fp;

  file = attachDir(temp);
  free(temp);

  // Removes file with tag
  if ((fp = fopen(file, "r")) != NULL)
  {
    fclose(fp);
    remove(file);
  }

  free(file);
}

// Read and spit copy of input file to command
void printFile(char *fileName)
{
  FILE *fp = fopen(fileName, "r");
  char c;

  printf("Input File:\n");
  while ((c = fgetc(fp)) != EOF)
    printf("%c", c);

  fclose(fp);
}

// Checks boolean flag
int checkFlag(int index)
{
  int shifted;

  // isolate target bit
  shifted = FLAGS << (31 - index);
  shifted = shifted >> 31;

  return shifted;
}

// Converts a string to an integer
int toDigit(char *value)
{
  int i, mult = 1, result = 0, len = strlen(value);

  for (i = len - 1; i >= 0; i--)
  {
    result += (value[i] - '0') * mult;
    mult *= 10;
  }

  return result;
}

#endif // COMP_H_
