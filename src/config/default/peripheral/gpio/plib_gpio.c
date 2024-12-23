/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.c

  Summary:
    GPIO function implementations for the GPIO PLIB.

  Description:
    The GPIO PLIB provides a simple interface to manage peripheral
    input-output controller.

*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#include "plib_gpio.h"
#include "interrupts.h"



/* Array to store callback objects of each configured interrupt */
static volatile GPIO_PIN_CALLBACK_OBJ portPinCbObj[16];

/* Array to store number of interrupts in each PORT Channel + previous interrupt count */
static uint8_t portNumCb[10 + 1] = { 0, 1, 1, 3, 6, 6, 6, 8, 11, 14, 16, };

/******************************************************************************
  Function:
    GPIO_Initialize ( void )

  Summary:
    Initialize the GPIO library.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_Initialize ( void )
{

    /* PORTA Initialization */
    LATA = 0x1U; /* Initial Latch Value */
    TRISACLR = 0x33U; /* Direction Control */
    ANSELACLR = 0x23U; /* Digital Mode Enable */
    CNPUASET = 0xc00cU; /* Pull-Up Enable */

    /* Change Notice Enable */
    CNCONASET = _CNCONA_ON_MASK;
    PORTA;
    IEC3SET = _IEC3_CNAIE_MASK;
    /* PORTB Initialization */
    LATB = 0x9130U; /* Initial Latch Value */
    TRISBCLR = 0x1938U; /* Direction Control */
    ANSELBCLR = 0xfff8U; /* Digital Mode Enable */
    /* PORTC Initialization */
    LATC = 0x0U; /* Initial Latch Value */
    TRISCCLR = 0x18U; /* Direction Control */
    ANSELCCLR = 0x1eU; /* Digital Mode Enable */

    /* Change Notice Enable */
    CNCONCSET = _CNCONC_ON_MASK;
    PORTC;
    IEC3SET = _IEC3_CNCIE_MASK;
    /* PORTD Initialization */
    LATD = 0x0U; /* Initial Latch Value */
    TRISDCLR = 0x601U; /* Direction Control */
    ANSELDCLR = 0xc000U; /* Digital Mode Enable */

    /* Change Notice Enable */
    CNCONDSET = _CNCOND_ON_MASK;
    PORTD;
    IEC3SET = _IEC3_CNDIE_MASK;
    /* PORTE Initialization */
    LATE = 0x4aU; /* Initial Latch Value */
    TRISECLR = 0x6fU; /* Direction Control */
    ANSELECLR = 0x3f0U; /* Digital Mode Enable */
    /* PORTF Initialization */
    LATF = 0x0U; /* Initial Latch Value */
    TRISFCLR = 0x1000U; /* Direction Control */
    ANSELFCLR = 0x3000U; /* Digital Mode Enable */
    /* PORTG Initialization */
    LATG = 0x200U; /* Initial Latch Value */
    TRISGCLR = 0x2200U; /* Direction Control */
    ANSELGCLR = 0x83c0U; /* Digital Mode Enable */

    /* Change Notice Enable */
    CNCONGSET = _CNCONG_ON_MASK;
    PORTG;
    IEC3SET = _IEC3_CNGIE_MASK;
    /* PORTH Initialization */
    LATH = 0x2181U; /* Initial Latch Value */
    TRISHCLR = 0x29e9U; /* Direction Control */
    ANSELHCLR = 0x73U; /* Digital Mode Enable */

    /* Change Notice Enable */
    CNCONHSET = _CNCONH_ON_MASK;
    PORTH;
    IEC3SET = _IEC3_CNHIE_MASK;
    /* PORTJ Initialization */
    LATJ = 0x1000U; /* Initial Latch Value */
    TRISJCLR = 0xd306U; /* Direction Control */
    ANSELJCLR = 0xb00U; /* Digital Mode Enable */

    /* Change Notice Enable */
    CNCONJSET = _CNCONJ_ON_MASK;
    PORTJ;
    IEC3SET = _IEC3_CNJIE_MASK;
    /* PORTK Initialization */
    LATK = 0x43U; /* Initial Latch Value */
    TRISKCLR = 0xc3U; /* Direction Control */

    /* Change Notice Enable */
    CNCONKSET = _CNCONK_ON_MASK;
    PORTK;
    IEC3SET = _IEC3_CNKIE_MASK;

    /* Unlock system for PPS configuration */
    SYSKEY = 0x00000000U;
    SYSKEY = 0xAA996655U;
    SYSKEY = 0x556699AAU;

    CFGCONbits.IOLOCK = 0U;

    /* PPS Input Remapping */
    SDI2R = 1;
    U2RXR = 13;
    U6RXR = 5;
    SDI5R = 5;
    SDI6R = 2;
    SDI1R = 7;
    U5RXR = 0;
    SS1R = 10;
    U4RXR = 6;
    U1RXR = 4;
    U3RXR = 12;

    /* PPS Output Remapping */
    RPG8R = 6;
    RPE8R = 2;
    RPB7R = 4;
    RPB10R = 9;
    RPB15R = 10;
    RPF3R = 13;
    RPF2R = 12;
    RPF8R = 11;
    RPF4R = 11;
    RPF5R = 11;
    RPC13R = 5;
    RPD3R = 3;
    RPD4R = 2;
    RPF0R = 1;
    RPG1R = 1;

        /* Lock back the system after PPS configuration */
    CFGCONbits.IOLOCK = 1U;

    SYSKEY = 0x00000000U;

    uint32_t i;
    /* Initialize Interrupt Pin data structures */
    portPinCbObj[6 + 0].pin = GPIO_PIN_RG15;
    
    portPinCbObj[1 + 0].pin = GPIO_PIN_RC1;
    
    portPinCbObj[1 + 1].pin = GPIO_PIN_RC2;
    
    portPinCbObj[11 + 0].pin = GPIO_PIN_RJ13;
    
    portPinCbObj[8 + 0].pin = GPIO_PIN_RH2;
    
    portPinCbObj[14 + 0].pin = GPIO_PIN_RK3;
    
    portPinCbObj[8 + 1].pin = GPIO_PIN_RH4;
    
    portPinCbObj[3 + 0].pin = GPIO_PIN_RD14;
    
    portPinCbObj[14 + 1].pin = GPIO_PIN_RK4;
    
    portPinCbObj[3 + 1].pin = GPIO_PIN_RD11;
    
    portPinCbObj[8 + 2].pin = GPIO_PIN_RH15;
    
    portPinCbObj[3 + 2].pin = GPIO_PIN_RD12;
    
    portPinCbObj[11 + 1].pin = GPIO_PIN_RJ3;
    
    portPinCbObj[0 + 0].pin = GPIO_PIN_RA6;
    
    portPinCbObj[11 + 2].pin = GPIO_PIN_RJ6;
    
    portPinCbObj[6 + 1].pin = GPIO_PIN_RG12;
    
    for(i=0U; i<16U; i++)
    {
        portPinCbObj[i].callback = NULL;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: GPIO APIs which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t GPIO_PortRead ( GPIO_PORT port )

  Summary:
    Read all the I/O lines of the selected port.

  Description:
    This function reads the live data values on all the I/O lines of the
    selected port.  Bit values returned in each position indicate corresponding
    pin levels.
    1 = Pin is high.
    0 = Pin is low.

    This function reads the value regardless of pin configuration, whether it is
    set as as an input, driven by the GPIO Controller, or driven by a peripheral.

  Remarks:
    If the port has less than 32-bits, unimplemented pins will read as
    low (0).
    Implemented pins are Right aligned in the 32-bit return value.
*/
uint32_t GPIO_PortRead(GPIO_PORT port)
{
    return (*(volatile uint32_t *)(&PORTA + (port * 0x40U)));
}

// *****************************************************************************
/* Function:
    void GPIO_PortWrite (GPIO_PORT port, uint32_t mask, uint32_t value);

  Summary:
    Write the value on the masked I/O lines of the selected port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value)
{
    *(volatile uint32_t *)(&LATA + (port * 0x40U)) = (*(volatile uint32_t *)(&LATA + (port * 0x40U)) & (~mask)) | (mask & value);
}

// *****************************************************************************
/* Function:
    uint32_t GPIO_PortLatchRead ( GPIO_PORT port )

  Summary:
    Read the latched value on all the I/O lines of the selected port.

  Remarks:
    See plib_gpio.h for more details.
*/
uint32_t GPIO_PortLatchRead(GPIO_PORT port)
{
    return (*(volatile uint32_t *)(&LATA + (port * 0x40U)));
}

// *****************************************************************************
/* Function:
    void GPIO_PortSet ( GPIO_PORT port, uint32_t mask )

  Summary:
    Set the selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortSet(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&LATASET + (port * 0x40U)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortClear ( GPIO_PORT port, uint32_t mask )

  Summary:
    Clear the selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortClear(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&LATACLR + (port * 0x40U)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortToggle ( GPIO_PORT port, uint32_t mask )

  Summary:
    Toggles the selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortToggle(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&LATAINV + (port * 0x40U))= mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortInputEnable ( GPIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as input.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&TRISASET + (port * 0x40U)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortOutputEnable ( GPIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as output(s).

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&TRISACLR + (port * 0x40U)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortInterruptEnable(GPIO_PORT port, uint32_t mask)

  Summary:
    Enables IO interrupt on selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortInterruptEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNENASET + (port * 0x40U)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortInterruptDisable(GPIO_PORT port, uint32_t mask)

  Summary:
    Disables IO interrupt on selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortInterruptDisable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNENACLR + (port * 0x40U)) = mask;
}

// *****************************************************************************
// *****************************************************************************
// Section: GPIO APIs which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void GPIO_PinIntEnable(GPIO_PIN pin, GPIO_INTERRUPT_STYLE style)

  Summary:
    Enables IO interrupt of particular style on selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PinIntEnable(GPIO_PIN pin, GPIO_INTERRUPT_STYLE style)
{
    GPIO_PORT port;
    uint32_t mask;

    port = (GPIO_PORT)(pin>>4U);
    mask =  0x1UL << (pin & 0xFU);

    if (style == GPIO_INTERRUPT_ON_MISMATCH)
    {
        *(volatile uint32_t *)(&CNENASET + (port * 0x40U)) = mask;
    }
    else if (style == GPIO_INTERRUPT_ON_RISING_EDGE)
    {
        *(volatile uint32_t *)(&CNENASET + (port * 0x40U)) = mask;
        *(volatile uint32_t *)(&CNNEACLR + (port * 0x40U)) = mask;
    }
    else if (style == GPIO_INTERRUPT_ON_FALLING_EDGE)
    {
        *(volatile uint32_t *)(&CNENACLR + (port * 0x40U)) = mask;
        *(volatile uint32_t *)(&CNNEASET + (port * 0x40U)) = mask;
    }
    else if (style == GPIO_INTERRUPT_ON_BOTH_EDGES)
    {
        *(volatile uint32_t *)(&CNENASET + (port * 0x40U)) = mask;
        *(volatile uint32_t *)(&CNNEASET + (port * 0x40U)) = mask;
    }
    else
    {
        /* Nothing to process */
    }
}

// *****************************************************************************
/* Function:
    void GPIO_PinIntDisable(GPIO_PIN pin)

  Summary:
    Disables IO interrupt on selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PinIntDisable(GPIO_PIN pin)
{
    GPIO_PORT port;
    uint32_t mask;

    port = (GPIO_PORT)(pin>>4U);
    mask =  0x1UL << (pin & 0xFU);

    *(volatile uint32_t *)(&CNENACLR + (port * 0x40U)) = mask;
    *(volatile uint32_t *)(&CNNEACLR + (port * 0x40U)) = mask;
}
// *****************************************************************************
/* Function:
    bool GPIO_PinInterruptCallbackRegister(
        GPIO_PIN pin,
        const GPIO_PIN_CALLBACK callback,
        uintptr_t context
    );

  Summary:
    Allows application to register callback for configured pin.

  Remarks:
    See plib_gpio.h for more details.
*/
bool GPIO_PinInterruptCallbackRegister(
    GPIO_PIN pin,
    const GPIO_PIN_CALLBACK callback,
    uintptr_t context
)
{
    uint8_t i;
    uint8_t portIndex;

    portIndex = (uint8_t)(pin >> 4U);

    for(i = portNumCb[portIndex]; i < portNumCb[portIndex +1]; i++)
    {
        if (portPinCbObj[i].pin == pin)
        {
            portPinCbObj[i].callback = callback;
            portPinCbObj[i].context  = context;
            return true;
        }
    }
    return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: Local Function Implementation
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    void CHANGE_NOTICE_D_InterruptHandler(void)

  Summary:
    Interrupt Handler for change notice interrupt for channel D.

  Remarks:
    It is an internal function called from ISR, user should not call it directly.
*/
    


/*******************************************************************************
 End of File
*/
