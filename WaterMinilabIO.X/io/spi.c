#include "peripheral/gpio/plib_gpio.h"
#include <string.h>
#include "interrupts.h"
#include "tx_api.h"

#define SPI1_CS_PIN                      GPIO_PIN_RD12

/* Global object to save SPI Exchange related data */
#define SPI1_CON_CKP                        (0UL << _SPI1CON_CKP_POSITION)
#define SPI1_CON_CKE                        (1UL << _SPI1CON_CKE_POSITION)
#define SPI1_CON_MODE_32_MODE_16            (0UL << _SPI1CON_MODE16_POSITION)
#define SPI1_CON_ENHBUF                     (1UL << _SPI1CON_ENHBUF_POSITION)
#define SPI1_CON_STXISEL                    (3UL << _SPI1CON_STXISEL_POSITION)
#define SPI1_CON_SRXISEL                    (1UL << _SPI1CON_SRXISEL_POSITION)
#define SPI1_CON_SSEN                       (1UL << _SPI1CON_SSEN_POSITION)

#define SPI1_ENABLE_RX_INT()                IEC3SET = 0x4000
#define SPI1_DISABLE_RX_INT()               IEC3CLR = 0x4000
#define SPI1_CLEAR_RX_INT_FLAG()            IFS3CLR = 0x4000

#define SPI1_ENABLE_TX_INT()                IEC3SET = 0x8000
#define SPI1_DISABLE_TX_INT()               IEC3CLR = 0x8000
#define SPI1_CLEAR_TX_INT_FLAG()            IFS3CLR = 0x8000

#define SPI1_ENABLE_ERR_INT()               IEC3SET = 0x2000
#define SPI1_CLEAR_ERR_INT_FLAG()           IFS3CLR = 0x2000


typedef struct _xb_t
{
    uint8_t *volatile ptr;
    volatile int size;
    volatile int index;
} xb_t;

static TX_SEMAPHORE sem_spi1_fault, sem_spi1_rx, sem_spi1_rx_api, sem_spi1_tx, sem_spi1_tx_api;

static xb_t spi1_rxbuf = {0,0,0}, spi1_txbuf = {0,0,0};


static int _spi1_read_fifo()
{
    while (!(SPI1STAT & _SPI1STAT_SPIRBE_MASK))
    {
        uint32_t x = SPI1BUF;
        
        if(spi1_rxbuf.index < spi1_rxbuf.size)
            spi1_rxbuf.ptr[spi1_rxbuf.index++] = (uint8_t)x;
        else
            return 0;
    }
    return 1;
}

static void _spi1_rx_handler()
{
    if(spi1_rxbuf.ptr)
    {
        if(!_spi1_read_fifo())
        {
            spi1_rxbuf.ptr = 0;
            SPI1_CLEAR_RX_INT_FLAG();
            _tx_semaphore_ceiling_put(&sem_spi1_rx,1);
        }
    }
    
    volatile uint32_t _tempu32;
    while (!(SPI1STAT & _SPI1STAT_SPIRBE_MASK))
        _tempu32 = SPI1BUF;
    SPI1_CLEAR_RX_INT_FLAG();   
}


static void _spi1_tx_handler()
{
    if(spi1_txbuf.ptr)
    {
        while ((!(SPI1STAT & _SPI1STAT_SPITBF_MASK)))
        {
            if(spi1_txbuf.index < spi1_txbuf.size)
                SPI1BUF = spi1_txbuf.ptr[spi1_txbuf.index++];
            else
            {
                spi1_txbuf.ptr = 0;
                SPI1_DISABLE_TX_INT();
                SPI1_CLEAR_TX_INT_FLAG();
                tx_semaphore_ceiling_put(&sem_spi1_tx,1);
                if(GPIO_PinRead((GPIO_PIN)SPI1_CS_PIN))
                    SPI0_INT_Set();
                return;
            }
        }
        
        if(GPIO_PinRead((GPIO_PIN)SPI1_CS_PIN))
            SPI0_INT_Set();
    }
    else
    {
        SPI1_DISABLE_TX_INT();
        SPI1_CLEAR_TX_INT_FLAG();
    }
}

static void __attribute__((used)) SPI1_CS_Handler(GPIO_PIN pin, uintptr_t context)
{
    if(!GPIO_PinRead((GPIO_PIN)SPI1_CS_PIN))
        SPI0_INT_Clear();
    
    else
    {
        
        
        SPI1_CLEAR_RX_INT_FLAG();
        if(spi1_rxbuf.ptr)
        {
            _spi1_read_fifo();
            spi1_rxbuf.ptr = 0;
            tx_semaphore_ceiling_put(&sem_spi1_rx,1);
        }
        else
        {
            volatile int x;
            while (!(SPI1STAT & _SPI1STAT_SPIRBE_MASK))
            { x = SPI1BUF; }
        }
        if(spi1_txbuf.ptr)
        {
            spi1_txbuf.ptr = 0;
            SPI1_DISABLE_TX_INT();
            SPI1_CLEAR_TX_INT_FLAG();
            tx_semaphore_ceiling_put(&sem_spi1_tx,1);
        }
    }
}

void __ISR(_SPI1_FAULT_VECTOR, ipl1SAVEALL) SPI1_FAULT_Handler (void)
{
 //    _tx_thread_context_save();
    SPI1STATCLR = _SPI1STAT_SPIROV_MASK;
    SPI1_CLEAR_ERR_INT_FLAG();
//    _tx_thread_context_restore();
 }

void __ISR(_SPI1_RX_VECTOR, ipl1SAVEALL) SPI1_RX_Handler (void)
{
    _tx_thread_context_save();
    _spi1_rx_handler();
    _tx_thread_context_restore();
}

void __ISR(_SPI1_TX_VECTOR, ipl1SAVEALL) SPI1_TX_Handler (void)
{
    _tx_thread_context_save();
    _spi1_tx_handler();
    _tx_thread_context_restore();
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
    
    if(tx_semaphore_create(&sem_spi1_fault,"",0) != TX_SUCCESS)
        __builtin_software_breakpoint();
    
    if(tx_semaphore_create(&sem_spi1_rx,"",0) != TX_SUCCESS)
        __builtin_software_breakpoint();
    
    if(tx_semaphore_create(&sem_spi1_rx_api,"",1) != TX_SUCCESS)
        __builtin_software_breakpoint();
    
    if(tx_semaphore_create(&sem_spi1_tx,"",0) != TX_SUCCESS)
        __builtin_software_breakpoint();
    
    if(tx_semaphore_create(&sem_spi1_tx_api,"",1) != TX_SUCCESS)
        __builtin_software_breakpoint();
    

    /* Register callback and enable notifications on Chip Select logic level change */
    (void)GPIO_PinInterruptCallbackRegister(SPI1_CS_PIN, SPI1_CS_Handler, 0U);
    GPIO_PinInterruptEnable(SPI1_CS_PIN);

    /* Enable SPI1 RX and Error Interrupts. TX interrupt will be enabled when a SPI write is submitted. */
    SPI1_ENABLE_RX_INT();
    SPI1_DISABLE_TX_INT();
    SPI1_ENABLE_ERR_INT();

    /* Enable SPI1 */
    SPI1CONSET = _SPI1CON_ON_MASK;
}

size_t SPI1_Read(void *dest, int size)
{
    size_t rv;
    tx_semaphore_get(&sem_spi1_rx_api, TX_WAIT_FOREVER);

    SPI1_DISABLE_RX_INT();
    spi1_rxbuf.size = size;
    spi1_rxbuf.index = 0;
    spi1_rxbuf.ptr = dest;
    SPI1_ENABLE_RX_INT();
    
    tx_semaphore_get(&sem_spi1_rx, TX_WAIT_FOREVER);
    
    rv = spi1_rxbuf.index;
    spi1_rxbuf.index = 0;
    
    tx_semaphore_ceiling_put(&sem_spi1_rx_api,1);
    return rv;
}

size_t SPI1_Write(const void *src, int size)
{
    size_t rv;
    tx_semaphore_get(&sem_spi1_tx_api, TX_WAIT_FOREVER);
    
    spi1_txbuf.ptr = (void*)src;
    spi1_txbuf.size = size;
    spi1_txbuf.index = 0;
    
    SPI1_ENABLE_TX_INT();
    
    tx_semaphore_get(&sem_spi1_tx, TX_WAIT_FOREVER);
    
    rv = spi1_txbuf.index;
    spi1_txbuf.index = 0;
    
    tx_semaphore_ceiling_put(&sem_spi1_tx_api,1);
    return rv;    
}

////////////////////////////////////////////////////////////////////////////////