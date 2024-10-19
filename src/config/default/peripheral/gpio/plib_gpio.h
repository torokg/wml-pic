/*******************************************************************************
  GPIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.h UUUUUUUUU

  Summary:
    GPIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (GPIO) module.

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

#ifndef PLIB_GPIO_H
#define PLIB_GPIO_H

#include <device.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************


/*** Macros for STY_INDEX pin ***/
#define STY_INDEX_Set()               (LATGSET = (1U<<15))
#define STY_INDEX_Clear()             (LATGCLR = (1U<<15))
#define STY_INDEX_Toggle()            (LATGINV= (1U<<15))
#define STY_INDEX_OutputEnable()      (TRISGCLR = (1U<<15))
#define STY_INDEX_InputEnable()       (TRISGSET = (1U<<15))
#define STY_INDEX_Get()               ((PORTG >> 15) & 0x1U)
#define STY_INDEX_PIN                  GPIO_PIN_RG15
#define STY_INDEX_InterruptEnable()   (CNENGSET = (1U<<15))
#define STY_INDEX_InterruptDisable()  (CNENGCLR = (1U<<15))

/*** Macros for STY_STEP pin ***/
#define STY_STEP_Set()               (LATASET = (1U<<5))
#define STY_STEP_Clear()             (LATACLR = (1U<<5))
#define STY_STEP_Toggle()            (LATAINV= (1U<<5))
#define STY_STEP_OutputEnable()      (TRISACLR = (1U<<5))
#define STY_STEP_InputEnable()       (TRISASET = (1U<<5))
#define STY_STEP_Get()               ((PORTA >> 5) & 0x1U)
#define STY_STEP_PIN                  GPIO_PIN_RA5

/*** Macros for STY_DIR pin ***/
#define STY_DIR_Set()               (LATESET = (1U<<5))
#define STY_DIR_Clear()             (LATECLR = (1U<<5))
#define STY_DIR_Toggle()            (LATEINV= (1U<<5))
#define STY_DIR_OutputEnable()      (TRISECLR = (1U<<5))
#define STY_DIR_InputEnable()       (TRISESET = (1U<<5))
#define STY_DIR_Get()               ((PORTE >> 5) & 0x1U)
#define STY_DIR_PIN                  GPIO_PIN_RE5

/*** Macros for STZ_ENABLE pin ***/
#define STZ_ENABLE_Set()               (LATESET = (1U<<6))
#define STZ_ENABLE_Clear()             (LATECLR = (1U<<6))
#define STZ_ENABLE_Toggle()            (LATEINV= (1U<<6))
#define STZ_ENABLE_OutputEnable()      (TRISECLR = (1U<<6))
#define STZ_ENABLE_InputEnable()       (TRISESET = (1U<<6))
#define STZ_ENABLE_Get()               ((PORTE >> 6) & 0x1U)
#define STZ_ENABLE_PIN                  GPIO_PIN_RE6

/*** Macros for STZ_DIAG pin ***/
#define STZ_DIAG_Set()               (LATESET = (1U<<7))
#define STZ_DIAG_Clear()             (LATECLR = (1U<<7))
#define STZ_DIAG_Toggle()            (LATEINV= (1U<<7))
#define STZ_DIAG_OutputEnable()      (TRISECLR = (1U<<7))
#define STZ_DIAG_InputEnable()       (TRISESET = (1U<<7))
#define STZ_DIAG_Get()               ((PORTE >> 7) & 0x1U)
#define STZ_DIAG_PIN                  GPIO_PIN_RE7

/*** Macros for STZ_INDEX pin ***/
#define STZ_INDEX_Set()               (LATCSET = (1U<<1))
#define STZ_INDEX_Clear()             (LATCCLR = (1U<<1))
#define STZ_INDEX_Toggle()            (LATCINV= (1U<<1))
#define STZ_INDEX_OutputEnable()      (TRISCCLR = (1U<<1))
#define STZ_INDEX_InputEnable()       (TRISCSET = (1U<<1))
#define STZ_INDEX_Get()               ((PORTC >> 1) & 0x1U)
#define STZ_INDEX_PIN                  GPIO_PIN_RC1
#define STZ_INDEX_InterruptEnable()   (CNENCSET = (1U<<1))
#define STZ_INDEX_InterruptDisable()  (CNENCCLR = (1U<<1))

/*** Macros for STZ_DIR pin ***/
#define STZ_DIR_Set()               (LATJSET = (1U<<8))
#define STZ_DIR_Clear()             (LATJCLR = (1U<<8))
#define STZ_DIR_Toggle()            (LATJINV= (1U<<8))
#define STZ_DIR_OutputEnable()      (TRISJCLR = (1U<<8))
#define STZ_DIR_InputEnable()       (TRISJSET = (1U<<8))
#define STZ_DIR_Get()               ((PORTJ >> 8) & 0x1U)
#define STZ_DIR_PIN                  GPIO_PIN_RJ8

/*** Macros for STZ_STEP pin ***/
#define STZ_STEP_Set()               (LATJSET = (1U<<9))
#define STZ_STEP_Clear()             (LATJCLR = (1U<<9))
#define STZ_STEP_Toggle()            (LATJINV= (1U<<9))
#define STZ_STEP_OutputEnable()      (TRISJCLR = (1U<<9))
#define STZ_STEP_InputEnable()       (TRISJSET = (1U<<9))
#define STZ_STEP_Get()               ((PORTJ >> 9) & 0x1U)
#define STZ_STEP_PIN                  GPIO_PIN_RJ9

/*** Macros for STC_ENABLE pin ***/
#define STC_ENABLE_Set()               (LATJSET = (1U<<12))
#define STC_ENABLE_Clear()             (LATJCLR = (1U<<12))
#define STC_ENABLE_Toggle()            (LATJINV= (1U<<12))
#define STC_ENABLE_OutputEnable()      (TRISJCLR = (1U<<12))
#define STC_ENABLE_InputEnable()       (TRISJSET = (1U<<12))
#define STC_ENABLE_Get()               ((PORTJ >> 12) & 0x1U)
#define STC_ENABLE_PIN                  GPIO_PIN_RJ12

/*** Macros for STC_DIAG pin ***/
#define STC_DIAG_Set()               (LATJSET = (1U<<10))
#define STC_DIAG_Clear()             (LATJCLR = (1U<<10))
#define STC_DIAG_Toggle()            (LATJINV= (1U<<10))
#define STC_DIAG_OutputEnable()      (TRISJCLR = (1U<<10))
#define STC_DIAG_InputEnable()       (TRISJSET = (1U<<10))
#define STC_DIAG_Get()               ((PORTJ >> 10) & 0x1U)
#define STC_DIAG_PIN                  GPIO_PIN_RJ10

/*** Macros for STC_INDEX pin ***/
#define STC_INDEX_Set()               (LATCSET = (1U<<2))
#define STC_INDEX_Clear()             (LATCCLR = (1U<<2))
#define STC_INDEX_Toggle()            (LATCINV= (1U<<2))
#define STC_INDEX_OutputEnable()      (TRISCCLR = (1U<<2))
#define STC_INDEX_InputEnable()       (TRISCSET = (1U<<2))
#define STC_INDEX_Get()               ((PORTC >> 2) & 0x1U)
#define STC_INDEX_PIN                  GPIO_PIN_RC2
#define STC_INDEX_InterruptEnable()   (CNENCSET = (1U<<2))
#define STC_INDEX_InterruptDisable()  (CNENCCLR = (1U<<2))

/*** Macros for STC_DIR pin ***/
#define STC_DIR_Set()               (LATCSET = (1U<<3))
#define STC_DIR_Clear()             (LATCCLR = (1U<<3))
#define STC_DIR_Toggle()            (LATCINV= (1U<<3))
#define STC_DIR_OutputEnable()      (TRISCCLR = (1U<<3))
#define STC_DIR_InputEnable()       (TRISCSET = (1U<<3))
#define STC_DIR_Get()               ((PORTC >> 3) & 0x1U)
#define STC_DIR_PIN                  GPIO_PIN_RC3

/*** Macros for STC_STEP pin ***/
#define STC_STEP_Set()               (LATCSET = (1U<<4))
#define STC_STEP_Clear()             (LATCCLR = (1U<<4))
#define STC_STEP_Toggle()            (LATCINV= (1U<<4))
#define STC_STEP_OutputEnable()      (TRISCCLR = (1U<<4))
#define STC_STEP_InputEnable()       (TRISCSET = (1U<<4))
#define STC_STEP_Get()               ((PORTC >> 4) & 0x1U)
#define STC_STEP_PIN                  GPIO_PIN_RC4

/*** Macros for SPI2_CLK pin ***/
#define SPI2_CLK_Get()               ((PORTG >> 6) & 0x1U)
#define SPI2_CLK_PIN                  GPIO_PIN_RG6

/*** Macros for SPI2_MISO pin ***/
#define SPI2_MISO_Get()               ((PORTG >> 7) & 0x1U)
#define SPI2_MISO_PIN                  GPIO_PIN_RG7

/*** Macros for SPI2_MOSI pin ***/
#define SPI2_MOSI_Get()               ((PORTG >> 8) & 0x1U)
#define SPI2_MOSI_PIN                  GPIO_PIN_RG8

/*** Macros for SPI2_CS0 pin ***/
#define SPI2_CS0_Set()               (LATKSET = (1U<<0))
#define SPI2_CS0_Clear()             (LATKCLR = (1U<<0))
#define SPI2_CS0_Toggle()            (LATKINV= (1U<<0))
#define SPI2_CS0_OutputEnable()      (TRISKCLR = (1U<<0))
#define SPI2_CS0_InputEnable()       (TRISKSET = (1U<<0))
#define SPI2_CS0_Get()               ((PORTK >> 0) & 0x1U)
#define SPI2_CS0_PIN                  GPIO_PIN_RK0

/*** Macros for SPI2_CS1 pin ***/
#define SPI2_CS1_Set()               (LATGSET = (1U<<9))
#define SPI2_CS1_Clear()             (LATGCLR = (1U<<9))
#define SPI2_CS1_Toggle()            (LATGINV= (1U<<9))
#define SPI2_CS1_OutputEnable()      (TRISGCLR = (1U<<9))
#define SPI2_CS1_InputEnable()       (TRISGSET = (1U<<9))
#define SPI2_CS1_Get()               ((PORTG >> 9) & 0x1U)
#define SPI2_CS1_PIN                  GPIO_PIN_RG9

/*** Macros for SPI2_CS2 pin ***/
#define SPI2_CS2_Set()               (LATASET = (1U<<0))
#define SPI2_CS2_Clear()             (LATACLR = (1U<<0))
#define SPI2_CS2_Toggle()            (LATAINV= (1U<<0))
#define SPI2_CS2_OutputEnable()      (TRISACLR = (1U<<0))
#define SPI2_CS2_InputEnable()       (TRISASET = (1U<<0))
#define SPI2_CS2_Get()               ((PORTA >> 0) & 0x1U)
#define SPI2_CS2_PIN                  GPIO_PIN_RA0

/*** Macros for UART2_TX pin ***/
#define UART2_TX_Get()               ((PORTE >> 8) & 0x1U)
#define UART2_TX_PIN                  GPIO_PIN_RE8

/*** Macros for UART2_RX pin ***/
#define UART2_RX_Get()               ((PORTE >> 9) & 0x1U)
#define UART2_RX_PIN                  GPIO_PIN_RE9

/*** Macros for SPI2_CS3 pin ***/
#define SPI2_CS3_Set()               (LATBSET = (1U<<5))
#define SPI2_CS3_Clear()             (LATBCLR = (1U<<5))
#define SPI2_CS3_Toggle()            (LATBINV= (1U<<5))
#define SPI2_CS3_OutputEnable()      (TRISBCLR = (1U<<5))
#define SPI2_CS3_InputEnable()       (TRISBSET = (1U<<5))
#define SPI2_CS3_Get()               ((PORTB >> 5) & 0x1U)
#define SPI2_CS3_PIN                  GPIO_PIN_RB5

/*** Macros for ST8_ENABLE pin ***/
#define ST8_ENABLE_Set()               (LATBSET = (1U<<4))
#define ST8_ENABLE_Clear()             (LATBCLR = (1U<<4))
#define ST8_ENABLE_Toggle()            (LATBINV= (1U<<4))
#define ST8_ENABLE_OutputEnable()      (TRISBCLR = (1U<<4))
#define ST8_ENABLE_InputEnable()       (TRISBSET = (1U<<4))
#define ST8_ENABLE_Get()               ((PORTB >> 4) & 0x1U)
#define ST8_ENABLE_PIN                  GPIO_PIN_RB4

/*** Macros for ST8_DIAG pin ***/
#define ST8_DIAG_Set()               (LATJSET = (1U<<11))
#define ST8_DIAG_Clear()             (LATJCLR = (1U<<11))
#define ST8_DIAG_Toggle()            (LATJINV= (1U<<11))
#define ST8_DIAG_OutputEnable()      (TRISJCLR = (1U<<11))
#define ST8_DIAG_InputEnable()       (TRISJSET = (1U<<11))
#define ST8_DIAG_Get()               ((PORTJ >> 11) & 0x1U)
#define ST8_DIAG_PIN                  GPIO_PIN_RJ11

/*** Macros for ST8_INDEX pin ***/
#define ST8_INDEX_Set()               (LATJSET = (1U<<13))
#define ST8_INDEX_Clear()             (LATJCLR = (1U<<13))
#define ST8_INDEX_Toggle()            (LATJINV= (1U<<13))
#define ST8_INDEX_OutputEnable()      (TRISJCLR = (1U<<13))
#define ST8_INDEX_InputEnable()       (TRISJSET = (1U<<13))
#define ST8_INDEX_Get()               ((PORTJ >> 13) & 0x1U)
#define ST8_INDEX_PIN                  GPIO_PIN_RJ13
#define ST8_INDEX_InterruptEnable()   (CNENJSET = (1U<<13))
#define ST8_INDEX_InterruptDisable()  (CNENJCLR = (1U<<13))

/*** Macros for ST8_DIR pin ***/
#define ST8_DIR_Set()               (LATJSET = (1U<<14))
#define ST8_DIR_Clear()             (LATJCLR = (1U<<14))
#define ST8_DIR_Toggle()            (LATJINV= (1U<<14))
#define ST8_DIR_OutputEnable()      (TRISJCLR = (1U<<14))
#define ST8_DIR_InputEnable()       (TRISJSET = (1U<<14))
#define ST8_DIR_Get()               ((PORTJ >> 14) & 0x1U)
#define ST8_DIR_PIN                  GPIO_PIN_RJ14

/*** Macros for ST8_STEP pin ***/
#define ST8_STEP_Set()               (LATJSET = (1U<<15))
#define ST8_STEP_Clear()             (LATJCLR = (1U<<15))
#define ST8_STEP_Toggle()            (LATJINV= (1U<<15))
#define ST8_STEP_OutputEnable()      (TRISJCLR = (1U<<15))
#define ST8_STEP_InputEnable()       (TRISJSET = (1U<<15))
#define ST8_STEP_Get()               ((PORTJ >> 15) & 0x1U)
#define ST8_STEP_PIN                  GPIO_PIN_RJ15

/*** Macros for SPI5_RESET pin ***/
#define SPI5_RESET_Set()               (LATBSET = (1U<<3))
#define SPI5_RESET_Clear()             (LATBCLR = (1U<<3))
#define SPI5_RESET_Toggle()            (LATBINV= (1U<<3))
#define SPI5_RESET_OutputEnable()      (TRISBCLR = (1U<<3))
#define SPI5_RESET_InputEnable()       (TRISBSET = (1U<<3))
#define SPI5_RESET_Get()               ((PORTB >> 3) & 0x1U)
#define SPI5_RESET_PIN                  GPIO_PIN_RB3

/*** Macros for UART6_RX pin ***/
#define UART6_RX_Get()               ((PORTB >> 6) & 0x1U)
#define UART6_RX_PIN                  GPIO_PIN_RB6

/*** Macros for UART6_TX pin ***/
#define UART6_TX_Get()               ((PORTB >> 7) & 0x1U)
#define UART6_TX_PIN                  GPIO_PIN_RB7

/*** Macros for ST6_ENABLE pin ***/
#define ST6_ENABLE_Set()               (LATHSET = (1U<<0))
#define ST6_ENABLE_Clear()             (LATHCLR = (1U<<0))
#define ST6_ENABLE_Toggle()            (LATHINV= (1U<<0))
#define ST6_ENABLE_OutputEnable()      (TRISHCLR = (1U<<0))
#define ST6_ENABLE_InputEnable()       (TRISHSET = (1U<<0))
#define ST6_ENABLE_Get()               ((PORTH >> 0) & 0x1U)
#define ST6_ENABLE_PIN                  GPIO_PIN_RH0

/*** Macros for ST6_DIAG pin ***/
#define ST6_DIAG_Set()               (LATHSET = (1U<<1))
#define ST6_DIAG_Clear()             (LATHCLR = (1U<<1))
#define ST6_DIAG_Toggle()            (LATHINV= (1U<<1))
#define ST6_DIAG_OutputEnable()      (TRISHCLR = (1U<<1))
#define ST6_DIAG_InputEnable()       (TRISHSET = (1U<<1))
#define ST6_DIAG_Get()               ((PORTH >> 1) & 0x1U)
#define ST6_DIAG_PIN                  GPIO_PIN_RH1

/*** Macros for ST6_INDEX pin ***/
#define ST6_INDEX_Set()               (LATHSET = (1U<<2))
#define ST6_INDEX_Clear()             (LATHCLR = (1U<<2))
#define ST6_INDEX_Toggle()            (LATHINV= (1U<<2))
#define ST6_INDEX_OutputEnable()      (TRISHCLR = (1U<<2))
#define ST6_INDEX_InputEnable()       (TRISHSET = (1U<<2))
#define ST6_INDEX_Get()               ((PORTH >> 2) & 0x1U)
#define ST6_INDEX_PIN                  GPIO_PIN_RH2
#define ST6_INDEX_InterruptEnable()   (CNENHSET = (1U<<2))
#define ST6_INDEX_InterruptDisable()  (CNENHCLR = (1U<<2))

/*** Macros for ST6_STEP pin ***/
#define ST6_STEP_Set()               (LATHSET = (1U<<3))
#define ST6_STEP_Clear()             (LATHCLR = (1U<<3))
#define ST6_STEP_Toggle()            (LATHINV= (1U<<3))
#define ST6_STEP_OutputEnable()      (TRISHCLR = (1U<<3))
#define ST6_STEP_InputEnable()       (TRISHSET = (1U<<3))
#define ST6_STEP_Get()               ((PORTH >> 3) & 0x1U)
#define ST6_STEP_PIN                  GPIO_PIN_RH3

/*** Macros for SPI5_CS pin ***/
#define SPI5_CS_Set()               (LATBSET = (1U<<8))
#define SPI5_CS_Clear()             (LATBCLR = (1U<<8))
#define SPI5_CS_Toggle()            (LATBINV= (1U<<8))
#define SPI5_CS_OutputEnable()      (TRISBCLR = (1U<<8))
#define SPI5_CS_InputEnable()       (TRISBSET = (1U<<8))
#define SPI5_CS_Get()               ((PORTB >> 8) & 0x1U)
#define SPI5_CS_PIN                  GPIO_PIN_RB8

/*** Macros for SPI5_MISO pin ***/
#define SPI5_MISO_Get()               ((PORTB >> 9) & 0x1U)
#define SPI5_MISO_PIN                  GPIO_PIN_RB9

/*** Macros for SPI5_MOSI pin ***/
#define SPI5_MOSI_Get()               ((PORTB >> 10) & 0x1U)
#define SPI5_MOSI_PIN                  GPIO_PIN_RB10

/*** Macros for ST6_DIR pin ***/
#define ST6_DIR_Set()               (LATBSET = (1U<<11))
#define ST6_DIR_Clear()             (LATBCLR = (1U<<11))
#define ST6_DIR_Toggle()            (LATBINV= (1U<<11))
#define ST6_DIR_OutputEnable()      (TRISBCLR = (1U<<11))
#define ST6_DIR_InputEnable()       (TRISBSET = (1U<<11))
#define ST6_DIR_Get()               ((PORTB >> 11) & 0x1U)
#define ST6_DIR_PIN                  GPIO_PIN_RB11

/*** Macros for ST7_ENABLE pin ***/
#define ST7_ENABLE_Set()               (LATKSET = (1U<<1))
#define ST7_ENABLE_Clear()             (LATKCLR = (1U<<1))
#define ST7_ENABLE_Toggle()            (LATKINV= (1U<<1))
#define ST7_ENABLE_OutputEnable()      (TRISKCLR = (1U<<1))
#define ST7_ENABLE_InputEnable()       (TRISKSET = (1U<<1))
#define ST7_ENABLE_Get()               ((PORTK >> 1) & 0x1U)
#define ST7_ENABLE_PIN                  GPIO_PIN_RK1

/*** Macros for ST7_DIAG pin ***/
#define ST7_DIAG_Set()               (LATKSET = (1U<<2))
#define ST7_DIAG_Clear()             (LATKCLR = (1U<<2))
#define ST7_DIAG_Toggle()            (LATKINV= (1U<<2))
#define ST7_DIAG_OutputEnable()      (TRISKCLR = (1U<<2))
#define ST7_DIAG_InputEnable()       (TRISKSET = (1U<<2))
#define ST7_DIAG_Get()               ((PORTK >> 2) & 0x1U)
#define ST7_DIAG_PIN                  GPIO_PIN_RK2

/*** Macros for ST7_INDEX pin ***/
#define ST7_INDEX_Set()               (LATKSET = (1U<<3))
#define ST7_INDEX_Clear()             (LATKCLR = (1U<<3))
#define ST7_INDEX_Toggle()            (LATKINV= (1U<<3))
#define ST7_INDEX_OutputEnable()      (TRISKCLR = (1U<<3))
#define ST7_INDEX_InputEnable()       (TRISKSET = (1U<<3))
#define ST7_INDEX_Get()               ((PORTK >> 3) & 0x1U)
#define ST7_INDEX_PIN                  GPIO_PIN_RK3
#define ST7_INDEX_InterruptEnable()   (CNENKSET = (1U<<3))
#define ST7_INDEX_InterruptDisable()  (CNENKCLR = (1U<<3))

/*** Macros for ST7_STEP pin ***/
#define ST7_STEP_Set()               (LATASET = (1U<<1))
#define ST7_STEP_Clear()             (LATACLR = (1U<<1))
#define ST7_STEP_Toggle()            (LATAINV= (1U<<1))
#define ST7_STEP_OutputEnable()      (TRISACLR = (1U<<1))
#define ST7_STEP_InputEnable()       (TRISASET = (1U<<1))
#define ST7_STEP_Get()               ((PORTA >> 1) & 0x1U)
#define ST7_STEP_PIN                  GPIO_PIN_RA1

/*** Macros for SPI5_CLK pin ***/
#define SPI5_CLK_Get()               ((PORTF >> 13) & 0x1U)
#define SPI5_CLK_PIN                  GPIO_PIN_RF13

/*** Macros for ST7_DIR pin ***/
#define ST7_DIR_Set()               (LATFSET = (1U<<12))
#define ST7_DIR_Clear()             (LATFCLR = (1U<<12))
#define ST7_DIR_Toggle()            (LATFINV= (1U<<12))
#define ST7_DIR_OutputEnable()      (TRISFCLR = (1U<<12))
#define ST7_DIR_InputEnable()       (TRISFSET = (1U<<12))
#define ST7_DIR_Get()               ((PORTF >> 12) & 0x1U)
#define ST7_DIR_PIN                  GPIO_PIN_RF12

/*** Macros for ST9_ENABLE pin ***/
#define ST9_ENABLE_Set()               (LATBSET = (1U<<12))
#define ST9_ENABLE_Clear()             (LATBCLR = (1U<<12))
#define ST9_ENABLE_Toggle()            (LATBINV= (1U<<12))
#define ST9_ENABLE_OutputEnable()      (TRISBCLR = (1U<<12))
#define ST9_ENABLE_InputEnable()       (TRISBSET = (1U<<12))
#define ST9_ENABLE_Get()               ((PORTB >> 12) & 0x1U)
#define ST9_ENABLE_PIN                  GPIO_PIN_RB12

/*** Macros for ST9_DIAG pin ***/
#define ST9_DIAG_Set()               (LATBSET = (1U<<13))
#define ST9_DIAG_Clear()             (LATBCLR = (1U<<13))
#define ST9_DIAG_Toggle()            (LATBINV= (1U<<13))
#define ST9_DIAG_OutputEnable()      (TRISBCLR = (1U<<13))
#define ST9_DIAG_InputEnable()       (TRISBSET = (1U<<13))
#define ST9_DIAG_Get()               ((PORTB >> 13) & 0x1U)
#define ST9_DIAG_PIN                  GPIO_PIN_RB13

/*** Macros for SPI6_MISO pin ***/
#define SPI6_MISO_Get()               ((PORTB >> 14) & 0x1U)
#define SPI6_MISO_PIN                  GPIO_PIN_RB14

/*** Macros for SPI6_MOSI pin ***/
#define SPI6_MOSI_Get()               ((PORTB >> 15) & 0x1U)
#define SPI6_MOSI_PIN                  GPIO_PIN_RB15

/*** Macros for ST9_INDEX pin ***/
#define ST9_INDEX_Set()               (LATHSET = (1U<<4))
#define ST9_INDEX_Clear()             (LATHCLR = (1U<<4))
#define ST9_INDEX_Toggle()            (LATHINV= (1U<<4))
#define ST9_INDEX_OutputEnable()      (TRISHCLR = (1U<<4))
#define ST9_INDEX_InputEnable()       (TRISHSET = (1U<<4))
#define ST9_INDEX_Get()               ((PORTH >> 4) & 0x1U)
#define ST9_INDEX_PIN                  GPIO_PIN_RH4
#define ST9_INDEX_InterruptEnable()   (CNENHSET = (1U<<4))
#define ST9_INDEX_InterruptDisable()  (CNENHCLR = (1U<<4))

/*** Macros for ST9_STEP pin ***/
#define ST9_STEP_Set()               (LATHSET = (1U<<5))
#define ST9_STEP_Clear()             (LATHCLR = (1U<<5))
#define ST9_STEP_Toggle()            (LATHINV= (1U<<5))
#define ST9_STEP_OutputEnable()      (TRISHCLR = (1U<<5))
#define ST9_STEP_InputEnable()       (TRISHSET = (1U<<5))
#define ST9_STEP_Get()               ((PORTH >> 5) & 0x1U)
#define ST9_STEP_PIN                  GPIO_PIN_RH5

/*** Macros for ST9_DIR pin ***/
#define ST9_DIR_Set()               (LATHSET = (1U<<6))
#define ST9_DIR_Clear()             (LATHCLR = (1U<<6))
#define ST9_DIR_Toggle()            (LATHINV= (1U<<6))
#define ST9_DIR_OutputEnable()      (TRISHCLR = (1U<<6))
#define ST9_DIR_InputEnable()       (TRISHSET = (1U<<6))
#define ST9_DIR_Get()               ((PORTH >> 6) & 0x1U)
#define ST9_DIR_PIN                  GPIO_PIN_RH6

/*** Macros for SPI6_CS0 pin ***/
#define SPI6_CS0_Set()               (LATHSET = (1U<<7))
#define SPI6_CS0_Clear()             (LATHCLR = (1U<<7))
#define SPI6_CS0_Toggle()            (LATHINV= (1U<<7))
#define SPI6_CS0_OutputEnable()      (TRISHCLR = (1U<<7))
#define SPI6_CS0_InputEnable()       (TRISHSET = (1U<<7))
#define SPI6_CS0_Get()               ((PORTH >> 7) & 0x1U)
#define SPI6_CS0_PIN                  GPIO_PIN_RH7

/*** Macros for SPI6_INT0 pin ***/
#define SPI6_INT0_Set()               (LATDSET = (1U<<14))
#define SPI6_INT0_Clear()             (LATDCLR = (1U<<14))
#define SPI6_INT0_Toggle()            (LATDINV= (1U<<14))
#define SPI6_INT0_OutputEnable()      (TRISDCLR = (1U<<14))
#define SPI6_INT0_InputEnable()       (TRISDSET = (1U<<14))
#define SPI6_INT0_Get()               ((PORTD >> 14) & 0x1U)
#define SPI6_INT0_PIN                  GPIO_PIN_RD14
#define SPI6_INT0_InterruptEnable()   (CNENDSET = (1U<<14))
#define SPI6_INT0_InterruptDisable()  (CNENDCLR = (1U<<14))

/*** Macros for SPI6_CLK pin ***/
#define SPI6_CLK_Get()               ((PORTD >> 15) & 0x1U)
#define SPI6_CLK_PIN                  GPIO_PIN_RD15

/*** Macros for PWM0 pin ***/
#define PWM0_Get()               ((PORTF >> 3) & 0x1U)
#define PWM0_PIN                  GPIO_PIN_RF3

/*** Macros for PWM1 pin ***/
#define PWM1_Get()               ((PORTF >> 2) & 0x1U)
#define PWM1_PIN                  GPIO_PIN_RF2

/*** Macros for PWM2 pin ***/
#define PWM2_Get()               ((PORTF >> 8) & 0x1U)
#define PWM2_PIN                  GPIO_PIN_RF8

/*** Macros for SPI6_CS1 pin ***/
#define SPI6_CS1_Set()               (LATHSET = (1U<<8))
#define SPI6_CS1_Clear()             (LATHCLR = (1U<<8))
#define SPI6_CS1_Toggle()            (LATHINV= (1U<<8))
#define SPI6_CS1_OutputEnable()      (TRISHCLR = (1U<<8))
#define SPI6_CS1_InputEnable()       (TRISHSET = (1U<<8))
#define SPI6_CS1_Get()               ((PORTH >> 8) & 0x1U)
#define SPI6_CS1_PIN                  GPIO_PIN_RH8

/*** Macros for ST5_DIR pin ***/
#define ST5_DIR_Set()               (LATHSET = (1U<<11))
#define ST5_DIR_Clear()             (LATHCLR = (1U<<11))
#define ST5_DIR_Toggle()            (LATHINV= (1U<<11))
#define ST5_DIR_OutputEnable()      (TRISHCLR = (1U<<11))
#define ST5_DIR_InputEnable()       (TRISHSET = (1U<<11))
#define ST5_DIR_Get()               ((PORTH >> 11) & 0x1U)
#define ST5_DIR_PIN                  GPIO_PIN_RH11

/*** Macros for I2C1_SCL pin ***/
#define I2C1_SCL_Get()               ((PORTA >> 2) & 0x1U)
#define I2C1_SCL_PIN                  GPIO_PIN_RA2

/*** Macros for I2C1_SDA pin ***/
#define I2C1_SDA_Get()               ((PORTA >> 3) & 0x1U)
#define I2C1_SDA_PIN                  GPIO_PIN_RA3

/*** Macros for ST5_STEP pin ***/
#define ST5_STEP_Set()               (LATASET = (1U<<4))
#define ST5_STEP_Clear()             (LATACLR = (1U<<4))
#define ST5_STEP_Toggle()            (LATAINV= (1U<<4))
#define ST5_STEP_OutputEnable()      (TRISACLR = (1U<<4))
#define ST5_STEP_InputEnable()       (TRISASET = (1U<<4))
#define ST5_STEP_Get()               ((PORTA >> 4) & 0x1U)
#define ST5_STEP_PIN                  GPIO_PIN_RA4

/*** Macros for HEATER0_PWM pin ***/
#define HEATER0_PWM_Get()               ((PORTF >> 4) & 0x1U)
#define HEATER0_PWM_PIN                  GPIO_PIN_RF4

/*** Macros for HEATER1_PWM pin ***/
#define HEATER1_PWM_Get()               ((PORTF >> 5) & 0x1U)
#define HEATER1_PWM_PIN                  GPIO_PIN_RF5

/*** Macros for ST5_INDEX pin ***/
#define ST5_INDEX_Set()               (LATKSET = (1U<<4))
#define ST5_INDEX_Clear()             (LATKCLR = (1U<<4))
#define ST5_INDEX_Toggle()            (LATKINV= (1U<<4))
#define ST5_INDEX_OutputEnable()      (TRISKCLR = (1U<<4))
#define ST5_INDEX_InputEnable()       (TRISKSET = (1U<<4))
#define ST5_INDEX_Get()               ((PORTK >> 4) & 0x1U)
#define ST5_INDEX_PIN                  GPIO_PIN_RK4
#define ST5_INDEX_InterruptEnable()   (CNENKSET = (1U<<4))
#define ST5_INDEX_InterruptDisable()  (CNENKCLR = (1U<<4))

/*** Macros for ST5_DIAG pin ***/
#define ST5_DIAG_Set()               (LATKSET = (1U<<5))
#define ST5_DIAG_Clear()             (LATKCLR = (1U<<5))
#define ST5_DIAG_Toggle()            (LATKINV= (1U<<5))
#define ST5_DIAG_OutputEnable()      (TRISKCLR = (1U<<5))
#define ST5_DIAG_InputEnable()       (TRISKSET = (1U<<5))
#define ST5_DIAG_Get()               ((PORTK >> 5) & 0x1U)
#define ST5_DIAG_PIN                  GPIO_PIN_RK5

/*** Macros for ST5_ENABLE pin ***/
#define ST5_ENABLE_Set()               (LATKSET = (1U<<6))
#define ST5_ENABLE_Clear()             (LATKCLR = (1U<<6))
#define ST5_ENABLE_Toggle()            (LATKINV= (1U<<6))
#define ST5_ENABLE_OutputEnable()      (TRISKCLR = (1U<<6))
#define ST5_ENABLE_InputEnable()       (TRISKSET = (1U<<6))
#define ST5_ENABLE_Get()               ((PORTK >> 6) & 0x1U)
#define ST5_ENABLE_PIN                  GPIO_PIN_RK6

/*** Macros for I2C0_SCL pin ***/
#define I2C0_SCL_Get()               ((PORTA >> 14) & 0x1U)
#define I2C0_SCL_PIN                  GPIO_PIN_RA14

/*** Macros for I2C0_SDA pin ***/
#define I2C0_SDA_Get()               ((PORTA >> 15) & 0x1U)
#define I2C0_SDA_PIN                  GPIO_PIN_RA15

/*** Macros for ST4_DIR pin ***/
#define ST4_DIR_Set()               (LATDSET = (1U<<9))
#define ST4_DIR_Clear()             (LATDCLR = (1U<<9))
#define ST4_DIR_Toggle()            (LATDINV= (1U<<9))
#define ST4_DIR_OutputEnable()      (TRISDCLR = (1U<<9))
#define ST4_DIR_InputEnable()       (TRISDSET = (1U<<9))
#define ST4_DIR_Get()               ((PORTD >> 9) & 0x1U)
#define ST4_DIR_PIN                  GPIO_PIN_RD9

/*** Macros for ST4_STEP pin ***/
#define ST4_STEP_Set()               (LATDSET = (1U<<10))
#define ST4_STEP_Clear()             (LATDCLR = (1U<<10))
#define ST4_STEP_Toggle()            (LATDINV= (1U<<10))
#define ST4_STEP_OutputEnable()      (TRISDCLR = (1U<<10))
#define ST4_STEP_InputEnable()       (TRISDSET = (1U<<10))
#define ST4_STEP_Get()               ((PORTD >> 10) & 0x1U)
#define ST4_STEP_PIN                  GPIO_PIN_RD10

/*** Macros for ST4_INDEX pin ***/
#define ST4_INDEX_Set()               (LATDSET = (1U<<11))
#define ST4_INDEX_Clear()             (LATDCLR = (1U<<11))
#define ST4_INDEX_Toggle()            (LATDINV= (1U<<11))
#define ST4_INDEX_OutputEnable()      (TRISDCLR = (1U<<11))
#define ST4_INDEX_InputEnable()       (TRISDSET = (1U<<11))
#define ST4_INDEX_Get()               ((PORTD >> 11) & 0x1U)
#define ST4_INDEX_PIN                  GPIO_PIN_RD11
#define ST4_INDEX_InterruptEnable()   (CNENDSET = (1U<<11))
#define ST4_INDEX_InterruptDisable()  (CNENDCLR = (1U<<11))

/*** Macros for ST4_DIAG pin ***/
#define ST4_DIAG_Set()               (LATHSET = (1U<<12))
#define ST4_DIAG_Clear()             (LATHCLR = (1U<<12))
#define ST4_DIAG_Toggle()            (LATHINV= (1U<<12))
#define ST4_DIAG_OutputEnable()      (TRISHCLR = (1U<<12))
#define ST4_DIAG_InputEnable()       (TRISHSET = (1U<<12))
#define ST4_DIAG_Get()               ((PORTH >> 12) & 0x1U)
#define ST4_DIAG_PIN                  GPIO_PIN_RH12

/*** Macros for ST4_ENABLE pin ***/
#define ST4_ENABLE_Set()               (LATHSET = (1U<<13))
#define ST4_ENABLE_Clear()             (LATHCLR = (1U<<13))
#define ST4_ENABLE_Toggle()            (LATHINV= (1U<<13))
#define ST4_ENABLE_OutputEnable()      (TRISHCLR = (1U<<13))
#define ST4_ENABLE_InputEnable()       (TRISHSET = (1U<<13))
#define ST4_ENABLE_Get()               ((PORTH >> 13) & 0x1U)
#define ST4_ENABLE_PIN                  GPIO_PIN_RH13

/*** Macros for ENCX_B pin ***/
#define ENCX_B_Set()               (LATHSET = (1U<<15))
#define ENCX_B_Clear()             (LATHCLR = (1U<<15))
#define ENCX_B_Toggle()            (LATHINV= (1U<<15))
#define ENCX_B_OutputEnable()      (TRISHCLR = (1U<<15))
#define ENCX_B_InputEnable()       (TRISHSET = (1U<<15))
#define ENCX_B_Get()               ((PORTH >> 15) & 0x1U)
#define ENCX_B_PIN                  GPIO_PIN_RH15
#define ENCX_B_InterruptEnable()   (CNENHSET = (1U<<15))
#define ENCX_B_InterruptDisable()  (CNENHCLR = (1U<<15))

/*** Macros for SPI0_INT pin ***/
#define SPI0_INT_Set()               (LATDSET = (1U<<0))
#define SPI0_INT_Clear()             (LATDCLR = (1U<<0))
#define SPI0_INT_Toggle()            (LATDINV= (1U<<0))
#define SPI0_INT_OutputEnable()      (TRISDCLR = (1U<<0))
#define SPI0_INT_InputEnable()       (TRISDSET = (1U<<0))
#define SPI0_INT_Get()               ((PORTD >> 0) & 0x1U)
#define SPI0_INT_PIN                  GPIO_PIN_RD0

/*** Macros for SPI0_MISO pin ***/
#define SPI0_MISO_Get()               ((PORTC >> 13) & 0x1U)
#define SPI0_MISO_PIN                  GPIO_PIN_RC13

/*** Macros for SPI0_MOSI pin ***/
#define SPI0_MOSI_Get()               ((PORTC >> 14) & 0x1U)
#define SPI0_MOSI_PIN                  GPIO_PIN_RC14

/*** Macros for SPI0_CLK pin ***/
#define SPI0_CLK_Get()               ((PORTD >> 1) & 0x1U)
#define SPI0_CLK_PIN                  GPIO_PIN_RD1

/*** Macros for UART5_RX pin ***/
#define UART5_RX_Get()               ((PORTD >> 2) & 0x1U)
#define UART5_RX_PIN                  GPIO_PIN_RD2

/*** Macros for UART5_TX pin ***/
#define UART5_TX_Get()               ((PORTD >> 3) & 0x1U)
#define UART5_TX_PIN                  GPIO_PIN_RD3

/*** Macros for SPI0_CS pin ***/
#define SPI0_CS_Get()               ((PORTD >> 12) & 0x1U)
#define SPI0_CS_PIN                  GPIO_PIN_RD12
#define SPI0_CS_InterruptEnable()   (CNENDSET = (1U<<12))
#define SPI0_CS_InterruptDisable()  (CNENDCLR = (1U<<12))

/*** Macros for ENCX_A pin ***/
#define ENCX_A_Set()               (LATDSET = (1U<<13))
#define ENCX_A_Clear()             (LATDCLR = (1U<<13))
#define ENCX_A_Toggle()            (LATDINV= (1U<<13))
#define ENCX_A_OutputEnable()      (TRISDCLR = (1U<<13))
#define ENCX_A_InputEnable()       (TRISDSET = (1U<<13))
#define ENCX_A_Get()               ((PORTD >> 13) & 0x1U)
#define ENCX_A_PIN                  GPIO_PIN_RD13

/*** Macros for ENCX_Z pin ***/
#define ENCX_Z_Set()               (LATJSET = (1U<<0))
#define ENCX_Z_Clear()             (LATJCLR = (1U<<0))
#define ENCX_Z_Toggle()            (LATJINV= (1U<<0))
#define ENCX_Z_OutputEnable()      (TRISJCLR = (1U<<0))
#define ENCX_Z_InputEnable()       (TRISJSET = (1U<<0))
#define ENCX_Z_Get()               ((PORTJ >> 0) & 0x1U)
#define ENCX_Z_PIN                  GPIO_PIN_RJ0

/*** Macros for ENCY_ENABLE pin ***/
#define ENCY_ENABLE_Set()               (LATJSET = (1U<<1))
#define ENCY_ENABLE_Clear()             (LATJCLR = (1U<<1))
#define ENCY_ENABLE_Toggle()            (LATJINV= (1U<<1))
#define ENCY_ENABLE_OutputEnable()      (TRISJCLR = (1U<<1))
#define ENCY_ENABLE_InputEnable()       (TRISJSET = (1U<<1))
#define ENCY_ENABLE_Get()               ((PORTJ >> 1) & 0x1U)
#define ENCY_ENABLE_PIN                  GPIO_PIN_RJ1

/*** Macros for ENCX_ENABLE pin ***/
#define ENCX_ENABLE_Set()               (LATJSET = (1U<<2))
#define ENCX_ENABLE_Clear()             (LATJCLR = (1U<<2))
#define ENCX_ENABLE_Toggle()            (LATJINV= (1U<<2))
#define ENCX_ENABLE_OutputEnable()      (TRISJCLR = (1U<<2))
#define ENCX_ENABLE_InputEnable()       (TRISJSET = (1U<<2))
#define ENCX_ENABLE_Get()               ((PORTJ >> 2) & 0x1U)
#define ENCX_ENABLE_PIN                  GPIO_PIN_RJ2

/*** Macros for ENCY_A pin ***/
#define ENCY_A_Set()               (LATJSET = (1U<<3))
#define ENCY_A_Clear()             (LATJCLR = (1U<<3))
#define ENCY_A_Toggle()            (LATJINV= (1U<<3))
#define ENCY_A_OutputEnable()      (TRISJCLR = (1U<<3))
#define ENCY_A_InputEnable()       (TRISJSET = (1U<<3))
#define ENCY_A_Get()               ((PORTJ >> 3) & 0x1U)
#define ENCY_A_PIN                  GPIO_PIN_RJ3
#define ENCY_A_InterruptEnable()   (CNENJSET = (1U<<3))
#define ENCY_A_InterruptDisable()  (CNENJCLR = (1U<<3))

/*** Macros for UART4_TX pin ***/
#define UART4_TX_Get()               ((PORTD >> 4) & 0x1U)
#define UART4_TX_PIN                  GPIO_PIN_RD4

/*** Macros for UART4_RX pin ***/
#define UART4_RX_Get()               ((PORTD >> 5) & 0x1U)
#define UART4_RX_PIN                  GPIO_PIN_RD5

/*** Macros for ENCY_Z pin ***/
#define ENCY_Z_Set()               (LATDSET = (1U<<6))
#define ENCY_Z_Clear()             (LATDCLR = (1U<<6))
#define ENCY_Z_Toggle()            (LATDINV= (1U<<6))
#define ENCY_Z_OutputEnable()      (TRISDCLR = (1U<<6))
#define ENCY_Z_InputEnable()       (TRISDSET = (1U<<6))
#define ENCY_Z_Get()               ((PORTD >> 6) & 0x1U)
#define ENCY_Z_PIN                  GPIO_PIN_RD6

/*** Macros for ENCY_B pin ***/
#define ENCY_B_Set()               (LATDSET = (1U<<7))
#define ENCY_B_Clear()             (LATDCLR = (1U<<7))
#define ENCY_B_Toggle()            (LATDINV= (1U<<7))
#define ENCY_B_OutputEnable()      (TRISDCLR = (1U<<7))
#define ENCY_B_InputEnable()       (TRISDSET = (1U<<7))
#define ENCY_B_Get()               ((PORTD >> 7) & 0x1U)
#define ENCY_B_PIN                  GPIO_PIN_RD7

/*** Macros for UART1_TX pin ***/
#define UART1_TX_Get()               ((PORTF >> 0) & 0x1U)
#define UART1_TX_PIN                  GPIO_PIN_RF0

/*** Macros for UART1_RX pin ***/
#define UART1_RX_Get()               ((PORTF >> 1) & 0x1U)
#define UART1_RX_PIN                  GPIO_PIN_RF1

/*** Macros for ENCC_ENABLE pin ***/
#define ENCC_ENABLE_Set()               (LATKSET = (1U<<7))
#define ENCC_ENABLE_Clear()             (LATKCLR = (1U<<7))
#define ENCC_ENABLE_Toggle()            (LATKINV= (1U<<7))
#define ENCC_ENABLE_OutputEnable()      (TRISKCLR = (1U<<7))
#define ENCC_ENABLE_InputEnable()       (TRISKSET = (1U<<7))
#define ENCC_ENABLE_Get()               ((PORTK >> 7) & 0x1U)
#define ENCC_ENABLE_PIN                  GPIO_PIN_RK7

/*** Macros for UART3_TX pin ***/
#define UART3_TX_Get()               ((PORTG >> 1) & 0x1U)
#define UART3_TX_PIN                  GPIO_PIN_RG1

/*** Macros for UART3_RX pin ***/
#define UART3_RX_Get()               ((PORTG >> 0) & 0x1U)
#define UART3_RX_PIN                  GPIO_PIN_RG0

/*** Macros for ENCZ_B pin ***/
#define ENCZ_B_Set()               (LATASET = (1U<<6))
#define ENCZ_B_Clear()             (LATACLR = (1U<<6))
#define ENCZ_B_Toggle()            (LATAINV= (1U<<6))
#define ENCZ_B_OutputEnable()      (TRISACLR = (1U<<6))
#define ENCZ_B_InputEnable()       (TRISASET = (1U<<6))
#define ENCZ_B_Get()               ((PORTA >> 6) & 0x1U)
#define ENCZ_B_PIN                  GPIO_PIN_RA6

/*** Macros for ENCZ_A pin ***/
#define ENCZ_A_Set()               (LATASET = (1U<<7))
#define ENCZ_A_Clear()             (LATACLR = (1U<<7))
#define ENCZ_A_Toggle()            (LATAINV= (1U<<7))
#define ENCZ_A_OutputEnable()      (TRISACLR = (1U<<7))
#define ENCZ_A_InputEnable()       (TRISASET = (1U<<7))
#define ENCZ_A_Get()               ((PORTA >> 7) & 0x1U)
#define ENCZ_A_PIN                  GPIO_PIN_RA7
#define ENCZ_A_InterruptEnable()   (CNENASET = (1U<<7))
#define ENCZ_A_InterruptDisable()  (CNENACLR = (1U<<7))

/*** Macros for ENCZ_Z pin ***/
#define ENCZ_Z_Set()               (LATJSET = (1U<<4))
#define ENCZ_Z_Clear()             (LATJCLR = (1U<<4))
#define ENCZ_Z_Toggle()            (LATJINV= (1U<<4))
#define ENCZ_Z_OutputEnable()      (TRISJCLR = (1U<<4))
#define ENCZ_Z_InputEnable()       (TRISJSET = (1U<<4))
#define ENCZ_Z_Get()               ((PORTJ >> 4) & 0x1U)
#define ENCZ_Z_PIN                  GPIO_PIN_RJ4

/*** Macros for ENCC_Z pin ***/
#define ENCC_Z_Set()               (LATJSET = (1U<<5))
#define ENCC_Z_Clear()             (LATJCLR = (1U<<5))
#define ENCC_Z_Toggle()            (LATJINV= (1U<<5))
#define ENCC_Z_OutputEnable()      (TRISJCLR = (1U<<5))
#define ENCC_Z_InputEnable()       (TRISJSET = (1U<<5))
#define ENCC_Z_Get()               ((PORTJ >> 5) & 0x1U)
#define ENCC_Z_PIN                  GPIO_PIN_RJ5

/*** Macros for ENCC_B pin ***/
#define ENCC_B_Set()               (LATJSET = (1U<<6))
#define ENCC_B_Clear()             (LATJCLR = (1U<<6))
#define ENCC_B_Toggle()            (LATJINV= (1U<<6))
#define ENCC_B_OutputEnable()      (TRISJCLR = (1U<<6))
#define ENCC_B_InputEnable()       (TRISJSET = (1U<<6))
#define ENCC_B_Get()               ((PORTJ >> 6) & 0x1U)
#define ENCC_B_PIN                  GPIO_PIN_RJ6

/*** Macros for ENCC_A pin ***/
#define ENCC_A_Set()               (LATJSET = (1U<<7))
#define ENCC_A_Clear()             (LATJCLR = (1U<<7))
#define ENCC_A_Toggle()            (LATJINV= (1U<<7))
#define ENCC_A_OutputEnable()      (TRISJCLR = (1U<<7))
#define ENCC_A_InputEnable()       (TRISJSET = (1U<<7))
#define ENCC_A_Get()               ((PORTJ >> 7) & 0x1U)
#define ENCC_A_PIN                  GPIO_PIN_RJ7
#define ENCC_A_InterruptEnable()   (CNENJSET = (1U<<7))
#define ENCC_A_InterruptDisable()  (CNENJCLR = (1U<<7))

/*** Macros for ENCZ_ENABLE pin ***/
#define ENCZ_ENABLE_Set()               (LATESET = (1U<<0))
#define ENCZ_ENABLE_Clear()             (LATECLR = (1U<<0))
#define ENCZ_ENABLE_Toggle()            (LATEINV= (1U<<0))
#define ENCZ_ENABLE_OutputEnable()      (TRISECLR = (1U<<0))
#define ENCZ_ENABLE_InputEnable()       (TRISESET = (1U<<0))
#define ENCZ_ENABLE_Get()               ((PORTE >> 0) & 0x1U)
#define ENCZ_ENABLE_PIN                  GPIO_PIN_RE0

/*** Macros for STX_ENABLE pin ***/
#define STX_ENABLE_Set()               (LATESET = (1U<<1))
#define STX_ENABLE_Clear()             (LATECLR = (1U<<1))
#define STX_ENABLE_Toggle()            (LATEINV= (1U<<1))
#define STX_ENABLE_OutputEnable()      (TRISECLR = (1U<<1))
#define STX_ENABLE_InputEnable()       (TRISESET = (1U<<1))
#define STX_ENABLE_Get()               ((PORTE >> 1) & 0x1U)
#define STX_ENABLE_PIN                  GPIO_PIN_RE1

/*** Macros for STX_DIAG pin ***/
#define STX_DIAG_Set()               (LATGSET = (1U<<14))
#define STX_DIAG_Clear()             (LATGCLR = (1U<<14))
#define STX_DIAG_Toggle()            (LATGINV= (1U<<14))
#define STX_DIAG_OutputEnable()      (TRISGCLR = (1U<<14))
#define STX_DIAG_InputEnable()       (TRISGSET = (1U<<14))
#define STX_DIAG_Get()               ((PORTG >> 14) & 0x1U)
#define STX_DIAG_PIN                  GPIO_PIN_RG14

/*** Macros for STX_INDEX pin ***/
#define STX_INDEX_Set()               (LATGSET = (1U<<12))
#define STX_INDEX_Clear()             (LATGCLR = (1U<<12))
#define STX_INDEX_Toggle()            (LATGINV= (1U<<12))
#define STX_INDEX_OutputEnable()      (TRISGCLR = (1U<<12))
#define STX_INDEX_InputEnable()       (TRISGSET = (1U<<12))
#define STX_INDEX_Get()               ((PORTG >> 12) & 0x1U)
#define STX_INDEX_PIN                  GPIO_PIN_RG12
#define STX_INDEX_InterruptEnable()   (CNENGSET = (1U<<12))
#define STX_INDEX_InterruptDisable()  (CNENGCLR = (1U<<12))

/*** Macros for STX_STEP pin ***/
#define STX_STEP_Set()               (LATGSET = (1U<<13))
#define STX_STEP_Clear()             (LATGCLR = (1U<<13))
#define STX_STEP_Toggle()            (LATGINV= (1U<<13))
#define STX_STEP_OutputEnable()      (TRISGCLR = (1U<<13))
#define STX_STEP_InputEnable()       (TRISGSET = (1U<<13))
#define STX_STEP_Get()               ((PORTG >> 13) & 0x1U)
#define STX_STEP_PIN                  GPIO_PIN_RG13

/*** Macros for STX_DIR pin ***/
#define STX_DIR_Set()               (LATESET = (1U<<2))
#define STX_DIR_Clear()             (LATECLR = (1U<<2))
#define STX_DIR_Toggle()            (LATEINV= (1U<<2))
#define STX_DIR_OutputEnable()      (TRISECLR = (1U<<2))
#define STX_DIR_InputEnable()       (TRISESET = (1U<<2))
#define STX_DIR_Get()               ((PORTE >> 2) & 0x1U)
#define STX_DIR_PIN                  GPIO_PIN_RE2

/*** Macros for STY_ENABLE pin ***/
#define STY_ENABLE_Set()               (LATESET = (1U<<3))
#define STY_ENABLE_Clear()             (LATECLR = (1U<<3))
#define STY_ENABLE_Toggle()            (LATEINV= (1U<<3))
#define STY_ENABLE_OutputEnable()      (TRISECLR = (1U<<3))
#define STY_ENABLE_InputEnable()       (TRISESET = (1U<<3))
#define STY_ENABLE_Get()               ((PORTE >> 3) & 0x1U)
#define STY_ENABLE_PIN                  GPIO_PIN_RE3

/*** Macros for STY_DIAG pin ***/
#define STY_DIAG_Set()               (LATESET = (1U<<4))
#define STY_DIAG_Clear()             (LATECLR = (1U<<4))
#define STY_DIAG_Toggle()            (LATEINV= (1U<<4))
#define STY_DIAG_OutputEnable()      (TRISECLR = (1U<<4))
#define STY_DIAG_InputEnable()       (TRISESET = (1U<<4))
#define STY_DIAG_Get()               ((PORTE >> 4) & 0x1U)
#define STY_DIAG_PIN                  GPIO_PIN_RE4


// *****************************************************************************
/* GPIO Port

  Summary:
    Identifies the available GPIO Ports.

  Description:
    This enumeration identifies the available GPIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/


#define    GPIO_PORT_A  (0)
#define    GPIO_PORT_B  (1)
#define    GPIO_PORT_C  (2)
#define    GPIO_PORT_D  (3)
#define    GPIO_PORT_E  (4)
#define    GPIO_PORT_F  (5)
#define    GPIO_PORT_G  (6)
#define    GPIO_PORT_H  (7)
#define    GPIO_PORT_J  (8)
#define    GPIO_PORT_K  (9)
typedef uint32_t GPIO_PORT;

typedef enum
{
    GPIO_INTERRUPT_ON_MISMATCH,
    GPIO_INTERRUPT_ON_RISING_EDGE,
    GPIO_INTERRUPT_ON_FALLING_EDGE,
    GPIO_INTERRUPT_ON_BOTH_EDGES,
}GPIO_INTERRUPT_STYLE;

// *****************************************************************************
/* GPIO Port Pins

  Summary:
    Identifies the available GPIO port pins.

  Description:
    This enumeration identifies the available GPIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/


#define     GPIO_PIN_RA0  (0U)
#define     GPIO_PIN_RA1  (1U)
#define     GPIO_PIN_RA2  (2U)
#define     GPIO_PIN_RA3  (3U)
#define     GPIO_PIN_RA4  (4U)
#define     GPIO_PIN_RA5  (5U)
#define     GPIO_PIN_RA6  (6U)
#define     GPIO_PIN_RA7  (7U)
#define     GPIO_PIN_RA9  (9U)
#define     GPIO_PIN_RA10  (10U)
#define     GPIO_PIN_RA14  (14U)
#define     GPIO_PIN_RA15  (15U)
#define     GPIO_PIN_RB0  (16U)
#define     GPIO_PIN_RB1  (17U)
#define     GPIO_PIN_RB2  (18U)
#define     GPIO_PIN_RB3  (19U)
#define     GPIO_PIN_RB4  (20U)
#define     GPIO_PIN_RB5  (21U)
#define     GPIO_PIN_RB6  (22U)
#define     GPIO_PIN_RB7  (23U)
#define     GPIO_PIN_RB8  (24U)
#define     GPIO_PIN_RB9  (25U)
#define     GPIO_PIN_RB10  (26U)
#define     GPIO_PIN_RB11  (27U)
#define     GPIO_PIN_RB12  (28U)
#define     GPIO_PIN_RB13  (29U)
#define     GPIO_PIN_RB14  (30U)
#define     GPIO_PIN_RB15  (31U)
#define     GPIO_PIN_RC1  (33U)
#define     GPIO_PIN_RC2  (34U)
#define     GPIO_PIN_RC3  (35U)
#define     GPIO_PIN_RC4  (36U)
#define     GPIO_PIN_RC12  (44U)
#define     GPIO_PIN_RC13  (45U)
#define     GPIO_PIN_RC14  (46U)
#define     GPIO_PIN_RC15  (47U)
#define     GPIO_PIN_RD0  (48U)
#define     GPIO_PIN_RD1  (49U)
#define     GPIO_PIN_RD2  (50U)
#define     GPIO_PIN_RD3  (51U)
#define     GPIO_PIN_RD4  (52U)
#define     GPIO_PIN_RD5  (53U)
#define     GPIO_PIN_RD6  (54U)
#define     GPIO_PIN_RD7  (55U)
#define     GPIO_PIN_RD9  (57U)
#define     GPIO_PIN_RD10  (58U)
#define     GPIO_PIN_RD11  (59U)
#define     GPIO_PIN_RD12  (60U)
#define     GPIO_PIN_RD13  (61U)
#define     GPIO_PIN_RD14  (62U)
#define     GPIO_PIN_RD15  (63U)
#define     GPIO_PIN_RE0  (64U)
#define     GPIO_PIN_RE1  (65U)
#define     GPIO_PIN_RE2  (66U)
#define     GPIO_PIN_RE3  (67U)
#define     GPIO_PIN_RE4  (68U)
#define     GPIO_PIN_RE5  (69U)
#define     GPIO_PIN_RE6  (70U)
#define     GPIO_PIN_RE7  (71U)
#define     GPIO_PIN_RE8  (72U)
#define     GPIO_PIN_RE9  (73U)
#define     GPIO_PIN_RF0  (80U)
#define     GPIO_PIN_RF1  (81U)
#define     GPIO_PIN_RF2  (82U)
#define     GPIO_PIN_RF3  (83U)
#define     GPIO_PIN_RF4  (84U)
#define     GPIO_PIN_RF5  (85U)
#define     GPIO_PIN_RF8  (88U)
#define     GPIO_PIN_RF12  (92U)
#define     GPIO_PIN_RF13  (93U)
#define     GPIO_PIN_RG0  (96U)
#define     GPIO_PIN_RG1  (97U)
#define     GPIO_PIN_RG6  (102U)
#define     GPIO_PIN_RG7  (103U)
#define     GPIO_PIN_RG8  (104U)
#define     GPIO_PIN_RG9  (105U)
#define     GPIO_PIN_RG12  (108U)
#define     GPIO_PIN_RG13  (109U)
#define     GPIO_PIN_RG14  (110U)
#define     GPIO_PIN_RG15  (111U)
#define     GPIO_PIN_RH0  (112U)
#define     GPIO_PIN_RH1  (113U)
#define     GPIO_PIN_RH2  (114U)
#define     GPIO_PIN_RH3  (115U)
#define     GPIO_PIN_RH4  (116U)
#define     GPIO_PIN_RH5  (117U)
#define     GPIO_PIN_RH6  (118U)
#define     GPIO_PIN_RH7  (119U)
#define     GPIO_PIN_RH8  (120U)
#define     GPIO_PIN_RH9  (121U)
#define     GPIO_PIN_RH10  (122U)
#define     GPIO_PIN_RH11  (123U)
#define     GPIO_PIN_RH12  (124U)
#define     GPIO_PIN_RH13  (125U)
#define     GPIO_PIN_RH14  (126U)
#define     GPIO_PIN_RH15  (127U)
#define     GPIO_PIN_RJ0  (128U)
#define     GPIO_PIN_RJ1  (129U)
#define     GPIO_PIN_RJ2  (130U)
#define     GPIO_PIN_RJ3  (131U)
#define     GPIO_PIN_RJ4  (132U)
#define     GPIO_PIN_RJ5  (133U)
#define     GPIO_PIN_RJ6  (134U)
#define     GPIO_PIN_RJ7  (135U)
#define     GPIO_PIN_RJ8  (136U)
#define     GPIO_PIN_RJ9  (137U)
#define     GPIO_PIN_RJ10  (138U)
#define     GPIO_PIN_RJ11  (139U)
#define     GPIO_PIN_RJ12  (140U)
#define     GPIO_PIN_RJ13  (141U)
#define     GPIO_PIN_RJ14  (142U)
#define     GPIO_PIN_RJ15  (143U)
#define     GPIO_PIN_RK0  (144U)
#define     GPIO_PIN_RK1  (145U)
#define     GPIO_PIN_RK2  (146U)
#define     GPIO_PIN_RK3  (147U)
#define     GPIO_PIN_RK4  (148U)
#define     GPIO_PIN_RK5  (149U)
#define     GPIO_PIN_RK6  (150U)
#define     GPIO_PIN_RK7  (151U)

    /* This element should not be used in any of the GPIO APIs.
       It will be used by other modules or application to denote that none of the GPIO Pin is used */
#define    GPIO_PIN_NONE   (-1)

typedef uint32_t GPIO_PIN;

typedef  void (*GPIO_PIN_CALLBACK) ( GPIO_PIN pin, uintptr_t context);

void GPIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t GPIO_PortRead(GPIO_PORT port);

void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value);

uint32_t GPIO_PortLatchRead ( GPIO_PORT port );

void GPIO_PortSet(GPIO_PORT port, uint32_t mask);

void GPIO_PortClear(GPIO_PORT port, uint32_t mask);

void GPIO_PortToggle(GPIO_PORT port, uint32_t mask);

void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortInterruptEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortInterruptDisable(GPIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: Local Data types and Prototypes
// *****************************************************************************
// *****************************************************************************

typedef struct {

    /* target pin */
    GPIO_PIN                 pin;

    /* Callback for event on target pin*/
    GPIO_PIN_CALLBACK        callback;

    /* Callback Context */
    uintptr_t               context;

} GPIO_PIN_CALLBACK_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void GPIO_PinWrite(GPIO_PIN pin, bool value)
{
	 uint32_t xvalue = (uint32_t)value;
    GPIO_PortWrite((pin>>4U), (uint32_t)(0x1U) << (pin & 0xFU), (xvalue) << (pin & 0xFU));
}

static inline bool GPIO_PinRead(GPIO_PIN pin)
{
    return ((((GPIO_PortRead((GPIO_PORT)(pin>>4U))) >> (pin & 0xFU)) & 0x1U) != 0U);
}

static inline bool GPIO_PinLatchRead(GPIO_PIN pin)
{
    return (((GPIO_PortLatchRead((GPIO_PORT)(pin>>4U)) >> (pin & 0xFU)) & 0x1U) != 0U);
}

static inline void GPIO_PinToggle(GPIO_PIN pin)
{
    GPIO_PortToggle((pin>>4U), (uint32_t)0x1U << (pin & 0xFU));
}

static inline void GPIO_PinSet(GPIO_PIN pin)
{
    GPIO_PortSet((pin>>4U), (uint32_t)0x1U << (pin & 0xFU));
}

static inline void GPIO_PinClear(GPIO_PIN pin)
{
    GPIO_PortClear((pin>>4U), (uint32_t)0x1U << (pin & 0xFU));
}

static inline void GPIO_PinInputEnable(GPIO_PIN pin)
{
    GPIO_PortInputEnable((pin>>4U), (uint32_t)0x1U << (pin & 0xFU));
}

static inline void GPIO_PinOutputEnable(GPIO_PIN pin)
{
    GPIO_PortOutputEnable((pin>>4U), (uint32_t)0x1U << (pin & 0xFU));
}

#define GPIO_PinInterruptEnable(pin)       GPIO_PinIntEnable(pin, GPIO_INTERRUPT_ON_MISMATCH)
#define GPIO_PinInterruptDisable(pin)      GPIO_PinIntDisable(pin)

void GPIO_PinIntEnable(GPIO_PIN pin, GPIO_INTERRUPT_STYLE style);
void GPIO_PinIntDisable(GPIO_PIN pin);

bool GPIO_PinInterruptCallbackRegister(
    GPIO_PIN pin,
    const   GPIO_PIN_CALLBACK callback,
    uintptr_t context
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_GPIO_H
