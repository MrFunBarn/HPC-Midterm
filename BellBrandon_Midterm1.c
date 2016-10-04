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
    // MPI Variables
    int             my_rank;           
    int             p;                 
    int             tag = 0;    
    MPI_Status      status;
    char            name[MPI_MAX_PROCESSOR_NAME];
    int             pnamemax;

    // Popt cmd line argument variables.
    int iter_number  = 0;
    int count_alive  = 0;
    int block_type   = 0;
    int verbose      = 0;
    int async_comm   = 0;
    int checker_type = 0;
    char* filename;

    ///////////////////////////////////////////////////////////////////////////
    // Initialize MPI and retreive world size, p's rank, and p's node name.
    ///////////////////////////////////////////////////////////////////////////
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Get_processor_name(name,&pnamemax);

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

    ///////////////////////////////////////////////////////////////////////////
    // Print outputs. 
    ///////////////////////////////////////////////////////////////////////////

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
    // Final clean-up and shutdown. 
    ///////////////////////////////////////////////////////////////////////////
    poptFreeContext(context);
    exit(0);
}
