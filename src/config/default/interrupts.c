/*******************************************************************************
 System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt.c

  Summary:
    Interrupt vectors mapping

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "interrupts.h"
#include "definitions.h"



// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************


/* All the handlers are defined here.  Each will call its PLIB-specific function. */
// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector declarations
// *****************************************************************************
// *****************************************************************************
void TIMER_2_Handler (void);
void TIMER_3_Handler (void);
void I2C1_BUS_Handler (void);
void I2C1_MASTER_Handler (void);
void SPI1_FAULT_Handler (void);
void SPI1_RX_Handler (void);
void SPI1_TX_Handler (void);
void SPI2_RX_Handler (void);
void SPI2_TX_Handler (void);
void UART1_FAULT_Handler (void);
void UART1_RX_Handler (void);
void UART1_TX_Handler (void);
void UART2_FAULT_Handler (void);
void UART2_RX_Handler (void);
void UART2_TX_Handler (void);
void UART3_FAULT_Handler (void);
void UART3_RX_Handler (void);
void UART3_TX_Handler (void);
void UART4_FAULT_Handler (void);
void UART4_RX_Handler (void);
void UART4_TX_Handler (void);
void SPI5_RX_Handler (void);
void SPI5_TX_Handler (void);
void UART5_FAULT_Handler (void);
void UART5_RX_Handler (void);
void UART5_TX_Handler (void);
void SPI6_RX_Handler (void);
void SPI6_TX_Handler (void);
void UART6_FAULT_Handler (void);
void UART6_RX_Handler (void);
void UART6_TX_Handler (void);
void CHANGE_NOTICE_A_Handler (void);
void CHANGE_NOTICE_C_Handler (void);
void CHANGE_NOTICE_D_Handler (void);
void CHANGE_NOTICE_G_Handler (void);
void CHANGE_NOTICE_H_Handler (void);
void CHANGE_NOTICE_J_Handler (void);
void CHANGE_NOTICE_K_Handler (void);

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector definitions
// *****************************************************************************
// *****************************************************************************

void __ISR(_TIMER_2_VECTOR, ipl1SRS) TIMER_2_Handler (void)
{
    TIMER_2_InterruptHandler();
}

void __ISR(_TIMER_3_VECTOR, ipl1SRS) TIMER_3_Handler (void)
{
    TIMER_3_InterruptHandler();
}

void __ISR(_UART1_FAULT_VECTOR, ipl1SAVEALL) UART1_FAULT_Handler (void)
{
    _tx_thread_context_save();
    UART1_FAULT_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART1_RX_VECTOR, ipl1SAVEALL) UART1_RX_Handler (void)
{
    _tx_thread_context_save();
    UART1_RX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART1_TX_VECTOR, ipl1SAVEALL) UART1_TX_Handler (void)
{
    _tx_thread_context_save();
    UART1_TX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART2_FAULT_VECTOR, ipl1SAVEALL) UART2_FAULT_Handler (void)
{
    _tx_thread_context_save();
    UART2_FAULT_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART2_RX_VECTOR, ipl1SAVEALL) UART2_RX_Handler (void)
{
    _tx_thread_context_save();
    UART2_RX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART2_TX_VECTOR, ipl1SAVEALL) UART2_TX_Handler (void)
{
    _tx_thread_context_save();
    UART2_TX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART3_FAULT_VECTOR, ipl1SAVEALL) UART3_FAULT_Handler (void)
{
    _tx_thread_context_save();
    UART3_FAULT_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART3_RX_VECTOR, ipl1SAVEALL) UART3_RX_Handler (void)
{
    _tx_thread_context_save();
    UART3_RX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART3_TX_VECTOR, ipl1SAVEALL) UART3_TX_Handler (void)
{
    _tx_thread_context_save();
    UART3_TX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART4_FAULT_VECTOR, ipl1SAVEALL) UART4_FAULT_Handler (void)
{
    _tx_thread_context_save();
    UART4_FAULT_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART4_RX_VECTOR, ipl1SAVEALL) UART4_RX_Handler (void)
{
    _tx_thread_context_save();
    UART4_RX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART4_TX_VECTOR, ipl1SAVEALL) UART4_TX_Handler (void)
{
    _tx_thread_context_save();
    UART4_TX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART5_FAULT_VECTOR, ipl1SAVEALL) UART5_FAULT_Handler (void)
{
    _tx_thread_context_save();
    UART5_FAULT_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART5_RX_VECTOR, ipl1SAVEALL) UART5_RX_Handler (void)
{
    _tx_thread_context_save();
    UART5_RX_InterruptHandler();
    _tx_thread_context_restore();    
}

void __ISR(_UART5_TX_VECTOR, ipl1SAVEALL) UART5_TX_Handler (void)
{
    _tx_thread_context_save();
    UART5_TX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART6_FAULT_VECTOR, ipl1SAVEALL) UART6_FAULT_Handler (void)
{
    _tx_thread_context_save();
    UART6_FAULT_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART6_RX_VECTOR, ipl1SAVEALL) UART6_RX_Handler (void)
{
    _tx_thread_context_save();
    UART6_RX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_UART6_TX_VECTOR, ipl1SAVEALL) UART6_TX_Handler (void)
{
    _tx_thread_context_save();
    UART6_TX_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_CHANGE_NOTICE_A_VECTOR, ipl1SAVEALL) CHANGE_NOTICE_A_Handler (void)
{
    _tx_thread_context_save();
    CHANGE_NOTICE_A_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_CHANGE_NOTICE_C_VECTOR, ipl1SAVEALL) CHANGE_NOTICE_C_Handler (void)
{
    _tx_thread_context_save();
    CHANGE_NOTICE_C_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_CHANGE_NOTICE_D_VECTOR, ipl1SAVEALL) CHANGE_NOTICE_D_Handler (void)
{
    _tx_thread_context_save();
    CHANGE_NOTICE_D_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_CHANGE_NOTICE_G_VECTOR, ipl1SAVEALL) CHANGE_NOTICE_G_Handler (void)
{
    _tx_thread_context_save();
    CHANGE_NOTICE_G_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_CHANGE_NOTICE_H_VECTOR, ipl1SAVEALL) CHANGE_NOTICE_H_Handler (void)
{
    _tx_thread_context_save();
    CHANGE_NOTICE_H_InterruptHandler();
    _tx_thread_context_restore();
}

void __ISR(_CHANGE_NOTICE_J_VECTOR, ipl1SRS) CHANGE_NOTICE_J_Handler (void)
{
    CHANGE_NOTICE_J_InterruptHandler();
}

void __ISR(_CHANGE_NOTICE_K_VECTOR, ipl1SRS) CHANGE_NOTICE_K_Handler (void)
{
    CHANGE_NOTICE_K_InterruptHandler();
}




/*******************************************************************************
 End of File
*/
