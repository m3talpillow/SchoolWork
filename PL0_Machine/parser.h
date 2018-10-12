// Jared Johansson and Tyler Cudnohufsky
// File to manage parsing and ICG

#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

// Prototypes
void getNextToken();
void program();
void block(int);
void constdec(int);
void paramBlock(int, int *);
void vardec(int, int);
void addSymbol(char *, int, int, int , int);
int position(char *);
void statement(int);
void call(int);
void condition(int);
void expression(int);
void term(int);
void factor(int);
void addCode(int, int, int, int);
void error(int);
void printAssembly();

// Debug boolean for system prints
short PARSER = 0;
int AR = 0;

// Begins parsing
void parser()
{
  // Print system messages of parser trace
  if (checkFlag(SYSTEM_MESSAGES))
    PARSER = 1;

  // Clear files from older runs
  clearOldFiles("-a");

  // Reset lexList index (first getNextToken will + to 0)
  lx = -1;

  program();
  printf("No errors, program is syntactically correct.\n");
  if (PARSER) printf("\n");

  // if -a tag was used
  if (checkFlag(PRINT_ASSEMBLY))
    printAssembly();
}

// <block> .
void program()
{
  if (PARSER) printf("\nPARSER: %-10s %-5s\n", "Program", "start");

  // loads first lexeme into buffer
  getNextToken();

  // begin main loop
  block(0);

  if (token != periodsym)
    error(9);

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s\n", "Program", "end");
    fflush(stdout);
  }
}

// <constdec> <vardec> <procdec> <statement>
void block(int level)
{
  // Stack space must be 4 min for bp, sp, pc, return value
  // Save the origional indexes for SYMBOL_TABLE and CODE
  int i, space = 4, prevsx = sx, prevcx = cx, params = 0;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Block", "start", level);

  // Do not surpass max depth
  if (level > MAX_LEXI_LEVELS)
    error(28);

  // Jump to reach after procedure if not at main depth
  if (level != 0)
    addCode(JMP, ZERO, ZERO, 0);

  // Parameters found
  if (token == lparentsym)
  {
    getNextToken();
    paramBlock(level, &params);

    if (token != semicolonsym)
      error(5);
    getNextToken();
  }

  // const-declaration, var-declaration, statement
  while (token == constsym || token == varsym || token == procsym)
  {
    // <constdec>
    if (token == constsym)
    {
      getNextToken();

      // Ident always follows const token
      if (token != identsym)
        error(4);

      // Possibly multiple declarations
      while (token == identsym)
      {
        constdec(level);
        // multiple assignments, comma, should expect another identsym
        while (token == commasym)
        {
          getNextToken();
          constdec(level);
        }

        // Should be at end of line, if so pull next token and restart loop
        if (token == semicolonsym)
          getNextToken();
        else
          error(5);
      }
    }

    // <vardec> (add space needed to save variables)
    if (token == varsym)
    {
      getNextToken();

      // Must be identifier
      if (token == identsym)
      {
        // Add space to stack for variable
        space++;
        vardec(level, space + params);

        // Multiple declarations
        while (token == commasym)
        {
          space++;
          getNextToken();
          vardec(level, space + params);
        }

        // End of line
        if (token == semicolonsym)
          getNextToken();
        else
          error(5);
      }
      else
        error(4);
    }

    // <procdec>
    while (token == procsym)
    {
      // Only procedure declaration, must jump passed until needed
      getNextToken();

      if (token == identsym)
      {
        // Funct's name, its a procedure, and it's scope (level),
        // CODE location: add 1 to pass pending jump to be added
        addSymbol(lexList[lx]->name, PROC, ZERO, level, cx + 1);
      }
      else
        error(4);

      getNextToken();
      // Semicolon isn't required if params
      if (token == semicolonsym)
        getNextToken();

      // Begin generating code for funct and params
      block(level + 1);

      // Varify semicolon
      if (token != semicolonsym)
        error(5);
      getNextToken();
    }
  }

  // Reserve space for AR and vars/params
  addCode(INC, ZERO, ZERO, space + params);
  sp += space + params;
  AR = space + params;

  // Recover any left over items on reg stack from params
  if (level != 0)
  {
    for (i = 0; i < params; i++)
    {
      // check if given too many arguements
      addCode(STO, params - i - 1, 0, 4 + (params - i));
    }
  }

  // <statement>
  statement(level);

  // If at main, keep global vars and no jump needed
  if (level != 0)
  {
    // "erase" the local variables from symbol table if not global
    sx = prevsx;
    // Save end of procedure declaration, offset to skip return statement
    CODE[prevcx]->m = cx + 1;
  }

  // if at base level end program, else return from call
  if (level == 0)
    addCode(SIO, ZERO, ZERO, 3);
  else
    addCode(RTN, ZERO, ZERO, ZERO);
  sp -= space + params;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Block", "end", level);
}

// <param-block>
void paramBlock(int level, int *param)
{
  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Params", "start", level);
  if (token == identsym)
  {
    *param = *param + 1;
    // Increase number of parameters found and offset by 3 for AR
    addSymbol(lexList[lx]->name, VAR, ZERO, level, 4 + *param);
    getNextToken();
  }
  else
    error(14);

  // Multiple params
  while (token == commasym)
  {
    getNextToken();
    if (token == identsym)
    {
      *param = *param + 1;
      // Increase number of parameters found and offset by 3 for AR
      addSymbol(lexList[lx]->name, VAR, ZERO, level, 4 + *param);
      getNextToken();
    }
    else
      error(14);
  }

  if (token != rparentsym)
    error(22);
  getNextToken();
  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Params", "found", param);
}

// <ident> := <express> | call <ident> | begin <statement-list> end |
// if <condition> then <statement> | while <condition> do <statement> | e
void statement(int level)
{
  // variable index, condition start index, fix jump index, lexi-level pass
  int vx, condx, skpx, lvlCheck;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Statement", "start", level);

  // <ident> := <expression>
  if (token == identsym)
  {
    // Find location of saved Symbol (variable index)
    vx = position(lexList[lx]->name);

    // if zero, could not be found
    if (vx == 0)
      error(11);

    // Found, but its of type const or procedure
    else if (SYMBOL_TABLE[vx]->kind != 2)
      error(12);

    getNextToken();
    if (token == becomessym)
    {
      getNextToken();
      expression(level);

      // Varify lexigraphical depth
      if ((lvlCheck = level - SYMBOL_TABLE[vx]->level) > MAX_LEXI_LEVELS)
        error(28);

      // Store what we just calculated in expression into var location
      addCode(STO, --rx, lvlCheck, SYMBOL_TABLE[vx]->addr);
    }
    else
      error(13);
  }

  // call <ident>
  else if (token == callsym)
  {
    getNextToken();
    call(level);
  }

  // begin <statement-list> end
  else if (token == beginsym)
  {
    do {
      getNextToken();
      statement(level);
      // Multiple statements
    } while (token == semicolonsym);

    // begin must be followed up with end
    if (token != endsym)
      error(17);
    getNextToken();
  }

  // if <condition> then <statement>
  else if (token == ifsym)
  {
    getNextToken();
    condition(level);

    // if must be followed up with then
    if (token != thensym)
      error(16);

    getNextToken();
    condx = cx; // hold current code index
    addCode(JPC, --rx, ZERO, 0);
    statement(level);
    if (token == elsesym)
    {
      getNextToken();
      int condx2 = cx;

      addCode(JMP, ZERO, ZERO, 0);
      CODE[condx]->m = cx;
      statement(level);
      CODE[condx2]->m = cx;
    }
    else
    {
        CODE[condx]->m = cx;
    }
  }

  // while <condition> do <statement>
  else if (token == whilesym)
  {
    // Where code for condition starts
    condx = cx;

    getNextToken();
    condition(level);

    // Come back to fix jump address if condition fails
    skpx = cx;
    addCode(JPC, --rx, ZERO, 0);

    // while must be followed up with do
    if (token == dosym)
      getNextToken();
    else
      error(18);

    statement(level);
    addCode(JMP, ZERO, ZERO, condx);
    CODE[skpx]->m = cx;
  }

  // write <expression>
  else if (token == writesym)
  {
    getNextToken();
    expression(level);
    addCode(SIO, --rx, ZERO , 1);
  }

  // Read an input
  else if (token == readsym)
  {
    getNextToken();
    if (token != identsym)
      error(14);
    vx = position(lexList[lx]->name);

    if (vx == 0)
      error(11);

    else if (SYMBOL_TABLE[vx]->kind != 2)
      error(12);

    if ((lvlCheck = level - SYMBOL_TABLE[vx]->level) > MAX_LEXI_LEVELS)
      error(28);

    addCode(SIO, rx++, vx, 2);
    addCode(STO, --rx, lvlCheck, SYMBOL_TABLE[vx]->addr);
    getNextToken();
  }

  // Return statement
  else if (token == returnsym)
  {
    getNextToken();
    expression(level);

    // Store result into STACK[bp]
    addCode(STO, --rx, 0, 1);

    // if at base level end program, else return from call
    if (level == 0)
      addCode(SIO, ZERO, ZERO, 3);
    // Only give return if no return is already comming
    else if (lexList[lx]->tokenID != endsym)
      addCode(RTN, ZERO, ZERO, ZERO);
  }

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Statement", "end", level);
}

// <term> | add/sub <term>
void expression(int level)
{
  int prevToken;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Express", "start", level);

  // <term>
  if (token != plussym && token != minussym)
    term(level);

  // add/sub <term>
  while (token == plussym || token == minussym)
  {
    // <term>
    prevToken = token;
    getNextToken();
    term(level);

    // ADD or SUB top two of reg stack and decrement rx by one
    rx--;
    if (prevToken == plussym)
      addCode(ADD, rx - 1 , rx - 1 , rx);
    else
      addCode(SUB, rx - 1, rx - 1, rx);
  }

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s %d\n", "Express", "end", level);
    fflush(stdout);
  }
}

// <factor> | <term> <mult-op> <factor>
void term(int level)
{
  int prevToken;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Term", "start", level);

  factor(level);
  while (token == multsym || token == slashsym)
  {
    prevToken = token;
    getNextToken();
    factor(level);

    // Multiply or divide the top two on reg stack
    rx--;
    if (prevToken == multsym)
      addCode(MUL, rx - 1 , rx - 1 , rx);
    else
      addCode(DIV, rx - 1 , rx - 1 , rx);
  }

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s %d\n", "Term", "end", level);
    fflush(stdout);
  }
}

// <ident> | <number> | ( <expression> ) | <call>
void factor(int level)
{
  int vx, lvlCheck;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Factor", "start", level);

  // <ident>
  if (token == identsym)
  {
    vx = position(lexList[lx]->name);

    if (vx != 0)
    {
      // If const push its value onto stack
      if (SYMBOL_TABLE[vx]->kind == CONST)
        addCode(LIT, rx++, ZERO, SYMBOL_TABLE[vx]->val);

      // If variable push its value onto stack
      else if (SYMBOL_TABLE[vx]->kind == VAR)
      {
        if ((lvlCheck = level - SYMBOL_TABLE[vx]->level) > MAX_LEXI_LEVELS)
          error(28);

        addCode(LOD, rx++, lvlCheck, SYMBOL_TABLE[vx]->addr);
      }
    }
    else
      error(11);

    getNextToken();
  }

  // <number>
  else if (token == numbersym)
  {
    // Push number onto reg stack and increment reg pointer
    addCode(LIT, rx++, ZERO, lexList[lx]->value);
    getNextToken();
  }

  // <expression>
  else if (token == lparentsym)
  {
    getNextToken();
    expression(level);

    if (token == rparentsym)
      getNextToken();
    else
      error(22);
  }

  else if (token == callsym)
  {
    getNextToken();
    call(level);

    // Called from expression, need return value saved in sp + 1
    addCode(LOD, rx++, 0, AR + 1);
  }

  else
    error(14);

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s %d\n", "Factor", "end", level);
    fflush(stdout);
  }
}

// call <ident>
void call(int level)
{
  int vx = 0, lvlCheck = 0;

  if (token == identsym)
  {
    // Find location in ST
    vx = position(lexList[lx]->name);

    if (vx == 0)
      error(11);

    // Must be a procedure
    if (SYMBOL_TABLE[vx]->kind != 3)
      error (15);

    getNextToken();
    // if parameters, push onto reg stack
    if (token == lparentsym)
    {
      getNextToken();
      if (token != identsym)
        error(14);

      // Puts params into reg stack, last top
      expression(level);
      while (token == commasym)
      {
        getNextToken();
        expression(level);
      }

      if (token != rparentsym)
        error(22);
      getNextToken();
    }

    if ((lvlCheck = level - SYMBOL_TABLE[vx]->level) > MAX_LEXI_LEVELS)
      error(28);

    // Add a call to function
    addCode(CAL, ZERO, lvlCheck, SYMBOL_TABLE[vx]->addr);
  }
  else
    error(14);
}

// odd <expression> | <expression> <relation> <expression>
void condition(int level)
{
  int prevToken = 0;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Condition", "start", level);

  // Odd <expression>
  if (token == oddsym)
  {
    getNextToken();
    expression(level);
    // Mod top of stack, save back to top of stack
    addCode(ODD, rx - 1, 0, 0);
  }

  // <expression> <relation> <expression>
  else
  {
    expression(level);

    // token must be one of these: <relation>
    if (token != eqlsym && token != neqsym && token != lessym &&
                token != leqsym && token != gtrsym && token != geqsym)
      error(20);
    else
    {
      prevToken = token;
      getNextToken();
      expression(level);

      // Code for which relation, decrement rx
      rx--;
      switch (prevToken)
      {
        case eqlsym:
          addCode(EQL, rx - 1, rx - 1, rx);
          break;
        case neqsym:
          addCode(NEQ, rx - 1, rx - 1, rx);
          break;
        case lessym:
          addCode(LSS, rx - 1, rx - 1, rx);
          break;
        case leqsym:
          addCode(LEQ, rx - 1, rx - 1, rx);
          break;
        case gtrsym:
          addCode(GTR, rx - 1, rx - 1, rx);
          break;
        case geqsym:
          addCode(GEQ, rx - 1, rx - 1, rx);
          break;
      }
    }
  }

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s %d\n", "Condition", "end", level);
    fflush(stdout);
  }
}

// const <ident> = <number> (multiple)
void constdec(int level)
{
  char *name;
  int val;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Constdec", "start", level);

  // <ident> = <number> (multiple handled by <block>)
  if (token == identsym)
  {
    // Get name from identsym
    name = lexList[lx]->name;
    getNextToken();

    // Should be assigned with becomessym
    if (token == eqlsym)
      error(1);

    // Must assign value to constant
    else if (token == becomessym)
    {
      getNextToken();

      // Assigning a number value
      if (token == numbersym)
      {
        // Get value from numbersym
        val = lexList[lx]->value;

        // Constant's name, the fact its a const, level, and its value
        addSymbol(name, CONST, val, level, ZERO);
        getNextToken();
      }
      else
        error(2);
    }
    else
      error(4);
  }

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s %d\n", "Constdec", "end", level);
    fflush(stdout);
  }
}

// var <ident-list> | e
void vardec(int level, int m)
{
  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "Vardec", "start", level);

  // Variables are not assigned a value when declared
  // <ident-list> multiple handled by <block>)
  if (token == identsym)
  {
    // Var name, its a var, level its at, space location
    addSymbol(lexList[lx]->name, VAR, ZERO, level, m);
    getNextToken();
  }
  else
    error(4);

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s %d\n", "Vardec", "end", level);
    fflush(stdout);
  }
}

// Adds new values into symbol table (on identsym or numbersym)
void addSymbol(char *name, int type, int value, int level, int m)
{
  // Skip 0 index on sym table
  sx++;

  if (PARSER) printf("PARSER: %-10s %-5s %d\n", "addSymbol", "start", level);

  // Create Symbol struct
  if ((SYMBOL_TABLE[sx] = calloc(1, sizeof(Symbol))) == NULL)
    error(26);

  // Saves known name
  strcpy(SYMBOL_TABLE[sx]->name, name);

  // Save type
  SYMBOL_TABLE[sx]->kind = type;

  // Const: set value
  if (type == CONST)
  {
    SYMBOL_TABLE[sx]->val = value;
    SYMBOL_TABLE[sx]->level = level;
  }

  // variable or procedure: set L and M
  else
  {
    SYMBOL_TABLE[sx]->level = level;
    SYMBOL_TABLE[sx]->addr = m;
  }

  if (PARSER)
  {
    printf("PARSER: %-10s %-5s %d\n", "addSymbol", "end", level);
    fflush(stdout);
  }
}

// Locate Symbol position in SYMBOL_TABLE
int position(char *name)
{
  // Start at back of SYMBOL_TABLE (closest scope)
  int index = sx;

  if (PARSER) printf("PARSER: %-10s %-5s\n", "Position", name);

  // Compare variable names until match found, ST[0]==NULL
  while (index > 0)
  {
    if (strcmp(name, SYMBOL_TABLE[index]->name) == 0)
    {
      if (PARSER) printf("PARSER: %-10s %-5d\n", "Position", index);
      return index;
    }

    index--;
  }

  // Could not be found
  return 0;
}

// Reads output to get next token values
void getNextToken()
{
  token = lexList[++lx]->tokenID;
  tokenValue = lexList[lx]->value;
  strcpy(tokenName, lexList[lx]->name);

  if (PARSER)
  {
    printf("PARSER: %-10s %-5d:%s%s%s\n",
           "getToken", token, TOKEN_NAMES[token],
           (token == identsym)?" > ":"", tokenName);
    fflush(stdout);
  }
}

// Adds into code bank
void addCode(int op, int r, int l, int m)
{
  // Too many instructions
  if (cx > MAX_CODE_SIZE)
    error(25);

  else
  {
    // Allocate memory for instr
    CODE[cx] = calloc(1, sizeof(Instr));

    // Assign instr values (where is r?)
    CODE[cx]->op = op;
    CODE[cx]->r = r;
    CODE[cx]->l = l;
    CODE[cx]->m = m;

    if (PARSER)
    {
      printf("PARSER: %-10s %5s => %d %d %d %d\n", "addCode","", op, r, l, m);
      fflush(stdout);
    }

    // Move CODE index forward
    cx++;
  }
}

// Prints assembly code to output file
void printAssembly()
{
  int i;
  char *file, *temp;
  FILE *ofp;

  // Create new temp file name, make it a directory, open it
  temp = appendFileName(TEMP_FILE, "-a");
  file = attachDir(temp);
  ofp = fopen(file, "w");
  free(temp);

  for (i = 0; (i < MAX_CODE_SIZE) && CODE[i]; i++)
    fprintf(ofp, "%2d %2d %2d %2d\n",
                  CODE[i]->op, CODE[i]->r, CODE[i]->l, CODE[i]->m);

  fclose(ofp);
  appendOutput("Parser/ICG Output:", file);

  // Delete temp file
  if (!checkFlag(KEEP_TEMPS))
    remove(file);

  free(file);
}

// Finds which error occured and prints to screen, stops program.
void error(int num)
{
  printf("ERROR: ");
  switch(num)
  {
    case 1:
      printf("Used '=' instead of ':='.\n");
      break;
    case 2:
      printf("':=' must be followed by a number.\n");
      break;
    case 3:
      printf("Identifier must be followed by '='.\n");
      break;
    case 4:
      printf("Const, var, and precedure must be followed by identifier.\n");
      break;
    case 5:
      printf("Semicolon or comma missing.\n");
      break;
    case 6:
      printf("Incorrect symbol after procedure declaration.\n");
      break;
    case 7:
      printf("Statement expected.\n");
      break;
    case 8:
      printf("Incorrect symbol after statement part in block.\n");
      break;
    case 9:
      printf("Period expected.\n");
      break;
    case 10:
      printf("Semicolon between statements missing.\n");
      break;
    case 11:
      printf("Undeclared identifier.\n");
      break;
    case 12:
      printf("Assignment to constant or procedure is not allowed.\n");
      break;
    case 13:
      printf("Assignment operator expected.\n");
      break;
    case 14:
      printf("Identifier expected.\n");
      break;
    case 15:
      printf("Call of a constant or variable is meaningless.\n");
      break;
    case 16:
      printf("then expected.\n");
      break;
    case 17:
      printf("Semicolon or } expected.\n");
      break;
    case 18:
      printf("do expected.\n");
      break;
    case 19:
      printf("Incorrect symbol following statement.\n");
      break;
    case 20:
      printf("Relational operator expected.\n");
      break;
    case 21:
      printf("Expression must not contain a procedure identifier.\n");
      break;
    case 22:
      printf("Right parenthesis missing.\n");
      break;
    case 23:
      printf("The preceding factor cannot begin with this symbol.\n");
      break;
    case 24:
      printf("An expression cannot begin with this symbol.\n");
      break;
    case 25:
      printf("This number is too large.\n");
      break;
    case 26:
      printf("Ran out of memory!\n");
      break;
    case 27:
      printf("TEMPFILE could not be found.\n");
      break;
    case 28:
      printf("Lexigraphical level too high.\n");
      break;
    case 29:
      printf("Return type expected.\n");
      break;
    case 30:
      printf("Returnsym found, but no expected return.\n");
      break;
    case 31:
      printf("Returnsym expected.\n");
      break;
    case 33:
      printf("Too many arguments passed to function.\n");
      break;
    default:
      printf("Unknown error.\n");
  }

  // Ends program if error occurs
  exit(0);
}

#endif // PARSE_H_
