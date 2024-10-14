/*******************************************************************************
  UART4 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_uart4.c

  Summary:
    UART4 PLIB Implementation File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#include "device.h"
#include "plib_uart4.h"
#include "interrupts.h"
#include "tx_api.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART4 Implementation
// *****************************************************************************
// *****************************************************************************

volatile static UART_RING_BUFFER_OBJECT uart4Obj;

#define UART4_READ_BUFFER_SIZE      (128U)
#define UART4_READ_BUFFER_SIZE_9BIT (128U >> 1)
#define UART4_RX_INT_DISABLE()      IEC5CLR = _IEC5_U4RXIE_MASK;
#define UART4_RX_INT_ENABLE()       IEC5SET = _IEC5_U4RXIE_MASK;

volatile static uint8_t UART4_ReadBuffer[UART4_READ_BUFFER_SIZE];

#define UART4_WRITE_BUFFER_SIZE      (128U)
#define UART4_WRITE_BUFFER_SIZE_9BIT (128U >> 1)
#define UART4_TX_INT_DISABLE()       IEC5CLR = _IEC5_U4TXIE_MASK;
#define UART4_TX_INT_ENABLE()        IEC5SET = _IEC5_U4TXIE_MASK;

volatile static uint8_t UART4_WriteBuffer[UART4_WRITE_BUFFER_SIZE];

#define UART4_IS_9BIT_MODE_ENABLED()    ( (U4MODE) & (_U4MODE_PDSEL0_MASK | _U4MODE_PDSEL1_MASK)) == (_U4MODE_PDSEL0_MASK | _U4MODE_PDSEL1_MASK) ? true:false

void static UART4_ErrorClear( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint8_t dummyData = 0u;

    errors = (UART_ERROR)(U4STA & (_U4STA_OERR_MASK | _U4STA_FERR_MASK | _U4STA_PERR_MASK));

    if(errors != UART_ERROR_NONE)
    {
        /* If it's a overrun error then clear it to flush FIFO */
        if((U4STA & _U4STA_OERR_MASK) != 0U)
        {
            U4STACLR = _U4STA_OERR_MASK;
        }

        /* Read existing error bytes from FIFO to clear parity and framing error flags */
        while((U4STA & _U4STA_URXDA_MASK) != 0U)
        {
            dummyData = (uint8_t)U4RXREG;
        }

        /* Clear error interrupt flag */
        IFS5CLR = _IFS5_U4EIF_MASK;

        /* Clear up the receive interrupt flag so that RX interrupt is not
         * triggered for error bytes */
        IFS5CLR = _IFS5_U4RXIF_MASK;

    }

    // Ignore the warning
    (void)dummyData;
}

static TX_SEMAPHORE sem_uart4_tx_buffer, sem_uart4_tx, sem_uart4_rx_buffer;

void UART4_Initialize( void )
{
    tx_semaphore_create(&sem_uart4_tx_buffer,"",1);
    tx_semaphore_create(&sem_uart4_tx,"",0);
    tx_semaphore_create(&sem_uart4_rx_buffer,"",0);
    /* Set up UxMODE bits */
    /* STSEL  = 0 */
    /* PDSEL = 0 */

    U4MODE = 0x8;

    /* Enable UART4 Receiver and Transmitter */
    U4STASET = (_U4STA_UTXEN_MASK | _U4STA_URXEN_MASK | _U4STA_UTXISEL1_MASK );

    /* BAUD Rate register Setup */
    U4BRG = 216;

    /* Disable Interrupts */
    IEC5CLR = _IEC5_U4EIE_MASK;

    IEC5CLR = _IEC5_U4RXIE_MASK;

    IEC5CLR = _IEC5_U4TXIE_MASK;

    /* Initialize instance object */
    uart4Obj.rdCallback = NULL;
    uart4Obj.rdInIndex = 0;
    uart4Obj.rdOutIndex = 0;
    uart4Obj.isRdNotificationEnabled = false;
    uart4Obj.isRdNotifyPersistently = false;
    uart4Obj.rdThreshold = 0;

    uart4Obj.wrCallback = NULL;
    uart4Obj.wrInIndex = 0;
    uart4Obj.wrOutIndex = 0;
    uart4Obj.isWrNotificationEnabled = false;
    uart4Obj.isWrNotifyPersistently = false;
    uart4Obj.wrThreshold = 0;

    uart4Obj.errors = UART_ERROR_NONE;

    if (UART4_IS_9BIT_MODE_ENABLED())
    {
        uart4Obj.rdBufferSize = UART4_READ_BUFFER_SIZE_9BIT;
        uart4Obj.wrBufferSize = UART4_WRITE_BUFFER_SIZE_9BIT;
    }
    else
    {
        uart4Obj.rdBufferSize = UART4_READ_BUFFER_SIZE;
        uart4Obj.wrBufferSize = UART4_WRITE_BUFFER_SIZE;
    }


    /* Turn ON UART4 */
    U4MODESET = _U4MODE_ON_MASK;

    /* Enable UART4_FAULT Interrupt */
    IEC5SET = _IEC5_U4EIE_MASK;

    /* Enable UART4_RX Interrupt */
    IEC5SET = _IEC5_U4RXIE_MASK;
}

bool UART4_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
{
    bool status = false;
    uint32_t baud;
    uint32_t status_ctrl;
    uint32_t uxbrg = 0;

    if (setup != NULL)
    {
        baud = setup->baudRate;

        if ((baud == 0U) || ((setup->dataWidth == UART_DATA_9_BIT) && (setup->parity != UART_PARITY_NONE)))
        {
            return status;
        }

        if(srcClkFreq == 0U)
        {
            srcClkFreq = UART4_FrequencyGet();
        }

        /* Calculate BRG value */
        uxbrg = (((srcClkFreq >> 2) + (baud >> 1)) / baud);

        /* Check if the baud value can be set with low baud settings */
        if (uxbrg < 1U)
        {
            return status;
        }

        uxbrg -= 1U;

        if (uxbrg > UINT16_MAX)
        {
            return status;
        }

        /* Turn OFF UART4. Save UTXEN, URXEN and UTXBRK bits as these are cleared upon disabling UART */

        status_ctrl = U4STA & (_U4STA_UTXEN_MASK | _U4STA_URXEN_MASK | _U4STA_UTXBRK_MASK);

        U4MODECLR = _U4MODE_ON_MASK;

        if(setup->dataWidth == UART_DATA_9_BIT)
        {
            /* Configure UART4 mode */
            U4MODE = (U4MODE & (~_U4MODE_PDSEL_MASK)) | setup->dataWidth;
        }
        else
        {
            /* Configure UART4 mode */
            U4MODE = (U4MODE & (~_U4MODE_PDSEL_MASK)) | setup->parity;
        }

        /* Configure UART4 mode */
        U4MODE = (U4MODE & (~_U4MODE_STSEL_MASK)) | setup->stopBits;

        /* Configure UART4 Baud Rate */
        U4BRG = uxbrg;

        if (UART4_IS_9BIT_MODE_ENABLED())
        {
            uart4Obj.rdBufferSize = UART4_READ_BUFFER_SIZE_9BIT;
            uart4Obj.wrBufferSize = UART4_WRITE_BUFFER_SIZE_9BIT;
        }
        else
        {
            uart4Obj.rdBufferSize = UART4_READ_BUFFER_SIZE;
            uart4Obj.wrBufferSize = UART4_WRITE_BUFFER_SIZE;
        }

        U4MODESET = _U4MODE_ON_MASK;

        /* Restore UTXEN, URXEN and UTXBRK bits. */
        U4STASET = status_ctrl;

        status = true;
    }

    return status;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool UART4_RxPushByte(uint16_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;
    uint32_t rdInIdx;

    tempInIndex = uart4Obj.rdInIndex + 1U;

    if (tempInIndex >= uart4Obj.rdBufferSize)
    {
        tempInIndex = 0U;
    }
    
    if (tempInIndex == uart4Obj.rdOutIndex)
    {
        tempInIndex = uart4Obj.rdInIndex + 1U;

        if (tempInIndex >= uart4Obj.rdBufferSize)
        {
            tempInIndex = 0U;
        }
    }

    /* Attempt to push the data into the ring buffer */
    if (tempInIndex != uart4Obj.rdOutIndex)
    {
        uint32_t rdInIndex = uart4Obj.rdInIndex;

        if (UART4_IS_9BIT_MODE_ENABLED())
        {
            rdInIdx = uart4Obj.rdInIndex << 1U;
            UART4_ReadBuffer[rdInIdx] = (uint8_t)rdByte;
            UART4_ReadBuffer[rdInIdx + 1U] = (uint8_t)(rdByte >> 8U);
        }
        else
        {
            UART4_ReadBuffer[rdInIndex] = (uint8_t)rdByte;
        }

        uart4Obj.rdInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }
    tx_semaphore_ceiling_put(&sem_uart4_rx_buffer,1);

    return isSuccess;
}

size_t UART4_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex = 0;
    uint32_t rdInIndex = 0;
    uint32_t rdOut16Idx;
    uint32_t nBytesRead16Idx;
    
    UART4_RX_INT_DISABLE();
    
    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = uart4Obj.rdOutIndex;
    rdInIndex = uart4Obj.rdInIndex;
    
    while (nBytesRead < size)
    {
        if (rdOutIndex != rdInIndex)
        {
            if (UART4_IS_9BIT_MODE_ENABLED())
            {
                rdOut16Idx = rdOutIndex << 1U;
                nBytesRead16Idx = nBytesRead << 1U;

                pRdBuffer[nBytesRead16Idx] = UART4_ReadBuffer[rdOut16Idx];
                pRdBuffer[nBytesRead16Idx + 1U] = UART4_ReadBuffer[rdOut16Idx + 1U];
            }
            else
            {
                pRdBuffer[nBytesRead] = UART4_ReadBuffer[rdOutIndex];
            }
            nBytesRead++;
            rdOutIndex++;

            if (rdOutIndex >= uart4Obj.rdBufferSize)
            {
                rdOutIndex = 0U;
            }
        }
        else
        {
            /* No more data available in the RX buffer */
            UART4_RX_INT_ENABLE();
            tx_semaphore_get(&sem_uart4_rx_buffer,TX_WAIT_FOREVER);
            UART4_RX_INT_DISABLE();
        }
    }

    uart4Obj.rdOutIndex = rdOutIndex;
    UART4_RX_INT_ENABLE();

    return nBytesRead;
}

size_t UART4_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdInIndex;
    uint32_t rdOutIndex;

    /* Take a snapshot of indices to avoid processing in critical section */
    rdInIndex = uart4Obj.rdInIndex;
    rdOutIndex = uart4Obj.rdOutIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex -  rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (uart4Obj.rdBufferSize -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t UART4_ReadFreeBufferCountGet(void)
{
    return (uart4Obj.rdBufferSize - 1U) - UART4_ReadCountGet();
}

size_t UART4_ReadBufferSizeGet(void)
{
    return (uart4Obj.rdBufferSize - 1U);
}

bool UART4_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = uart4Obj.isRdNotificationEnabled;

    uart4Obj.isRdNotificationEnabled = isEnabled;

    uart4Obj.isRdNotifyPersistently = isPersistent;

    return previousStatus;
}

void UART4_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0U)
    {
        uart4Obj.rdThreshold = nBytesThreshold;
    }
}


/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool UART4_TxPullByte(uint16_t* pWrByte)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = uart4Obj.wrOutIndex;
    uint32_t wrInIndex = uart4Obj.wrInIndex;
    uint32_t wrOut16Idx;

    if (wrOutIndex != wrInIndex)
    {
        if (UART4_IS_9BIT_MODE_ENABLED())
        {
            wrOut16Idx = wrOutIndex << 1U;
            pWrByte[0] = UART4_WriteBuffer[wrOut16Idx];
            pWrByte[1] = UART4_WriteBuffer[wrOut16Idx + 1U];
        }
        else
        {
            *pWrByte = UART4_WriteBuffer[wrOutIndex];
        }
        wrOutIndex++;

        if (wrOutIndex >= uart4Obj.wrBufferSize)
        {
            wrOutIndex = 0U;
        }

        uart4Obj.wrOutIndex = wrOutIndex;

        isSuccess = true;
    }

    return isSuccess;
}

static inline bool UART4_TxPushByte(uint16_t wrByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;
    uint32_t wrOutIndex = uart4Obj.wrOutIndex;
    uint32_t wrInIndex = uart4Obj.wrInIndex;
    uint32_t wrIn16Idx;

    tempInIndex = wrInIndex + 1U;

    if (tempInIndex >= uart4Obj.wrBufferSize)
    {
        tempInIndex = 0U;
    }
    if (tempInIndex != wrOutIndex)
    {
        if (UART4_IS_9BIT_MODE_ENABLED())
        {
            wrIn16Idx = wrInIndex << 1U;
            UART4_WriteBuffer[wrIn16Idx] = (uint8_t)wrByte;
            UART4_WriteBuffer[wrIn16Idx + 1U] = (uint8_t)(wrByte >> 8U);
        }
        else
        {
            UART4_WriteBuffer[wrInIndex] = (uint8_t)wrByte;
        }

        uart4Obj.wrInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void UART4_WriteNotificationSend(void)
{
    if(UART4_WriteFreeBufferCountGet() > 0)
        tx_semaphore_ceiling_put(&sem_uart4_tx_buffer,1);
}

static size_t UART4_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;

    /* Take a snapshot of indices to avoid processing in critical section */

    uint32_t wrOutIndex = uart4Obj.wrOutIndex;
    uint32_t wrInIndex = uart4Obj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex - wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (uart4Obj.wrBufferSize -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t UART4_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = UART4_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t UART4_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;
    uint16_t halfWordData = 0U;
    
    UART4_TX_INT_DISABLE();
    tx_semaphore_get(&sem_uart4_tx,TX_NO_WAIT);
    
    while (nBytesWritten < size)
    {
        if (UART4_IS_9BIT_MODE_ENABLED())
        {
            halfWordData = pWrBuffer[(2U * nBytesWritten) + 1U];
            halfWordData <<= 8U;
            halfWordData |= pWrBuffer[(2U * nBytesWritten)];
            if (UART4_TxPushByte(halfWordData) == true)
            {
                nBytesWritten++;
            }
            else
            {
                UART4_TX_INT_ENABLE();
                tx_semaphore_get(&sem_uart4_tx_buffer,TX_WAIT_FOREVER);
                UART4_TX_INT_DISABLE();
                tx_semaphore_get(&sem_uart4_tx,TX_NO_WAIT);
            }
        }
        else
        {
            if (UART4_TxPushByte(pWrBuffer[nBytesWritten]) == true)
            {
                nBytesWritten++;
            }
            else
            {
                UART4_TX_INT_ENABLE();
                tx_semaphore_get(&sem_uart4_tx_buffer,TX_WAIT_FOREVER);
                UART4_TX_INT_DISABLE();
                tx_semaphore_get(&sem_uart4_tx,TX_NO_WAIT);
            }
        }
    }

    /* Check if any data is pending for transmission */
    if (UART4_WritePendingBytesGet() > 0U)
    {
        /* Enable TX interrupt as data is pending for transmission */
        UART4_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t UART4_WriteFreeBufferCountGet(void)
{ return (uart4Obj.wrBufferSize - 1U) - UART4_WriteCountGet(); }

size_t UART4_WriteBufferSizeGet(void)
{ return (uart4Obj.wrBufferSize - 1U); }

bool UART4_TransmitComplete( void )
{ return (U4STA & _U4STA_TRMT_MASK) != 0U; }

void UART4_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0U)
    {
        uart4Obj.wrThreshold = nBytesThreshold;
    }
}

UART_ERROR UART4_ErrorGet( void )
{
    UART_ERROR errors = uart4Obj.errors;

    uart4Obj.errors = UART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool UART4_AutoBaudQuery( void )
{
    bool autobaudq_check = false;
    if((U4MODE & _U4MODE_ABAUD_MASK) != 0U)
    {
         autobaudq_check = true;
    }
     return autobaudq_check;
}

void UART4_AutoBaudSet( bool enable )
{
    if( enable == true )
    {
        U4MODESET = _U4MODE_ABAUD_MASK;
    }

    /* Turning off ABAUD if it was on can lead to unpredictable behavior, so that
       direction of control is not allowed in this function.                      */
}

void __attribute__((used)) UART4_FAULT_InterruptHandler (void)
{
    /* Save the error to be reported later */
    uart4Obj.errors = (UART_ERROR)(U4STA & (_U4STA_OERR_MASK | _U4STA_FERR_MASK | _U4STA_PERR_MASK));

    UART4_ErrorClear();

    /* Client must call UARTx_ErrorGet() function to clear the errors */
    if( uart4Obj.rdCallback != NULL )
    {
        uintptr_t rdContext = uart4Obj.rdContext;

        uart4Obj.rdCallback(UART_EVENT_READ_ERROR, rdContext);
    }
}

void __attribute__((used)) UART4_RX_InterruptHandler (void)
{
    /* Keep reading until there is a character availabe in the RX FIFO */
    while((U4STA & _U4STA_URXDA_MASK) == _U4STA_URXDA_MASK)
    {
        UART4_RxPushByte( (uint16_t )(U4RXREG) );
    }

    /* Clear UART4 RX Interrupt flag */
    IFS5CLR = _IFS5_U4RXIF_MASK;
}

void UART4_Flush()
{
    if(!UART4_WritePendingBytesGet())
        return;
    tx_semaphore_get(&sem_uart4_tx,TX_WAIT_FOREVER);
}

void __attribute__((used)) UART4_TX_InterruptHandler (void)
{
    uint16_t wrByte;

    /* Check if any data is pending for transmission */
    if (UART4_WritePendingBytesGet() > 0U)
    {
        /* Keep writing to the TX FIFO as long as there is space */
        while((U4STA & _U4STA_UTXBF_MASK) == 0U)
        {
            if (UART4_TxPullByte(&wrByte) == true)
            {
                if (UART4_IS_9BIT_MODE_ENABLED())
                {
                    U4TXREG = wrByte;
                }
                else
                {
                    U4TXREG = (uint8_t)wrByte;
                }

                /* Send notification */
                UART4_WriteNotificationSend();
            }
            else
            {
                /* Nothing to transmit. Disable the data register empty interrupt. */
                tx_semaphore_ceiling_put(&sem_uart4_tx,1);
                UART4_TX_INT_DISABLE();
                break;
            }
        }

        /* Clear UART4TX Interrupt flag */
        IFS5CLR = _IFS5_U4TXIF_MASK;
    }
    else
    {
        /* Nothing to transmit. Disable the data register empty interrupt. */
        UART4_TX_INT_DISABLE();

        /* Clear UART4TX Interrupt flag */
        IFS5CLR = _IFS5_U4TXIF_MASK;
    }
}

