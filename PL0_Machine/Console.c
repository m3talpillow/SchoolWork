// Jared Johansson and Tyler Cudnohufsky

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"
#include "scanner.h"
#include "parser.h"
#include "pmachine.h"

// Console printing
int CB = 0;

// Prototypes
void readCommandLine(int, char* []);
void incorrectSyntax(void);
void fileError(char *);


// Takes code written in PL/0
int main(int argc, char *argv[])
{
  readCommandLine(argc, argv);

  if (checkFlag(PRINT_FILE))
    appendOutput("Input File:", DIR_INPUT);

  // Input file -> scanner output created
  printf("Scanner start.\n");
  scanner(argv[1]);

  // Scanner -> parser/Intermediate Code Generator output
  printf("Parser/ICG start.\n");
  parser();

  // Parser/ICG -> pmachine
  printf("Pmachine start.\n");
  pmachine();

  printf("Complete.\n");
}

// Verifies command line and sets all flags
void readCommandLine(int argc, char *argv[])
{
  int i, j = 0;
  FILE *fp = NULL;
  char *temp;
  char buffer[MAX_BUFFER_SIZE];

  // Confirms a second string
  if (argc > 1)
  {
    // Confirms file is in correct directory and saves info
    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
      // Save directory location
      for (i = 0; buffer[i] != '\0'; i++)
        j++;
      if ((DIR_CURRENT = malloc(sizeof(char) * i)) == NULL)
        fileError("MALLOC");
      strcpy(DIR_CURRENT, buffer);

      // Save directory/outputs
      strcat(buffer, "\\outputs\\");
      for ( ; buffer[i] != '\0'; i++)
        ;
      if ((DIR_OUTPUTS = malloc(sizeof(char) * i)) == NULL)
        fileError("MALLOC");
      strcpy(DIR_OUTPUTS, buffer);

      // Save directory/inputs
      buffer[j] = '\0';
      strcat(buffer, "\\inputs\\");
      strcat(buffer, argv[1]);
      for (i = j; buffer[i] != '\0'; i++)
        ;
      if ((DIR_INPUT = malloc(sizeof(char) * i)) == NULL)
        fileError("MALLOC");
      strcpy(DIR_INPUT, buffer);
    }

    // Open directory/inputs/fileName
    if ((fp = fopen(DIR_INPUT, "r")) != NULL)
    {
      fclose(fp);

      // Sets optional flags
      for (i = 2; i < argc; i++)
      {
        // Tag notifier '-' exists, then search tags
        if (argv[i][0] == '-')
        {
          switch ((char)argv[i][1])
          {
            case 'l':
              FLAGS += 1 << PRINT_LEXEME;
              break;
            case 't':
              FLAGS += 1 << PRINT_LEXEME_TABLE;
              break;
            case 'a':
              FLAGS += 1 << PRINT_ASSEMBLY;
              break;
            case 'v':
              FLAGS += 1 << PRINT_VM;
              break;
            case 'f':
              FLAGS += 1 << PRINT_FILE;
              break;
            case 'd':
              FLAGS += 1 << KEEP_TEMPS;
              break;
            case 's':
              FLAGS += 1 << SYSTEM_MESSAGES;
              break;
            default:
              printf("Unrecognized tag: %s\n", argv[i]);
          }
        }
        else
          printf("Unrecognized tag: %s\n", argv[i]);
      }

      if (checkFlag(SYSTEM_MESSAGES))
        CB = 1;

      if (CB)
      {
        printf("Saved files:\n");
        printf("%7s: %s\n", "Outputs",  DIR_OUTPUTS);
        printf("%7s: %s\n", "Input", argv[1]);
      }
      // Make output file name
      temp = appendFileName(argv[1], "-OUTPUT");
      DIR_MASTER_OUT = attachDir(temp);
      free(temp);
      if (CB) printf("%7s: %s\n", "Master", DIR_MASTER_OUT);

      // Make a temp file name to attach tags to
      TEMP_FILE = appendFileName(argv[1], TEMPFILE);
      if (CB) printf("%7s: %s\n\n", "Temps", TEMP_FILE);

      // Check if filename is already in directory. If so, erase
      fclose(fopen(DIR_MASTER_OUT, "w"));

      // If no tags, remove previous output file (if exists);
      if (argc < 3)
        remove(DIR_MASTER_OUT);

      // Was in correct syntax and input file exists
      return;
    }
    // Probably correct syntax, but file could not be found
    else
      fileError(argv[1]);
  }

  incorrectSyntax();
  exit(0);
}

// Exception handling
void fileError(char *fileName)
{
  printf("ERROR: File could not be opened: %s\n", fileName);

  if (DIR_INPUT != NULL)
    printf("At directory: %s\n", DIR_INPUT);
  else
    printf("Within directory: %s\n", DIR_CURRENT);

  exit(0);
}

// Prints error message for user before exiting program.
void incorrectSyntax()
{
  printf( "ERROR: incorrect syntax used.\n"
          "Correct syntax: Console.c <f> <opt> <opt>\n"
          "<f> is the file to be opened.\n"
          "<opt> is the optional printing selections.\n"
          "\n"
          "Print tags include:\n"
          "-f   output of the input file.\n"
          "-l   list of tokens (scanner output).\n"
          "-t   table of lexemes (easy to read tokens)\n"
          "-a   generated assembly code (parser output).\n"
          "-v   virtual machine trace (vm output).\n"
          "-d   makes output files for all steps.\n"
          "-s   prints system messages to console.\n" );
}
