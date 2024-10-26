#include "definitions.h"
#include <drivers/encoder.h>
#include <dev/stepper.hh>

extern "C"
void CHANGE_NOTICE_A_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATA;
    status &= CNENA;
    PORTA;
    IFS3CLR = _IFS3_CNAIF_MASK;
    if(status)
        encoders[2].notify();
}

extern "C"
void CHANGE_NOTICE_C_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATC;
    status &= CNENC;
    PORTC;
    IFS3CLR = _IFS3_CNCIF_MASK;
    if((status & ((uint32_t)1U << (STZ_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[2].notify();
    
    if((status & ((uint32_t)1U << (STC_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[3].notify();
}

extern "C" void stepper_index_notify(uint8_t index);
extern "C" void SPI1_CS_Handler();

extern "C"
void CHANGE_NOTICE_D_InterruptHandler(void)
{
    uint8_t i;
    uint32_t status;
    GPIO_PIN pin;
    uintptr_t context;

    status  = CNSTATD;
    status &= CNEND;

    PORTD;
    IFS3CLR = _IFS3_CNDIF_MASK;

    if((status & ((uint32_t)1U << (SPI0_CS_PIN & 0xFU))) != 0U)
        SPI1_CS_Handler();
    
    if((status & ((uint32_t)1U << (ST4_INDEX_PIN & 0xFU))) != 0U)
        stepper_index_notify(4);
}
extern "C"
void CHANGE_NOTICE_G_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATG;
    status &= CNENG;
    PORTG;
    IFS3CLR = _IFS3_CNGIF_MASK;
    
    if((status & ((uint32_t)1U << (STX_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[0].notify();
    
    if((status & ((uint32_t)1U << (STY_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[1].notify();
}

extern "C"
void CHANGE_NOTICE_H_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATH;
    status &= CNENH;
    volatile int x = PORTH;
    IFS3CLR = _IFS3_CNHIF_MASK;
    
    if((status & ((uint32_t)1U << (ENCX_B_PIN & 0xFU))) != 0U)
        encoders[0].notify();
    
    if((status & ((uint32_t)1U << (ST6_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[6].notify();
    
    if((status & ((uint32_t)1U << (ST9_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[9].notify();
}

extern "C"
void CHANGE_NOTICE_J_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATJ;
    status &= CNENJ;
    PORTJ;
    IFS3CLR = _IFS3_CNJIF_MASK;
    
    if((status & ((uint32_t)1U << (ENCY_B_PIN & 0xFU))) != 0U)
        encoders[1].notify();
    
    if((status & ((uint32_t)1U << (ENCC_B_PIN & 0xFU))) != 0U)
        encoders[3].notify();
    
    if((status & ((uint32_t)1U << (ST8_INDEX_PIN & 0xFU))) != 0U)
        stepper_index_notify(8);
}

extern "C"
void CHANGE_NOTICE_K_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATK;
    status &= CNENK;
    PORTK;
    IFS3CLR = _IFS3_CNKIF_MASK;
    
    if((status & ((uint32_t)1U << (ST5_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[5].notify();
    
    if((status & ((uint32_t)1U << (ST7_INDEX_PIN & 0xFU))) != 0U)
        stepper_index[7].notify();
}