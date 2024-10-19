#ifndef DRIVERS_UART_HH
# define DRIVERS_UART_HH
# include "definitions.h"
# include <array>
# include <mutex>
# include <thread>
struct HardwareSerial
{
    std::mutex mutex;
    bool(*const SerialSetup)(UART_SERIAL_SETUP *setup, uint32_t srcClkFreq);
    size_t(*const Available)();
    size_t(*const Read)(uint8_t *dst, size_t size);
    size_t(*const Write)(uint8_t *src, size_t size);
    void(*const Flush)();
    
public:
    constexpr HardwareSerial
     ( bool(*const ss)(UART_SERIAL_SETUP *setup, uint32_t srcClkFreq)
     , size_t(*av)()
     , size_t(*rd)(uint8_t *dst, size_t size)
     , size_t(*wr)(uint8_t *src, size_t size)
     , void(*fl)()
     )
        : SerialSetup(ss)
        , Available(av)
        , Read(rd)
        , Write(wr)
        , Flush(fl)
     {}
        
    inline void begin(long baudrate)
    {
        UART_SERIAL_SETUP s;
        s.baudRate = baudrate;
        s.parity = UART_PARITY_NONE;
        s.dataWidth = UART_DATA_8_BIT;
        s.stopBits = UART_STOP_1_BIT;
        SerialSetup(&s,0);
    }
    
    
    constexpr void end(){}
    
        
    inline size_t available()
    { return Available(); }
    
    inline size_t write(uint8_t c)
    { return Write(&c,1); }
    
    inline size_t write(uint8_t *data, size_t size)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return Write(data,size);
    }
    
    inline int read()
    { 
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        uint8_t r;
        while(Read(&r,1) != 1);
        return r;
    }

    inline void flush()
    { Flush(); }

};

#endif