#ifndef DRIVERS_OC_H
# define DRIVERS_OC_H
struct oc_type
{
    void(*const set)(uint16_t value);
    size_t period;
};
#endif
