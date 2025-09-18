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

#include <cstdio>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

#include "string.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

bool commandAvailable = false;
char command[APP_READ_BUFFER_SIZE];

char consoleMenu[] = {
"       Console over USB CDC\r\n"
"Type a command followed by [ENTER]:\r\n"
"Turn on led\r\n"
"Turn off led\r\n"
"Toggle led\r\n"
"\r\n"
};

void ReadLine( char* data );
void ConsoleReady(void);
void DecodeCommand(char * command);

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    cdc_usb_return_line_callback_register(ReadLine);
    cdc_usb_console_ready_callback_register(ConsoleReady);
    SYSTICK_TimerStart();
    uint16_t delay = 0;
    
    while ( true )
    {
        if(delay++ == 0xFFFF){
            GPIO_PA16_Toggle();
        }
        if(commandAvailable){
            commandAvailable = false;
            DecodeCommand(command);
        }
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

void ReadLine( char* data ){
    if(data == NULL || data[0] == '\0'){
        return;
    }
    commandAvailable = true;
    sprintf(command, "%s", data);
}

void ConsoleReady(void){
    cdc_usb_write(consoleMenu);
}

void DecodeCommand(char * command){
    char txBuffer[256] = {0};
    if(command == NULL || command[0] == '\0'){
        return;
    }
    if(strcmp(command, "Turn on led") == 0){
        GPIO_PB06_Clear();
        sprintf(txBuffer, "\r\nled is on\r\n");
    } else if(strcmp(command, "Turn off led") == 0){
        GPIO_PB06_Set();
        sprintf(txBuffer, "\r\nled is off\r\n");
    } else if(strcmp(command, "Toggle led") == 0){
        GPIO_PB06_Toggle();
        sprintf(txBuffer, "\r\nled is toggled\r\n");
    } else {
        sprintf(txBuffer, "\r\nUnknown command\r\n");
    }
    strcat(txBuffer, "\r\n");
    strcat(txBuffer, consoleMenu);
    cdc_usb_write(txBuffer);
}