#include "../encoder.h"


void
Encoder::notify()
{
    uint8_t r =
        (GPIO_PinRead(pin_a)?1:0) +
        (GPIO_PinRead(pin_b)?2:0);
        //(GPIO_PinRead(pin_z)?4:0);

    const uint8_t prev = state[statep];
    if(prev == r)
        return;

    if(((prev == 0) && (r == 3)) || ((prev == 3) && (r == 0))  || ((prev == 2) && (r == 0))  || ((prev == 1) && (r == 3)))
        position -= 1;
    else if(((prev == 1) && (r == 2)) || ((prev == 2) && (r == 1)) || ((prev == 0) && (r == 2))|| ((prev == 3) && (r == 1)))
        position += 1;

    else
    {
        __builtin_software_breakpoint();
    }

    if(++statep > 3)
        statep = 0;

    state[statep] = r;
}

std::array<Encoder,4> encoders = {
    Encoder{ENCX_ENABLE_PIN, ENCX_A_PIN, ENCX_B_PIN, ENCX_Z_PIN},
    Encoder{ENCY_ENABLE_PIN, ENCY_A_PIN, ENCY_B_PIN, ENCY_Z_PIN},
    Encoder{ENCZ_ENABLE_PIN, ENCZ_A_PIN, ENCZ_B_PIN, ENCZ_Z_PIN},
    Encoder{ENCC_ENABLE_PIN, ENCC_A_PIN, ENCC_B_PIN, ENCC_Z_PIN},
};


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
void CHANGE_NOTICE_H_InterruptHandler()
{
    uint32_t status;
    status  = CNSTATH;
    status &= CNENH;
    volatile int x = PORTH;
    IFS3CLR = _IFS3_CNHIF_MASK;
    if(status)
        encoders[0].notify();
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