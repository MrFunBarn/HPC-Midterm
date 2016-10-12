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

///////////////////////////////////////////////////////////////////////////
// Global Variables. 
///////////////////////////////////////////////////////////////////////////

// MPI Variables
int rank;
int np;
int my_name_len;
char my_name[255];

// global verbose tag.
int verbose;

///////////////////////////////////////////////////////////////////////////
// Count Bugs:
// Find the number of bugs alive in the processes peice of the world.
//
// Inputs:
// - Ptr to the array for wich the bugs are to be counted.
// - int representing the iteration the array represents.
//
// Results:
// - Summs the bugs into a local variable and prints the result to stdout.
///////////////////////////////////////////////////////////////////////////
void countBugs( int *world, int iteration )
{
    int count = 0;
    // Loop variables.
    int i;
    // hight and width of world minus any ghost rows/cols.
    int width;    
    int height;

    // determine if the world is parallelized. If the program is running
    // serialy then there are no ghost rows and if it's running parallel then
    // ghost rows need to be alloted for and not count to prevent double
    // counting bugs. Set the loop variable ranges based on this information.

    // The serial Case.
    if ( np == 1 )
    {
        width   = field_width;
        height  = field_height;
    }
    // The block distribution case.
    else if ( ncols == 1 && np > 1 )
    {
        width   = field_width;
        height  = field_height;
        //printf("The count array address is %d, col %d, row %d\n",world, ncols, nrows);
    }

    // Count the bugs for a serial world. This gets me the known 26,301 bugs.
    for ( i=(width+1); i<( width*height - width -1 ); i++ )
    {
        if ( *(world + i) == 1 )
            count++;
    }
    /* for ( i=0; i<( width*height*sizeof(int) ); i++ ) */
    /* { */
        /* if ( *(world + i) == 1 ) */
            /* count++; */
    /* } */


    printf("[ %d, %d , %d ]\n", rank, iteration, count);
}


int main( int argc, char* argv[] )
{
    // local MPI variables.
    MPI_Status  status;
    int tag = 0;

    // Popt cmd line argument variables.
    int iter_number  = 0;
    int count_alive  = 0;
    int block_type   = 0;
    int verbose      = 0;
    int async_comm   = 0;
    int checker_type = 0;
    char* filename   = ""; 

    // Loop variables.
    int i;
    int j;

    // Conways variables.
    int neighbors;

    ///////////////////////////////////////////////////////////////////////////
    // Parse the comand line arguments with Popt.
    ///////////////////////////////////////////////////////////////////////////
    const struct poptOption optionsTable[] = 
    {
    {"interations",   'i',  POPT_ARG_INT,    &iter_number,  0, 
     "Set the number of world iterations.", "2, 3, ... n" },

    {"count-alive",   'c',  POPT_ARG_INT,    &count_alive,  0,
    "Specifiy the iteration after which to count bugs." , NULL },

    {"verbose",       'v',  POPT_ARG_NONE,   &verbose,      0, 
     "set verbose level to 1." , NULL },

    {"block",         'b',  POPT_ARG_NONE,   &block_type,   0, 
     "Set the process distribution to block type.", NULL },

    {"async-comm",    NULL, POPT_ARG_NONE,   &async_comm,   0, 
     "Set the communication type to asyncronous.", NULL },

    {"checker-board", NULL, POPT_ARG_NONE,   &checker_type, 0, 
     "Set the process distribution to checker board type.", NULL },

    {"filename",      'f',  POPT_ARG_STRING, &filename,     0, 
     "Set the name of the world file to read.", "*.pgm" },
    POPT_AUTOALIAS
    POPT_AUTOHELP
    POPT_TABLEEND
    };
    poptContext context = poptGetContext( "popt1", argc, argv, optionsTable, 0);
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
    // Set-up the MPI and conways variables based on partition scheme being used 
    // and read the starting world file. 
    ///////////////////////////////////////////////////////////////////////////

    // Make sure that only partition type is specified.
    if ( checker_type == 1 && block_type == 1 )
    {
        if ( rank == 0 )
        {
            printf(" => [ERROR] more than one partition scheme specified.\n");
        }
        // Call finalize before quiting, it's just good manners.
        MPI_Finalize();
        return 1;
    }
    // set ncol and nrows for the serial case.
    if ( np == 1 )
    {
        ncols = 1;
        nrows = 1;
    }
    // set ncol and n rows for the block distribution case.
    else if ( block_type == 1  )
    {
        my_row = rank;
        //my_col = 1;
        ncols = 1;
        nrows = np;

        ///////////////////////////////////////////////////////////////////////////
        // Syncroness exchange of ghost rows.
        // The top row sends first and all the other processes recv first.
        ///////////////////////////////////////////////////////////////////////////
        // If top p.
        if (rank == 0 )
        {
            // send the bottom gohst row to next p.
            MPI_Send( (field_a + field_width*field_height - 2*field_width -1 ),
                       field_width, MPI_INT, rank+1, tag, MPI_COMM_WORLD );
            MPI_Recv( (field_a + field_width*field_height - 2*field_width -1), 
                       field_width, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &status );
        }

        // If bottom p.
        else if (rank == np-1 )
        {
            // Send the top row to gohst row of above block and recv into top gohst // row.
            MPI_Send( (field_a + 2*field_width -1 ),
                       field_width, MPI_INT, rank-1, tag, MPI_COMM_WORLD );
            MPI_Recv( (field_a + 2*field_width -1), 
                       field_width, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status );
        }

        // for the non edge processes.
        else
        {
            // Send the top row to gohst row of above block and recv into top gohst // row.
            MPI_Recv( (field_a + field_width*field_height - 2*field_width -1), 
                       field_width, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status );
            MPI_Send( (field_a + 2*field_width -1 ),
                       field_width, MPI_INT, rank-1, tag, MPI_COMM_WORLD );
            // send the bottom gohst row to next p.
            MPI_Send( (field_a + field_width*field_height - 2*field_width -1 ),
                       field_width, MPI_INT, rank+1, tag, MPI_COMM_WORLD );
            MPI_Recv( (field_a + field_width*field_height - 2*field_width -1), 
                       field_width, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &status );
        }
    }
    else if ( checker_type == 1 )
    {
        printf("you choose a not yet implemented checkerbord distribution.\n");
        // Call finalize before quiting, it's just good manners.
        MPI_Finalize();
        return 1;
    }
    else 
    {
        if ( rank == 0 )
        {
            printf(" => [ERROR] More than 1 process started but no partion scheme specified.\n");
            printf("   -> Pleas specify one of --checker-board or -b, --block\n");
        }
        // Call finalize before quiting, it's just good manners.
        MPI_Finalize();
        return 1;
    }

    // Read the world file using the wonderful provided function.
    // populates field_a and _b with the data from that region of the file with
    // size accomidations for the ghost rows/cols. I'll use them as field_a is
    // the i array and field_b is the i+1 array initialy.
    readpgm(filename);

    // Get an initial Bug count.
    countBugs( field_a, 0 );

    ///////////////////////////////////////////////////////////////////////////
    // Play the game: serial version. 
    ///////////////////////////////////////////////////////////////////////////
    int *swap;
    int l = field_width;
    int *cell;
    int *newcell;
    for (j=0; j<iter_number; j++)
    {
        for ( i=(field_width +1 ); i<( field_width*field_height - field_width -1 ); i++ )
        {
            cell = field_a + i;
            newcell = field_b + i;
            // Calculate a cells neighbors by summing the stencil.
            neighbors = 0;
            neighbors = *(cell-1)+ *(cell+1)+ *(cell-l)+ *(cell-l-1)+ *(cell-l+1)+ *(cell+l)+ *(cell+l+1)+ *(cell+l-1);

            // Apply Conway's Rules.
            if ( *cell == 1 && neighbors <= 1 )
                *newcell = 0;
            else if ( *cell == 1 && neighbors >= 4 )
                *newcell = 0;
            else if ( (*cell == 1 && neighbors == 2) || neighbors == 3 )
                *newcell = 1;
            else if ( *cell == 0 && neighbors == 3 )
                *newcell = 1;
            swap = field_b;
            field_b = field_a;
            field_a = swap;
        }

        //printf("iter j %d\n", j);
        // Bug count output.
        if ( count_alive !=0 )
        {
            if ( (j+1) % count_alive == 0 )
                // swap holds what ever the last i+1 ptr was, so the last
                // iteration.
                countBugs( swap, j );
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Final clean-up and shutdown. 
    ///////////////////////////////////////////////////////////////////////////

    // Free the popt context memory.
    poptFreeContext(context);
    // Shutdown MPI.
    MPI_Finalize();

    return 0;
}
