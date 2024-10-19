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
    if(status)
    {
        stepper_index[2].notify();
        stepper_index[3].notify();
    }
}

extern "C"
void CHANGE_NOTICE_G_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATG;
    status &= CNENG;
    PORTG;
    IFS3CLR = _IFS3_CNGIF_MASK;
    if(status)
    {
        stepper_index[0].notify();
        stepper_index[1].notify();
    }
}

extern "C"
void CHANGE_NOTICE_H_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATH;
    status &= CNENH;
    volatile int x = PORTH;
    IFS3CLR = _IFS3_CNHIF_MASK;
    if(status)
    {
        encoders[0].notify();
        stepper_index[6].notify();
        stepper_index[9].notify();
    }
}

extern "C"
void CHANGE_NOTICE_J_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATJ;
    status &= CNENJ;
    PORTJ;
    IFS3CLR = _IFS3_CNJIF_MASK;
    if(status)
    {
        encoders[1].notify();
        encoders[3].notify();
    }
}

extern "C"
void CHANGE_NOTICE_K_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATG;
    status &= CNENG;
    PORTG;
    IFS3CLR = _IFS3_CNGIF_MASK;
    if(status)
    {
        stepper_index[5].notify();
        stepper_index[7].notify();
    }
}