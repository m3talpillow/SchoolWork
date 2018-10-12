Coded by Jared Johansson and Tyler Cudnohufsky
Fall 2017 - COP3403

Compile: gcc Console.c compiler.h scanner.h parser.h pmachine.h
Inputs: Place files to be read into inputs folder.
Run: a.exe <inputFile> <tags>...<tags>
Output: Goes into an output folder. Main output holds all wanted tags. "(inputFile)-OUTPUT.txt"

Input:
-In order for files to be read they must be in the inputs folder. Otherwise they will not be able to be located.
-Input can be capital letters or lowercase, and spacing is optional between letters and symbols. 

Output/ tags:
-Placing a tag after input file will make the information appear in the master output file. 
-If any step is wanted to be saved in a separate file, -d will make all steps wanted print to separate files as well.
-Running the file without a tag will delete old files no longer needed. To clear all files related to an input
just run the file with no tags and all files will be deleted. Ex: a.exe <inputFile>
-A list of all possible tags will be given if the incorrect syntax is given. Ex: a.exe


