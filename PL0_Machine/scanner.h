// Jared Johansson and Tyler Cudnohufsky
// Has all functions needed to implement a scanner

#ifndef SCAN_H_
#define SCAN_H_

#include <ctype.h>
#include <stdarg.h>
#include "compiler.h"

// Prototypes
int scanner(char *);
void reset(int, ...);
void makeLexList(char *);
void buildLexeme(char *, int *, char);
void identifyLexeme(char *, int);
void identifySymbol(char *);
void printLexList(int);

// Boolean for debugging
short SCANNER = 0;

// Begin scanner
int scanner(char *fileName)
{
  // If system messages should be printed
  if (checkFlag(SYSTEM_MESSAGES))
    SCANNER = 1;

  if (SCANNER) printf("SCANNER: Clear old files.\n");

  // Clear older files from previous runs
  clearOldFiles("-l");
  clearOldFiles("-t");

  makeLexList(fileName);

  // Print token list to file
  if (checkFlag(PRINT_LEXEME))
    printLexList(0);

  // Print lexeme list to file
  if (checkFlag(PRINT_LEXEME_TABLE))
    printLexList(1);

  if (SCANNER) printf("SCANNER: exit scanner.\n");
  return 0;
}

// Generates lexeme list
void makeLexList(char *fileName)
{
  FILE *ifp = fopen(DIR_INPUT, "r");
  char c, lexeme[MAX_ID_SIZE + 1];
  int ldx = 0, letter = 0, digit = 0,
      symbol = 0, comment = 0;  // boolean flags

  if (SCANNER) printf("SCANNER: Begin making lexList\n");

  // Go through entire input file
  while ((c = fgetc(ifp)) != EOF)
  {
    // Checking for comments
    if (c == '/')
    {
      if (ldx != 0 && lexeme[ldx - 1] == '/')
      {
        comment = 1;
        reset(2, &ldx, &symbol);
      }
      else
        buildLexeme(lexeme, &ldx, c);
    }
    // If white space, stop collecting and check lexeme
    else if (c == ' ' || c == '\n' || c == '\t')
    {
      // Skips multiple whitespace
      if (ldx > 0)
      {
        identifyLexeme(lexeme, comment);
        reset(4, &ldx, &letter, &digit, &symbol);
      }

      // Once new line, no longer in comment
      if (c == '\n')
        comment = 0;
    }

    // Letter found
    else if (isalpha(c))
    {
      letter = 1;
      // If last char was
      if (digit || symbol)
      {
        identifyLexeme(lexeme, comment);
        reset(3, &ldx, &digit, &symbol);
      }
      // Add to lexeme
      buildLexeme(lexeme, &ldx, c);
    }

    // Digit found
    else if (isdigit(c))
    {
      digit = 1;
      // letters can come before digits
      if (symbol)
      {
        identifyLexeme(lexeme, comment);
        reset(2, &ldx, &symbol);
      }
      buildLexeme(lexeme, &ldx, c);
    }

    // Must be symbol
    else
    {
      symbol = 1;
      if (digit || letter)
      {
        identifyLexeme(lexeme, comment);
        reset(3, &ldx, &letter, &digit);
      }
      buildLexeme(lexeme, &ldx, c);
    }
  }
  // Catch last token, period
  if (ldx > 0)
    identifyLexeme(lexeme, comment);

  if (SCANNER) printf("SCANNER: lexList created\n\n");
  fclose(ifp);
}

// Resets all booleans passed
void reset(int num, ...)
{
  va_list valist;
  int i;

  // Initalize va list for number of args
  va_start(valist, num);
  // Go through all passed arguments
  for (i = 0; i < num; i++)
    *(va_arg(valist, int *)) = 0;

  // free memory
  va_end(valist);
}

// Easy building of lexeme
void buildLexeme(char *lexeme, int *index, char c)
{
  // Ensures string is within bounds
  if (*index > MAX_ID_SIZE)
  {
    printf("ERROR: lexeme too large!\n");
    exit(0);
  }

  // Add new char
  lexeme[*index] = c;
  // Advance index
  (*index)++;
  // Ensure null char at end
  lexeme[*index] = '\0';
}

// Takes a two char array to identify which Token
void identifySymbol(char *sym)
{
  int i, value = -1, len;

  if (sym[0] == ')' && sym[1] != '\0')
  {
    // Add into lexe list
    lexList[lx] = calloc(1, sizeof(Lexeme));
    lexList[lx++]->tokenID = rparentsym;
    if (SCANNER) printf("SCANNER: Symbol identified: %s\n", ")");

    // Remove from sym
    len = strlen(sym);
    for (i = 0; i < len; i++)
      sym[i] = sym[i+1];
  }

  // Check if a reserved word
  for (i = 0; i < NUM_RESERVED_SYM; i++)
    if (strcmp(sym, RESERVED_SYM[i]) == 0)
    {
      value = RS_VAL[i];
      break;
    }

  if (value < 0)
  {
    printf("ERROR: lexeme couldn't be identified: %s\n", sym);
    exit(0);
  }

  lexList[lx] = calloc(1, sizeof(Lexeme));
  lexList[lx++]->tokenID = value;
  if (SCANNER) printf("SCANNER: Symbol identified: %s\n", sym);
}

// Checks a lexeme and finds its Token
void identifyLexeme(char *name, int comment)
{
  int i, value = -1, len = strlen(name);

  // if in comment, do nothing.
  if (comment)
    return;

  // If symbol, different switch
  if (!isalpha(name[0]) && !isdigit(name[0]))
  {
    identifySymbol(name);
    return;
  }

  // Drops all alphas to lowercase (skips digits)
  for (i = 0; i < len; i++)
    name[i] = tolower(name[i]);

  // Check if a reserved word
  for (i = 0; i < NUM_RESERVED_WORDS; i++)
    if (strcmp(name, RESERVED_WORD[i]) == 0)
    {
      value = RW_VAL[i];
      break;
    }

  // lexeme wasn't a reserved word
  if (value < 0)
  {
    // If passed a digit or variable name
    if (isdigit(name[0]))
      value = numbersym;
    else if (isalpha(name[0]))
      value = identsym;
    else
    {
      printf("ERROR: lexeme couldn't be identified: %s\n", name);
      exit(0);
    }
  }

  // Add to lexList
  lexList[lx] = calloc(1, sizeof(Lexeme));
  lexList[lx]->tokenID = value;

  // Include token's name
  if (value == identsym)
    strcpy(lexList[lx]->name, name);

  // token's value
  else if (value == numbersym)
    lexList[lx]->value = toDigit(name);

  lx++;
  if (SCANNER) printf("SCANNER: Lexeme identified: %s\n", name);
}

// Prints the LexList
void printLexList(int type)
{
  int i = 0, j = 0, indent = 0, next, proc = 0;
  char *newfile, *temp;
  char buffer[MAX_BUFFER_SIZE], token[MAX_BUFFER_SIZE];
  FILE *ofp;

  // Create file destination
  if (type == 0)
    temp = appendFileName(TEMP_FILE, "-l");
  else
    temp = appendFileName(TEMP_FILE, "-t");
  newfile = attachDir(temp);
  free(temp);

  buffer[j] = '\0';
  ofp = fopen(newfile, "w");

  // traverse list
  while (lexList[i])
  {
    // String is either token name or id
    if (type == 0)
      sprintf(token, "%d", lexList[i]->tokenID);
    else
      strcpy(token, TOKEN_NAMES[lexList[i]->tokenID]);

    // After semicolon, new line
    if (lexList[i]->tokenID == semicolonsym)
    {
      fprintf(ofp, "%s%s", (indent)?buffer:"", token);
      if (lexList[i+1]->tokenID != endsym)
        fprintf(ofp, "\n");
      indent = 1;
    }

    // numsym and its value
    else if (lexList[i]->tokenID == numbersym)
    {
      fprintf(ofp, "%s %d ", token, lexList[i]->value);
      indent = 0;
    }

    // identsym and its name
    else if (lexList[i]->tokenID == identsym || lexList[i]->tokenID == callsym)
    {
      fprintf(ofp, "%s%s %s ", (indent)?buffer:"", token, lexList[i]->name);
      indent = 0;
    }

    // Anything else from statement
    else if (lexList[i]->tokenID == varsym || lexList[i]->tokenID == whilesym ||
             lexList[i]->tokenID == ifsym   || lexList[i]->tokenID == readsym  ||
             lexList[i]->tokenID == writesym || lexList[i]->tokenID == returnsym)
    {
      fprintf(ofp, "%s%s %s", (indent)?buffer:"", token, lexList[i]->name);
      indent = 0;
    }

    else if (lexList[i]->tokenID == dosym)
    {
      fprintf(ofp, "%s\n", token);
      indent = 1;
    }

    // begin, newline and indent lines after
    else if (lexList[i]->tokenID == beginsym)
    {
      fprintf(ofp, "%s%s\n", buffer, token);
      buffer[j++] = '\t';
      buffer[j] = '\0';
      indent = 1;
    }

    // end, print on its own line, remove a layer of indenting
    else if (lexList[i]->tokenID == endsym)
    {
      next = lexList[i + 1]->tokenID;
      buffer[--j] = '\0';

      // dont preindent if last token was end
      if (lexList[i - 1]->tokenID != endsym)
        fprintf(ofp, "\n");

      // dont indent if next token is semicolon
      fprintf(ofp, "%s%s ", buffer, token);
      if (next != semicolonsym && next != periodsym)
        fprintf(ofp, "\n");

      if (proc > 0)
      {
        buffer[--j] = '\0';
        proc--;
      }

      indent = 1;
    }

    // procedure declaration
    else if (lexList[i]->tokenID == procsym)
    {
      fprintf(ofp, "%s%s ", buffer, token);
      proc++;
      buffer[j++] = '\t';
      buffer[j] = '\0';
      indent = 0;
    }

    else if (lexList[i]->tokenID == rparentsym)
    {
      fprintf(ofp, "%s ", token);
      if (lexList[i+1]->tokenID == endsym)
        fprintf(ofp, "\n");
    }

    // everything else
    else
      fprintf(ofp, "%s ", token);

    i++;
  }
  fprintf(ofp, "\n");

  fclose(ofp);
  appendOutput("Scanner Output:", newfile);

  // Delete temp file
  if (!checkFlag(KEEP_TEMPS))
    remove(newfile);

  free(newfile);
}


#endif // SCAN_H_
