/* 
 * Brandon Bell
 * CSCI: 4576
 * Midterm 1: Conway's Game of Life. 
 * 10-5-16
 *
 * Popt was causeing my some frustrations so i grabbed a working example from
 * https://docs.fedoraproject.org/en-US/Fedora_Draft_Documentation/0.1/html/RPM_Guide/ch15s02s04.html
 * and modified it to fit my needs.
 *
 * Additionaly, I barrowed the globals.h, pgm, and pprintf file provided on
 * moddle.
 *
 * TO-DO
 * -> Allocate arrays
 * -> Read File
 * -> Count Bugs
 *
 * -> Serial np=1 rules.
 * -> Block implementation
 * -> Rule implementation
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <popt.h>

// Include global variables. 
#define __MAIN 
#include "globals.h"
#undef __MAIN

// User includes
#include "pprintf.h"
#include "pgm.h"

// MPI Variables
int rank;
int np;
int my_name_len;
char my_name[255];
// global verbose tag.
int verbose;

// Apply the rules for the game of life to a cell.
// Arguments: 
// - int ptr cell: a ptr to a cell (should be in i+1 array)
// - int ptr neighbors: number of neighbors that the cell has.
//
// Returns: 
// - modifies the the 1,0 value (life, death) of the cell.
void con_rules( int *cell, int *neighbors )
{
    if ( *neighbors <= 1 )
        *cell = 0;
    else if ( *neighbors >= 4 )
        *cell = 0;
    else if ( *neighbors == 2 || *neighbors == 3 )
        *cell = 1;
    else if ( *neighbors == 3 )
        *cell = 1;
}


int main( int argc, char* argv[] )
{
    // Popt cmd line argument variables.
    int iter_number  = 0;
    int count_alive  = 0;
    int block_type   = 0;
    int verbose      = 0;
    int async_comm   = 0;
    int checker_type = 0;
    char* filename   = "";

    ///////////////////////////////////////////////////////////////////////////
    // Parse the comand line arguments with Popt.
    ///////////////////////////////////////////////////////////////////////////
    struct poptOption optionsTable[] = 
    {
    {"interations",   'i',  POPT_ARG_INT,    &iter_number,  0, "Set the number of world iterations.", "2, 3, ... n" },
    {"count-alive",   'c',  POPT_ARG_INT,    &count_alive,  0, "Specifiy the iteration after which to count bugs." , NULL },
    {"verbose",       'v',  POPT_ARG_NONE,   &verbose,      0, "set verbose level to 1." , NULL },
    {"block",         'b',  POPT_ARG_NONE,   &block_type,   0, "Set the process distribution to block type.", NULL },
    {"async-comm",    NULL, POPT_ARG_NONE,   &async_comm,   0, "Set the communication type to asyncronous.", NULL },
    {"checker-board", NULL, POPT_ARG_NONE,   &checker_type, 0, "Set the process distribution to checker board type.", NULL },
    {"filename",      'f',  POPT_ARG_STRING, &filename,     0, "Set the name of the world file to read.", "*.pgm" },
    POPT_AUTOALIAS
    POPT_AUTOHELP
    POPT_TABLEEND
    };
    poptContext context = poptGetContext( "popt1", argc, argv, &optionsTable, 0);
    int option          = poptGetNextOpt(context);

    // Handle verbose output of command line arguments if v switch set. 
    if ( verbose == 1 )
    {
        printf("option = %d\n", option);
        printf("After processing, options have values:\n");
        printf("\t iterations holds %d\n", iter_number);
        printf("\t count_alive holds %d\n", count_alive);
        printf("\t block flag holds %d\n", block_type);
        printf("\t verbose flag holds %d\n", verbose);
        printf("\t checker flag holds %d\n", checker_type);
        printf("\t async_comm flag holds %d\n", async_comm);
        printf("\t filename holds [%s]\n", filename);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Initialize MPI and retreive world size, p's rank, and p's node name.
    ///////////////////////////////////////////////////////////////////////////
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(my_name,&my_name_len);
    // Verbose output of basic MPI process information.
    if ( verbose == 1 )
        printf("[ %s, %d ] World size = %d\n", my_name, rank, np);


    ///////////////////////////////////////////////////////////////////////////
    // Set-up the conways variables and read the starting world file. 
    ///////////////////////////////////////////////////////////////////////////

    // set ncol and nrows for the serial case.
    if ( np == 1 )
    {
        ncols = 1;
        nrows = 1;
    }
    // set ncol and n rows for the block distribution case.
    else if ( block_type == 1 )
    {
        ncols = 1;
        nrows = np;
    }
    else if ( checker_type == 1 )
    {
        printf("you choose a not yet implemented checkerbord distribution.\n");
        return 1;
    }
    // Read the world file using the wonderful provided function.
    readpgm(filename);


    ///////////////////////////////////////////////////////////////////////////
    // Final clean-up and shutdown. 
    ///////////////////////////////////////////////////////////////////////////

    // Free the popt context memory.
    poptFreeContext(context);
    // Shutdown MPI.
    MPI_Finalize();

    return 0;
}
