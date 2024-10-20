#ifndef ARGTYPES_HH
# define ARGTYPES_HH

template<typename T>
struct value_set_t
{
    T value;
    uint8_t item;
};

struct position_t
{
    int32_t x;
    int32_t y;
    int32_t z;
};

struct limit_t
{
    bool x;
    bool y;
    bool z;
};

#endif