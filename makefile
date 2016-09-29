CC=mpicc
flagGCC= -Wall -lm
flagIntel= -Wall
out=midterm

# all assumes gcc compiler wich needs -lm flag.
all:
	$(CC) $(flagGCC) BellBrandon_Midterm1.c -o $(out)1

# Don't want the -lm flag on the intel complier for Stampede/Comet as they use 
# thier own optomised math library. 
intel:
	$(CC) $(flagIntel) BellBrandon_Midterm1.c -o $(out)1
clean:
	rm $(out)* 
