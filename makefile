CC=mpicc
flagGCC= -Wall -lm -lpopt
flagIntel= -Wall -lpopt

C_FILES = BellBrandon_Midterm1.c pgm.c pprintf.c
O_FILES = BellBrandon_Midterm1.o pgm.o pprintf.o
out=midterm

# all assumes gcc compiler wich needs -lm flag.
all:
	$(CC) $(flagGCC) -c $(C_FILES) 
	$(CC) $(flagGCC) $(O_FILES) -o $(out)1

# Don't want the -lm flag on the intel complier for Stampede/Comet as they use 
# thier own optomised math library. 
intel:
	$(CC) $(flagIntel) -c $(C_FILES) 
	$(CC) $(flagIntel) $(O_FILES) -o $(out)1

# Just compile the main program, not the library files I grabbed.
main:
	$(CC) $(flagGCC) -c BellBrandon_Midterm1.c
	$(CC) $(flagGCC) $(O_FILES) -o $(out)1

clean:
	rm $(out)* 
	rm $(O_FILES) 
