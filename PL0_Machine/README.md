Coded by Jared Johansson and Tyler Cudnohufsky  
Fall 2017 - COP3403  
  
**Description:**  
Simulates a PL0 Compiler for use with the PMachine (virtual machine, located [here](https://github.com/m3talpillow/SchoolWork/tree/master/PMachine)).  
  
**How to:**  
*Compile:* gcc Console.c compiler.h scanner.h parser.h pmachine.h  
*Inputs:* Place files to be read into an inputs folder in the same directory.  
*Run:* a.exe inputFile tags...  
*Output:* Goes into an output folder in the same directory and/or creates several files.  
&nbsp;&nbsp;&nbsp; Output file will appear as: "(inputFile)-OUTPUT.txt".  
&nbsp;&nbsp;&nbsp; Saved tags will appear as: "(inputFile)-TEMP-(tag).txt".
*Tags:* determines what is placed into the output file, more information below 
&nbsp;&nbsp;&nbsp; -d prints all tags into their own files.   
&nbsp;&nbsp;&nbsp; -v prints the status of the pmachine; input code, base pointer, stack pointer, registers, and stack.  
&nbsp;&nbsp;&nbsp; -l prints the input file as token values.  
&nbsp;&nbsp;&nbsp; -t prints the input file as lexeme terms (easy to read token values).  
&nbsp;&nbsp;&nbsp; -a prints the generated assembly code outputed by the parser.  
&nbsp;&nbsp;&nbsp; -f prints the input file at the top of the main output file.  
&nbsp;&nbsp;&nbsp; -s outputs system messges to console. 
  
**Input:**  
-In order for files to be read they must be in the inputs folder. Otherwise they will not be able to be located.  
-Input can be capital letters or lowercase, and spacing is optional between letters and symbols.   
   
**Output/ tags:**  
-Placing a tag after input file will make the information appear in the master output file.   
-If any step is wanted to be saved in a separate file, -d will make all steps wanted print to separate files as well.  
-Running the file without a tag will delete old files. Only last run tags will persist or be created.
-A list of all possible tags will be given if the incorrect syntax is given, such as not specifying a file.   


