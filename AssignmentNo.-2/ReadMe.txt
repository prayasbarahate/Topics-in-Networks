*****************************Problem1*************************************************

To compile the program, use following commmand in terminal:

						g++ Assignment2.cpp
					
To run the program, use following commands: 

FOR ISLIP :	./a.out -N 32 -B 8 -p 0.5 -queue islip -K 0.8 -out Output.txt -T 1000

FOR INQ :	./a.out -N 32 -B 8 -p 0.5 -queue inq -K 0.8 -out Output.txt -T 1000

FOR KOUQ :	./a.out -N 32 -B 8 -p 0.5 -queue kouq -K 0.8 -out Output.txt -T 1000

Note : if any of paremeters are not provided in the command line, then that parameter will take the default value.

Output : output of the scheduling algorithm can be found in the provided output file, in the same folder, where the code file resides.
	As well as, output is displayed on the console.