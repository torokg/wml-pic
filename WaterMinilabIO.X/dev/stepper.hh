#ifndef DEV_STEPPER_HH
# define DEV_STEPPER_HH
# include <mutex>
# include <condition_variable>
# include <drivers/TMC2209.h>
# include <drivers/encoder.h>
# include <drivers/LimitSensor.hh>

    
struct stepper_index_handler
{
    GPIO_PIN pin;
    volatile uint32_t count;
    volatile bool last;
    
    
    inline void enable()
    { GPIO_PinInterruptEnable(pin); }
    
    inline void disable()
    { GPIO_PinInterruptDisable(pin); }
    
    stepper_index_handler(GPIO_PIN p)
        : pin(p)
        , count(0)
        , last(false)
    { disable(); }
    
    inline void notify()
    {
        const bool p = (bool)GPIO_PinRead(pin);
        if(last == p)
            return;
        last = p;
        if(p)
            count += 1;
    }
};

namespace dev
{


class stepper
    : public TMC2209
    , public std::chrono::high_resolution_clock
{
    std::mutex lk;
    std::condition_variable cv;
    stepper_index_handler &index;
    Encoder *encoder;
    LimitSensor *limit;
    std::function<void()> finish_callback;
    volatile size_t  last_index_count;
    volatile int32_t target_index;
    volatile int32_t current_index;
    volatile int32_t current_position;
    volatile int32_t current_speed;

    GPIO_PIN     pin_diag;
    size_t       steps_per_rev;
    volatile size_t       max_rpm;
    volatile size_t       max_accel;
    volatile size_t       homing_retries;
    std::thread *process;
    volatile bool homing;
    volatile bool pending;
    
    void _resetPosition();
    
    bool do_target_approach(std::unique_lock<std::mutex> &ul, time_point &t);
    bool do_homing(std::unique_lock<std::mutex> &ul, time_point &t);
    
    void process_start();
            
public:
    
    stepper(HardwareSerial &serial, TMC2209::SerialAddress address, stepper_index_handler &index, Encoder *enc, LimitSensor *lim, std::function<void()> fcb, GPIO_PIN penable, GPIO_PIN pdiag, uint8_t runCurrent, uint8_t holdCurrent, size_t spr, size_t mrpm, size_t macc);

    void init();
    
    void setTargetIndex(int32_t v);
    
    void setFreewheel(bool v);
    
    int32_t getPosition();
    
    void resetPosition();
    
    void setMaxAccel(uint32_t v);
    void setMaxRpm(uint32_t v);
    
    void home();
};   

}

extern std::array<stepper_index_handler, 10> stepper_index;

#endif