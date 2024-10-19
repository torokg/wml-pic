/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <pthread.h>


long long debug_long0 = -1, debug_long1 = -1;

#define stack_pool_size 5000
static char stack_pool_data[stack_pool_size];

static TX_BYTE_POOL stack_pool;

static char posix_mempool[POSIX_HEAP_SIZE_IN_BYTES];

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

extern void _Z4initv();

static TX_THREAD  main_thread;

void tx_application_define(void *first_unused_memory)
{
    char *main_thread_stack = TX_NULL;
    const size_t main_thread_stack_size = 4096;
    
    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&stack_pool, "stack pool", stack_pool_data, stack_pool_size);
    
    
    /* Allocate the stack for the main thread  */
    tx_byte_allocate(&stack_pool, (VOID **) &main_thread_stack, main_thread_stack_size, TX_NO_WAIT);
    
    if(tx_thread_create(&main_thread, "main_thread", _Z4initv, 0, main_thread_stack, main_thread_stack_size, 7, 1, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
    {
        while(true)
            __builtin_software_breakpoint();
    }
}


extern void alloc_initialize();
int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    posix_initialize(posix_mempool);
    
    alloc_initialize();
    
    /* Start ThreadX kernel */
    tx_kernel_enter();

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

