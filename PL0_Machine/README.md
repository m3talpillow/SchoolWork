Coded by Jared Johansson and Tyler Cudnohufsky  
Fall 2017 - COP3403  
  
**Description:**
Simulates a PL0 Compiler for use with the PMachine (virtual machine, located [here](https://github.com/m3talpillow/SchoolWork/tree/master/PMachine)).  
  
**How to:**  
Compile: gcc Console.c compiler.h scanner.h parser.h pmachine.h  
Inputs: Place files to be read into an inputs folder in the same directory.  
Run: a.exe inputFile tags...  
Output: Goes into an output folder in the same directory and/or creates several files.  
&nbsp;&nbsp;&nbsp;Tags determine what goes into the file output, which will appear as: "(inputFile)-OUTPUT.txt".  
    Files with the same input name will be overriden or deleted if the tags are differnt on the next run.
Tags: determines what is placed into the output file, used by typing "-tag" where tag is the letter of your choice
    - d prints all tags into their own files.   
    - v prints the status of the pmachine; input code, base pointer, stack pointer, registers, and stack.  
    - l prints the input file as token values.  
    - t prints the input file as lexeme terms (easy to read token values).  
    - a prints the generated assembly code outputed by the parser.  
    - f prints the input file at the top of the main output file.  
    - s outputs system messges to console. 
  
**Input:**  
-In order for files to be read they must be in the inputs folder. Otherwise they will not be able to be located.  
-Input can be capital letters or lowercase, and spacing is optional between letters and symbols.   
  
**Output/ tags:**  
-Placing a tag after input file will make the information appear in the master output file.   
-If any step is wanted to be saved in a separate file, -d will make all steps wanted print to separate files as well.  
-Running the file without a tag will delete old files no longer needed. To clear all files related to an input  
just run the file with no tags and all files will be deleted. Ex: a.exe inputFile  
-A list of all possible tags will be given if the incorrect syntax is given. Ex: a.exe   


