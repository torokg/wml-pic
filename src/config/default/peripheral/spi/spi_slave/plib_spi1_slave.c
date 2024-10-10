/*******************************************************************************
  SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_spi1_slave.c

  Summary:
    SPI1 Slave Source File

  Description:
    This file has implementation of all the interfaces provided for particular
    SPI peripheral.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019-2020 Microchip Technology Inc. and its subsidiaries.
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
#if 0
#include "plib_spi1_slave.h"
#include "peripheral/gpio/plib_gpio.h"
#include <string.h>
#include "interrupts.h"
#include "tx_api.h"
//#include "log.h"


#define LOG_WRITE(...)
// *****************************************************************************
// *****************************************************************************
// Section: SPI1 Slave Implementation
// *****************************************************************************
// *****************************************************************************
#define SPI1_CS_PIN                      GPIO_PIN_RD12


#define SPI1_READ_BUFFER_SIZE            256
#define SPI1_WRITE_BUFFER_SIZE           256

volatile static uint8_t SPI1_ReadBuffer[SPI1_READ_BUFFER_SIZE];
volatile static uint8_t SPI1_WriteBuffer[SPI1_WRITE_BUFFER_SIZE];


/* Global object to save SPI Exchange related data */
volatile static SPI_SLAVE_OBJECT spi1Obj;

#define SPI1_CON_CKP                        (0UL << _SPI1CON_CKP_POSITION)
#define SPI1_CON_CKE                        (1UL << _SPI1CON_CKE_POSITION)
#define SPI1_CON_MODE_32_MODE_16            (0UL << _SPI1CON_MODE16_POSITION)
#define SPI1_CON_ENHBUF                     (1UL << _SPI1CON_ENHBUF_POSITION)
#define SPI1_CON_STXISEL                    (3UL << _SPI1CON_STXISEL_POSITION)
#define SPI1_CON_SRXISEL                    (1UL << _SPI1CON_SRXISEL_POSITION)
#define SPI1_CON_SSEN                       (1UL << _SPI1CON_SSEN_POSITION)

#define SPI1_ENABLE_RX_INT()                IEC3SET = 0x4000
#define SPI1_CLEAR_RX_INT_FLAG()            IFS3CLR = 0x4000

#define SPI1_DISABLE_TX_INT()               IEC3CLR = 0x8000
#define SPI1_ENABLE_TX_INT()                IEC3SET = 0x8000
#define SPI1_CLEAR_TX_INT_FLAG()            IFS3CLR = 0x8000

#define SPI1_ENABLE_ERR_INT()               IEC3SET = 0x2000
#define SPI1_CLEAR_ERR_INT_FLAG()           IFS3CLR = 0x2000

/* Forward declarations */
static void SPI1_CS_Handler(GPIO_PIN pin, uintptr_t context);

static void mem_copy(volatile void* pDst, volatile void* pSrc, uint32_t nBytes)
{
    volatile uint8_t* pSource = (volatile uint8_t*)pSrc;
    volatile uint8_t* pDest = (volatile uint8_t*)pDst;

    for (uint32_t i = 0U; i < nBytes; i++)
    {
        pDest[i] = pSource[i];
    }
}

void SPI1_Initialize ( void )
{
    /* Disable SPI1 Interrupts */
    IEC3CLR = 0x2000;
    IEC3CLR = 0x4000;
    IEC3CLR = 0x8000;

    /* STOP and Reset the SPI */
    SPI1CON = 0;

    /* Clear SPI1 Interrupt flags */
    IFS3CLR = 0x2000;
    IFS3CLR = 0x4000;
    IFS3CLR = 0x8000;

    /* CLear the receiver overflow error flag */
    SPI1STATCLR = _SPI1STAT_SPIROV_MASK;

    /*
    SRXISEL = 1 (Receive buffer is not empty)
    STXISEL = 3 (Transmit buffer is not full)
    MSTEN = 0
    CKP = 0
    CKE = 1
    MODE< 32,16 > = 0
    ENHBUF = 1
    */

    SPI1CONSET = (SPI1_CON_ENHBUF | SPI1_CON_MODE_32_MODE_16 | SPI1_CON_CKE | SPI1_CON_CKP | SPI1_CON_SSEN | SPI1_CON_STXISEL | SPI1_CON_SRXISEL);

    /* Enable generation of interrupt on receiver overflow */
    SPI1CON2SET = _SPI1CON2_SPIROVEN_MASK;

    spi1Obj.rdInIndex = 0;
    spi1Obj.wrOutIndex = 0;
    spi1Obj.nWrBytes = 0;
    spi1Obj.errorStatus = SPI_SLAVE_ERROR_NONE;
    spi1Obj.callback = NULL ;
    spi1Obj.transferIsBusy = false ;
    spi1Obj.csInterruptPending = false;
    spi1Obj.rxInterruptActive = false;


    /* Register callback and enable notifications on Chip Select logic level change */
    (void)GPIO_PinInterruptCallbackRegister(SPI1_CS_PIN, SPI1_CS_Handler, 0U);
    GPIO_PinInterruptEnable(SPI1_CS_PIN);

    /* Enable SPI1 RX and Error Interrupts. TX interrupt will be enabled when a SPI write is submitted. */
    SPI1_ENABLE_RX_INT();
    SPI1_ENABLE_ERR_INT();

    /* Enable SPI1 */
    SPI1CONSET = _SPI1CON_ON_MASK;
}

/* For 16-bit/32-bit mode, the "size" must be specified in terms of 16-bit/32-bit words */
size_t SPI1_Read(void* pRdBuffer, size_t size)
{
    size_t rdSize = size;
    uint32_t rdInIndex = spi1Obj.rdInIndex;

    if (rdSize > rdInIndex)
    {
        rdSize = rdInIndex;
    }

   (void) mem_copy(pRdBuffer, SPI1_ReadBuffer, rdSize);

    return rdSize;
}

/* For 16-bit/32-bit mode, the "size" must be specified in terms of 16-bit/32-bit words */
size_t SPI1_Write(void* pWrBuffer, size_t size )
{
    size_t wrSize = size;
    size_t wrOutIndex = 0;

    SPI1_DISABLE_TX_INT();

    if (wrSize > SPI1_WRITE_BUFFER_SIZE)
    {
        wrSize = SPI1_WRITE_BUFFER_SIZE;
    }

    (void) mem_copy(SPI1_WriteBuffer, pWrBuffer, wrSize);

    spi1Obj.wrOutIndex = wrOutIndex;
    spi1Obj.nWrBytes = wrSize;

    /* Fill up the FIFO as long as there are empty elements */
    /*while ((!(SPI1STAT & _SPI1STAT_SPITBF_MASK)) && (wrOutIndex < wrSize))
    {
        SPI1BUF = SPI1_WriteBuffer[wrOutIndex];
        wrOutIndex++;
    }*/

    

    SPI1CONSET = 1<<(_SPI1CON_STXISEL_POSITION+1);
    /* Enable TX interrupt */
    SPI1_ENABLE_TX_INT();

    return wrSize;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI1_ReadCountGet(void)
{
    return spi1Obj.rdInIndex;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI1_ReadBufferSizeGet(void)
{
    return SPI1_READ_BUFFER_SIZE;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI1_WriteBufferSizeGet(void)
{
    return SPI1_WRITE_BUFFER_SIZE;
}

bool SPI1_WriteComplete(void)
{
    return spi1Obj.wrOutIndex == spi1Obj.nWrBytes;
}

void SPI1_CallbackRegister(SPI_SLAVE_CALLBACK callBack, uintptr_t context )
{
    spi1Obj.callback = callBack;

    spi1Obj.context = context;
}

/* The status is returned as busy when CS is asserted */
bool SPI1_IsBusy(void)
{
    return spi1Obj.transferIsBusy;
}


SPI_SLAVE_ERROR SPI1_ErrorGet(void)
{
    SPI_SLAVE_ERROR errorStatus = spi1Obj.errorStatus;

    spi1Obj.errorStatus = SPI_SLAVE_ERROR_NONE;

    return errorStatus;
}

static void __attribute__((used)) SPI1_CS_Handler(GPIO_PIN pin, uintptr_t context)
{
    bool activeState = false;
    LOG_WRITE("spi slave cs interrupt\n");

    if (GPIO_PinRead((GPIO_PIN)SPI1_CS_PIN) == activeState)
    {
        /* CS is asserted */
        spi1Obj.transferIsBusy = true;

    }
    else
    {
        /* Give application callback only if RX interrupt is not preempted and RX interrupt is not pending to be serviced */

        bool rxInterruptActive = spi1Obj.rxInterruptActive;

        if (((IFS3 & _IFS3_SPI1RXIF_MASK) == 0) && (rxInterruptActive == false))
        {
            /* CS is de-asserted */
            spi1Obj.transferIsBusy = false;
            if(spi1Obj.wrOutIndex != spi1Obj.nWrBytes)
            {
                spi1Obj.wrOutIndex = spi1Obj.nWrBytes = 0;
                if(spi1Obj.callback != NULL)
                {
                    uintptr_t context = spi1Obj.context;

                    spi1Obj.callback(context,SPI_TRANSMISSION_FAILED);
                }
            }
            spi1Obj.wrOutIndex = 0;
            spi1Obj.nWrBytes = 0;

            if(spi1Obj.callback != NULL)
            {
                uintptr_t context_val = spi1Obj.context;

                spi1Obj.callback(context_val, SPI_TRANSER_FINISHED);
            }

            /* Clear the read index. Application must read out the data by calling SPI1_Read API in the callback */
            spi1Obj.rdInIndex = 0;
        }
        else
        {
            /* If CS interrupt is serviced by either preempting the RX interrupt or RX interrupt is pending to be serviced,
             * then delegate the responsibility of giving application callback to the RX interrupt handler */

            spi1Obj.csInterruptPending = true;
        }
    }
}

void __attribute__((used)) SPI1_FAULT_InterruptHandler (void)
{
    LOG_WRITE("spi slave fault interrupt\n");
    spi1Obj.errorStatus =(SPI1STAT & _SPI1STAT_SPIROV_MASK);

    /* Clear the receive overflow flag */
    SPI1STATCLR = _SPI1STAT_SPIROV_MASK;

    SPI1_CLEAR_ERR_INT_FLAG();
}

void __attribute__((used)) SPI1_TX_InterruptHandler (void)
{
    if(!(SPI1CON & 1<<(_SPI1CON_STXISEL_POSITION+1)))
    {
        SPI1BUF = 0xff;
        SPI1_CLEAR_TX_INT_FLAG();
        return;
    }
    
    LOG_WRITE("spi slave tx interrupt\n");
    size_t wrOutIndex = spi1Obj.wrOutIndex;
    size_t nWrBytes = spi1Obj.nWrBytes;
    

    if((spi1Obj.callback != NULL) && nWrBytes)
    {
        uintptr_t context_val = spi1Obj.context;
        spi1Obj.callback(context_val, SPI_TRANSER_STARTED);
    }

    /* Fill up the FIFO as long as there are empty elements */
    while ((!(SPI1STAT & _SPI1STAT_SPITBF_MASK)) && (wrOutIndex < nWrBytes))
    {
        SPI1BUF = SPI1_WriteBuffer[wrOutIndex];
        wrOutIndex++;
    }
    
    
    spi1Obj.wrOutIndex = wrOutIndex;

    /* Clear the transmit interrupt flag */
    SPI1_CLEAR_TX_INT_FLAG();

    if (wrOutIndex >= nWrBytes)
    {
        if (wrOutIndex > nWrBytes)
        {
            if((spi1Obj.callback != NULL))
            {
                uintptr_t context_val = spi1Obj.context;

                spi1Obj.callback(context_val, SPI_TRANSMISSION_FAILED);
            }
        }
        spi1Obj.wrOutIndex = spi1Obj.nWrBytes = 0;
        //SPI1_DISABLE_TX_INT();
        SPI1CONCLR = 1<<(_SPI1CON_STXISEL_POSITION+1);
    }
}

void __attribute__((used)) SPI1_RX_InterruptHandler (void)
{
    LOG_WRITE("spi slave rx interrupt\n");
    uint32_t receivedData = 0;

    spi1Obj.rxInterruptActive = true;

    size_t rdInIndex = spi1Obj.rdInIndex;

    while (!(SPI1STAT & _SPI1STAT_SPIRBE_MASK))
    {
        /* Receive buffer is not empty. Read the received data. */
        receivedData = SPI1BUF;

        if (rdInIndex < SPI1_READ_BUFFER_SIZE)
        {
            SPI1_ReadBuffer[rdInIndex] = (uint8_t)receivedData;
            rdInIndex++;
        }
    }
    
    spi1Obj.rdInIndex = rdInIndex;

    /* Clear the receive interrupt flag */
    SPI1_CLEAR_RX_INT_FLAG();
    
    spi1Obj.rxInterruptActive = false;

    /* Check if CS interrupt occured before the RX interrupt and that CS interrupt delegated the responsibility to give
     * application callback to the RX interrupt */

    if (spi1Obj.csInterruptPending == true)
    {
        spi1Obj.csInterruptPending = false;
        spi1Obj.transferIsBusy = false;

        if(spi1Obj.wrOutIndex != spi1Obj.nWrBytes)
        {
            spi1Obj.wrOutIndex = spi1Obj.nWrBytes = 0;
            if(spi1Obj.callback != NULL)
            {
                uintptr_t context = spi1Obj.context;

                spi1Obj.callback(context,SPI_TRANSMISSION_FAILED);
            }
        }
        
        if(spi1Obj.callback != NULL)
        {
            uintptr_t context = spi1Obj.context;

            spi1Obj.callback(context,SPI_TRANSER_FINISHED);
        }

        /* Clear the read index. Application must read out the data by calling SPI1_Read API in the callback */
        spi1Obj.rdInIndex = 0;
    }
}
*/
#endif