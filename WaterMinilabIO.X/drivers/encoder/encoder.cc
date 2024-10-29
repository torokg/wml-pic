#include "../encoder.h"


void
Encoder::notify()
{
    uint8_t r =
        (GPIO_PinRead(pin_a)?1:0) +
        (GPIO_PinRead(pin_b)?2:0);

    const uint8_t prev = prev_state;
    if(prev == r)
        return;
    prev_state = r;
//    if(intpin)
//    {
    if(((prev == 0) && (r == 3)) || ((prev == 3) && (r == 0))  || ((prev == 2) && (r == 0))  || ((prev == 1) && (r == 3)))
        position += direction;
    else if(((prev == 1) && (r == 2)) || ((prev == 2) && (r == 1)) || ((prev == 0) && (r == 2)) || ((prev == 3) && (r == 1)))
        position -= direction;
//    }
#if 0
    else
    {
        if(((prev == 2) && (r == 3))/* || ((prev == 3) && (r == 2)) || ((prev == 0) && (r == 3))*/)
            position += 1;
        else if(((prev == 0) && (r == 1))/* || ((prev == 1) && (r == 0)) || ((prev == 2) && (r == 1)) || ((prev == 3) && (r == 0))*/)
            position -= 1;
        else
        {
            __builtin_software_breakpoint();
        }
    }
#endif
}

std::array<Encoder,4> encoders = {
    Encoder{ENCX_ENABLE_PIN, ENCX_A_PIN, ENCX_B_PIN, ENCX_Z_PIN, 1,1},
    Encoder{ENCY_ENABLE_PIN, ENCY_A_PIN, ENCY_B_PIN, ENCY_Z_PIN, 0,1},
    Encoder{ENCZ_ENABLE_PIN, ENCZ_A_PIN, ENCZ_B_PIN, ENCZ_Z_PIN, 1,1},
    Encoder{ENCC_ENABLE_PIN, ENCC_A_PIN, ENCC_B_PIN, ENCC_Z_PIN, 1,1},
};