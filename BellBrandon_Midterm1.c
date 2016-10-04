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
 * -> Popt
 *  -distribution type ( block/checkerboard )
 *  -syncro/asyncro messages
 *  -Number of iterations
 *  -if and when to count the bugs
 *  -initial world file
 *
 * -> Allocate arrays
 * -> Read File
 * -> Count Bugs
 *
 * -> Serial np=1 rules.
 * -> Block implementation
 * 
 */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <popt.h>

///////////////////////////////////////////////////////////////////////////////
// Popt Variables
///////////////////////////////////////////////////////////////////////////////
static int intVal = 55;
static int print = 0;
static char* stringVal;
void callback( poptContext context, 
               enum poptCallbackReason reason,
               const struct poptOption * option,
               const char * arg,
               const void * data )

// Apply the rules for the game of life to a cell.
// Arguments: 
// - int ptr cell: a ptr to a cell (should be in i+1 array)
// - int ptr neighbors: number of neighbors that the cell has.
//
// Returns: 
// - modifies the the 1,0 value (life, death) of the cell.
void con_rules( int *cell, int *neighbors )
{
    if ( neighbors <= 1 )
        cell = 0;
    else if ( neighbors >= 4 )
        cell = 0;
    else if ( neighbors == 2 || neighbors == 3 )
        cell = 1;
    else if ( neighbors == 3 )
        cell = 1;
}

int main( int argc, char* argv[] )
{
    // Popt variables
    static int intVal = 55;
    static int print = 0;
    static char* stringVal;
    void callback(poptContext context,
    enum poptCallbackReason reason,
    const struct poptOption * option,
    const char * arg,
    const void * data)

    // MPI Variables
    int             my_rank;           
    int             p;                 
    int             tag     = 0;    
    MPI_Status      status;
    char            name[MPI_MAX_PROCESSOR_NAME];
    int             pnamemax;
    
    ///////////////////////////////////////////////////////////////////////////
    // Initialize MPI and retreive world size, p's rank, and p's node name.
    ///////////////////////////////////////////////////////////////////////////
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Get_processor_name(name,&pnamemax);

    ///////////////////////////////////////////////////////////////////////////
    // Parse the comand line arguments. 
    ///////////////////////////////////////////////////////////////////////////
}
