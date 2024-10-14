/*******************************************************************************
  UART3 PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_uart3.c

  Summary:
    UART3 PLIB Implementation File

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
#include "plib_uart3.h"
#include "interrupts.h"
#include "tx_api.h"

// *****************************************************************************
// *****************************************************************************
// Section: UART3 Implementation
// *****************************************************************************
// *****************************************************************************

volatile static UART_RING_BUFFER_OBJECT uart3Obj;

#define UART3_READ_BUFFER_SIZE      (128U)
#define UART3_READ_BUFFER_SIZE_9BIT (128U >> 1)
#define UART3_RX_INT_DISABLE()      IEC4CLR = _IEC4_U3RXIE_MASK;
#define UART3_RX_INT_ENABLE()       IEC4SET = _IEC4_U3RXIE_MASK;

volatile static uint8_t UART3_ReadBuffer[UART3_READ_BUFFER_SIZE];

#define UART3_WRITE_BUFFER_SIZE      (128U)
#define UART3_WRITE_BUFFER_SIZE_9BIT (128U >> 1)
#define UART3_TX_INT_DISABLE()       IEC4CLR = _IEC4_U3TXIE_MASK;
#define UART3_TX_INT_ENABLE()        IEC4SET = _IEC4_U3TXIE_MASK;

volatile static uint8_t UART3_WriteBuffer[UART3_WRITE_BUFFER_SIZE];

#define UART3_IS_9BIT_MODE_ENABLED()    ( (U3MODE) & (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK)) == (_U3MODE_PDSEL0_MASK | _U3MODE_PDSEL1_MASK) ? true:false

void static UART3_ErrorClear( void )
{
    UART_ERROR errors = UART_ERROR_NONE;
    uint8_t dummyData = 0u;

    errors = (UART_ERROR)(U3STA & (_U3STA_OERR_MASK | _U3STA_FERR_MASK | _U3STA_PERR_MASK));

    if(errors != UART_ERROR_NONE)
    {
        /* If it's a overrun error then clear it to flush FIFO */
        if((U3STA & _U3STA_OERR_MASK) != 0U)
        {
            U3STACLR = _U3STA_OERR_MASK;
        }

        /* Read existing error bytes from FIFO to clear parity and framing error flags */
        while((U3STA & _U3STA_URXDA_MASK) != 0U)
        {
            dummyData = (uint8_t)U3RXREG;
        }

        /* Clear error interrupt flag */
        IFS4CLR = _IFS4_U3EIF_MASK;

        /* Clear up the receive interrupt flag so that RX interrupt is not
         * triggered for error bytes */
        IFS4CLR = _IFS4_U3RXIF_MASK;

    }

    // Ignore the warning
    (void)dummyData;
}

static TX_SEMAPHORE sem_uart3_tx_buffer, sem_uart3_tx, sem_uart3_rx_buffer;

void UART3_Initialize( void )
{
    tx_semaphore_create(&sem_uart3_tx_buffer,"",1);
    tx_semaphore_create(&sem_uart3_tx,"",0);
    tx_semaphore_create(&sem_uart3_rx_buffer,"",0);
    /* Set up UxMODE bits */
    /* STSEL  = 0 */
    /* PDSEL = 0 */

    U3MODE = 0x8;

    /* Enable UART3 Receiver and Transmitter */
    U3STASET = (_U3STA_UTXEN_MASK | _U3STA_URXEN_MASK | _U3STA_UTXISEL1_MASK );

    /* BAUD Rate register Setup */
    U3BRG = 216;

    /* Disable Interrupts */
    IEC4CLR = _IEC4_U3EIE_MASK;

    IEC4CLR = _IEC4_U3RXIE_MASK;

    IEC4CLR = _IEC4_U3TXIE_MASK;

    /* Initialize instance object */
    uart3Obj.rdCallback = NULL;
    uart3Obj.rdInIndex = 0;
    uart3Obj.rdOutIndex = 0;
    uart3Obj.isRdNotificationEnabled = false;
    uart3Obj.isRdNotifyPersistently = false;
    uart3Obj.rdThreshold = 0;

    uart3Obj.wrCallback = NULL;
    uart3Obj.wrInIndex = 0;
    uart3Obj.wrOutIndex = 0;
    uart3Obj.isWrNotificationEnabled = false;
    uart3Obj.isWrNotifyPersistently = false;
    uart3Obj.wrThreshold = 0;

    uart3Obj.errors = UART_ERROR_NONE;

    if (UART3_IS_9BIT_MODE_ENABLED())
    {
        uart3Obj.rdBufferSize = UART3_READ_BUFFER_SIZE_9BIT;
        uart3Obj.wrBufferSize = UART3_WRITE_BUFFER_SIZE_9BIT;
    }
    else
    {
        uart3Obj.rdBufferSize = UART3_READ_BUFFER_SIZE;
        uart3Obj.wrBufferSize = UART3_WRITE_BUFFER_SIZE;
    }


    /* Turn ON UART3 */
    U3MODESET = _U3MODE_ON_MASK;

    /* Enable UART3_FAULT Interrupt */
    IEC4SET = _IEC4_U3EIE_MASK;

    /* Enable UART3_RX Interrupt */
    IEC4SET = _IEC4_U3RXIE_MASK;
}

bool UART3_SerialSetup( UART_SERIAL_SETUP *setup, uint32_t srcClkFreq )
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
            srcClkFreq = UART3_FrequencyGet();
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

        /* Turn OFF UART3. Save UTXEN, URXEN and UTXBRK bits as these are cleared upon disabling UART */

        status_ctrl = U3STA & (_U3STA_UTXEN_MASK | _U3STA_URXEN_MASK | _U3STA_UTXBRK_MASK);

        U3MODECLR = _U3MODE_ON_MASK;

        if(setup->dataWidth == UART_DATA_9_BIT)
        {
            /* Configure UART3 mode */
            U3MODE = (U3MODE & (~_U3MODE_PDSEL_MASK)) | setup->dataWidth;
        }
        else
        {
            /* Configure UART3 mode */
            U3MODE = (U3MODE & (~_U3MODE_PDSEL_MASK)) | setup->parity;
        }

        /* Configure UART3 mode */
        U3MODE = (U3MODE & (~_U3MODE_STSEL_MASK)) | setup->stopBits;

        /* Configure UART3 Baud Rate */
        U3BRG = uxbrg;

        if (UART3_IS_9BIT_MODE_ENABLED())
        {
            uart3Obj.rdBufferSize = UART3_READ_BUFFER_SIZE_9BIT;
            uart3Obj.wrBufferSize = UART3_WRITE_BUFFER_SIZE_9BIT;
        }
        else
        {
            uart3Obj.rdBufferSize = UART3_READ_BUFFER_SIZE;
            uart3Obj.wrBufferSize = UART3_WRITE_BUFFER_SIZE;
        }

        U3MODESET = _U3MODE_ON_MASK;

        /* Restore UTXEN, URXEN and UTXBRK bits. */
        U3STASET = status_ctrl;

        status = true;
    }

    return status;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool UART3_RxPushByte(uint16_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;
    uint32_t rdInIdx;

    tempInIndex = uart3Obj.rdInIndex + 1U;

    if (tempInIndex >= uart3Obj.rdBufferSize)
    {
        tempInIndex = 0U;
    }
    
    if (tempInIndex == uart3Obj.rdOutIndex)
    {
        tempInIndex = uart3Obj.rdInIndex + 1U;

        if (tempInIndex >= uart3Obj.rdBufferSize)
        {
            tempInIndex = 0U;
        }
    }

    /* Attempt to push the data into the ring buffer */
    if (tempInIndex != uart3Obj.rdOutIndex)
    {
        uint32_t rdInIndex = uart3Obj.rdInIndex;

        if (UART3_IS_9BIT_MODE_ENABLED())
        {
            rdInIdx = uart3Obj.rdInIndex << 1U;
            UART3_ReadBuffer[rdInIdx] = (uint8_t)rdByte;
            UART3_ReadBuffer[rdInIdx + 1U] = (uint8_t)(rdByte >> 8U);
        }
        else
        {
            UART3_ReadBuffer[rdInIndex] = (uint8_t)rdByte;
        }

        uart3Obj.rdInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }
    tx_semaphore_ceiling_put(&sem_uart3_rx_buffer,1);

    return isSuccess;
}

size_t UART3_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex = 0;
    uint32_t rdInIndex = 0;
    uint32_t rdOut16Idx;
    uint32_t nBytesRead16Idx;
    
    UART3_RX_INT_DISABLE();
    
    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = uart3Obj.rdOutIndex;
    rdInIndex = uart3Obj.rdInIndex;
    
    while (nBytesRead < size)
    {
        if (rdOutIndex != rdInIndex)
        {
            if (UART3_IS_9BIT_MODE_ENABLED())
            {
                rdOut16Idx = rdOutIndex << 1U;
                nBytesRead16Idx = nBytesRead << 1U;

                pRdBuffer[nBytesRead16Idx] = UART3_ReadBuffer[rdOut16Idx];
                pRdBuffer[nBytesRead16Idx + 1U] = UART3_ReadBuffer[rdOut16Idx + 1U];
            }
            else
            {
                pRdBuffer[nBytesRead] = UART3_ReadBuffer[rdOutIndex];
            }
            nBytesRead++;
            rdOutIndex++;

            if (rdOutIndex >= uart3Obj.rdBufferSize)
            {
                rdOutIndex = 0U;
            }
        }
        else
        {
            /* No more data available in the RX buffer */
            UART3_RX_INT_ENABLE();
            tx_semaphore_get(&sem_uart3_rx_buffer,TX_WAIT_FOREVER);
            UART3_RX_INT_DISABLE();
        }
    }

    uart3Obj.rdOutIndex = rdOutIndex;
    UART3_RX_INT_ENABLE();

    return nBytesRead;
}

size_t UART3_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdInIndex;
    uint32_t rdOutIndex;

    /* Take a snapshot of indices to avoid processing in critical section */
    rdInIndex = uart3Obj.rdInIndex;
    rdOutIndex = uart3Obj.rdOutIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex -  rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (uart3Obj.rdBufferSize -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t UART3_ReadFreeBufferCountGet(void)
{
    return (uart3Obj.rdBufferSize - 1U) - UART3_ReadCountGet();
}

size_t UART3_ReadBufferSizeGet(void)
{
    return (uart3Obj.rdBufferSize - 1U);
}

bool UART3_ReadNotificationEnable(bool isEnabled, bool isPersistent)
{
    bool previousStatus = uart3Obj.isRdNotificationEnabled;

    uart3Obj.isRdNotificationEnabled = isEnabled;

    uart3Obj.isRdNotifyPersistently = isPersistent;

    return previousStatus;
}

void UART3_ReadThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0U)
    {
        uart3Obj.rdThreshold = nBytesThreshold;
    }
}


/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static bool UART3_TxPullByte(uint16_t* pWrByte)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = uart3Obj.wrOutIndex;
    uint32_t wrInIndex = uart3Obj.wrInIndex;
    uint32_t wrOut16Idx;

    if (wrOutIndex != wrInIndex)
    {
        if (UART3_IS_9BIT_MODE_ENABLED())
        {
            wrOut16Idx = wrOutIndex << 1U;
            pWrByte[0] = UART3_WriteBuffer[wrOut16Idx];
            pWrByte[1] = UART3_WriteBuffer[wrOut16Idx + 1U];
        }
        else
        {
            *pWrByte = UART3_WriteBuffer[wrOutIndex];
        }
        wrOutIndex++;

        if (wrOutIndex >= uart3Obj.wrBufferSize)
        {
            wrOutIndex = 0U;
        }

        uart3Obj.wrOutIndex = wrOutIndex;

        isSuccess = true;
    }

    return isSuccess;
}

static inline bool UART3_TxPushByte(uint16_t wrByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;
    uint32_t wrOutIndex = uart3Obj.wrOutIndex;
    uint32_t wrInIndex = uart3Obj.wrInIndex;
    uint32_t wrIn16Idx;

    tempInIndex = wrInIndex + 1U;

    if (tempInIndex >= uart3Obj.wrBufferSize)
    {
        tempInIndex = 0U;
    }
    if (tempInIndex != wrOutIndex)
    {
        if (UART3_IS_9BIT_MODE_ENABLED())
        {
            wrIn16Idx = wrInIndex << 1U;
            UART3_WriteBuffer[wrIn16Idx] = (uint8_t)wrByte;
            UART3_WriteBuffer[wrIn16Idx + 1U] = (uint8_t)(wrByte >> 8U);
        }
        else
        {
            UART3_WriteBuffer[wrInIndex] = (uint8_t)wrByte;
        }

        uart3Obj.wrInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}

/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static void UART3_WriteNotificationSend(void)
{
    if(UART3_WriteFreeBufferCountGet() > 0)
        tx_semaphore_ceiling_put(&sem_uart3_tx_buffer,1);
}

static size_t UART3_WritePendingBytesGet(void)
{
    size_t nPendingTxBytes;

    /* Take a snapshot of indices to avoid processing in critical section */

    uint32_t wrOutIndex = uart3Obj.wrOutIndex;
    uint32_t wrInIndex = uart3Obj.wrInIndex;

    if ( wrInIndex >=  wrOutIndex)
    {
        nPendingTxBytes =  wrInIndex - wrOutIndex;
    }
    else
    {
        nPendingTxBytes =  (uart3Obj.wrBufferSize -  wrOutIndex) + wrInIndex;
    }

    return nPendingTxBytes;
}

size_t UART3_WriteCountGet(void)
{
    size_t nPendingTxBytes;

    nPendingTxBytes = UART3_WritePendingBytesGet();

    return nPendingTxBytes;
}

size_t UART3_Write(uint8_t* pWrBuffer, const size_t size )
{
    size_t nBytesWritten  = 0;
    uint16_t halfWordData = 0U;
    
    UART3_TX_INT_DISABLE();
    tx_semaphore_get(&sem_uart3_tx,TX_NO_WAIT);
    
    while (nBytesWritten < size)
    {
        if (UART3_IS_9BIT_MODE_ENABLED())
        {
            halfWordData = pWrBuffer[(2U * nBytesWritten) + 1U];
            halfWordData <<= 8U;
            halfWordData |= pWrBuffer[(2U * nBytesWritten)];
            if (UART3_TxPushByte(halfWordData) == true)
            {
                nBytesWritten++;
            }
            else
            {
                UART3_TX_INT_ENABLE();
                tx_semaphore_get(&sem_uart3_tx_buffer,TX_WAIT_FOREVER);
                UART3_TX_INT_DISABLE();
                tx_semaphore_get(&sem_uart3_tx,TX_NO_WAIT);
            }
        }
        else
        {
            if (UART3_TxPushByte(pWrBuffer[nBytesWritten]) == true)
            {
                nBytesWritten++;
            }
            else
            {
                UART3_TX_INT_ENABLE();
                tx_semaphore_get(&sem_uart3_tx_buffer,TX_WAIT_FOREVER);
                UART3_TX_INT_DISABLE();
                tx_semaphore_get(&sem_uart3_tx,TX_NO_WAIT);
            }
        }
    }

    /* Check if any data is pending for transmission */
    if (UART3_WritePendingBytesGet() > 0U)
    {
        /* Enable TX interrupt as data is pending for transmission */
        UART3_TX_INT_ENABLE();
    }

    return nBytesWritten;
}

size_t UART3_WriteFreeBufferCountGet(void)
{ return (uart3Obj.wrBufferSize - 1U) - UART3_WriteCountGet(); }

size_t UART3_WriteBufferSizeGet(void)
{ return (uart3Obj.wrBufferSize - 1U); }

bool UART3_TransmitComplete( void )
{ return ((U3STA & _U3STA_TRMT_MASK) != 0U); }

void UART3_WriteThresholdSet(uint32_t nBytesThreshold)
{
    if (nBytesThreshold > 0U)
    {
        uart3Obj.wrThreshold = nBytesThreshold;
    }
}

UART_ERROR UART3_ErrorGet( void )
{
    UART_ERROR errors = uart3Obj.errors;

    uart3Obj.errors = UART_ERROR_NONE;

    /* All errors are cleared, but send the previous error state */
    return errors;
}

bool UART3_AutoBaudQuery( void )
{
    bool autobaudq_check = false;
    if((U3MODE & _U3MODE_ABAUD_MASK) != 0U)
    {
         autobaudq_check = true;
    }
     return autobaudq_check;
}

void UART3_AutoBaudSet( bool enable )
{
    if( enable == true )
    {
        U3MODESET = _U3MODE_ABAUD_MASK;
    }

    /* Turning off ABAUD if it was on can lead to unpredictable behavior, so that
       direction of control is not allowed in this function.                      */
}

void __attribute__((used)) UART3_FAULT_InterruptHandler (void)
{
    /* Save the error to be reported later */
    uart3Obj.errors = (UART_ERROR)(U3STA & (_U3STA_OERR_MASK | _U3STA_FERR_MASK | _U3STA_PERR_MASK));

    UART3_ErrorClear();

    /* Client must call UARTx_ErrorGet() function to clear the errors */
    if( uart3Obj.rdCallback != NULL )
    {
        uintptr_t rdContext = uart3Obj.rdContext;

        uart3Obj.rdCallback(UART_EVENT_READ_ERROR, rdContext);
    }
}

void __attribute__((used)) UART3_RX_InterruptHandler (void)
{
    /* Keep reading until there is a character availabe in the RX FIFO */
    while((U3STA & _U3STA_URXDA_MASK) == _U3STA_URXDA_MASK)
    {
        UART3_RxPushByte( (uint16_t )(U3RXREG));
    }

    /* Clear UART3 RX Interrupt flag */
    IFS4CLR = _IFS4_U3RXIF_MASK;
}

void UART3_Flush()
{
    if(!UART3_WritePendingBytesGet())
        return;
    tx_semaphore_get(&sem_uart3_tx,TX_WAIT_FOREVER);
}

void __attribute__((used)) UART3_TX_InterruptHandler (void)
{
    uint16_t wrByte;

    /* Check if any data is pending for transmission */
    if (UART3_WritePendingBytesGet() > 0U)
    {
        /* Keep writing to the TX FIFO as long as there is space */
        while((U3STA & _U3STA_UTXBF_MASK) == 0U)
        {
            if (UART3_TxPullByte(&wrByte) == true)
            {
                if (UART3_IS_9BIT_MODE_ENABLED())
                {
                    U3TXREG = wrByte;
                }
                else
                {
                    U3TXREG = (uint8_t)wrByte;
                }

                /* Send notification */
                UART3_WriteNotificationSend();
            }
            else
            {
                /* Nothing to transmit. Disable the data register empty interrupt. */
                tx_semaphore_ceiling_put(&sem_uart3_tx,1);
                UART3_TX_INT_DISABLE();
                break;
            }
        }

        /* Clear UART3TX Interrupt flag */
        IFS4CLR = _IFS4_U3TXIF_MASK;
    }
    else
    {
        /* Nothing to transmit. Disable the data register empty interrupt. */
        UART3_TX_INT_DISABLE();

        /* Clear UART3TX Interrupt flag */
        IFS4CLR = _IFS4_U3TXIF_MASK;
    }
}

