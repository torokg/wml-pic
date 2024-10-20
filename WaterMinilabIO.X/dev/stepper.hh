#ifndef DEV_STEPPER_HH
# define DEV_STEPPER_HH
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
{
    TX_SEMAPHORE sem_index;
    TX_SEMAPHORE sem_notify;
    std::mutex   target_index_lk;
    stepper_index_handler &index;
    Encoder *encoder;
    LimitSensor *limit;
    std::function<void()> finish_callback;
    volatile size_t  last_index_count;
    volatile int32_t target_index;
    volatile int32_t current_index;
    volatile int32_t current_speed;
    volatile bool last_dir;

    GPIO_PIN     pin_diag;
    size_t       steps_per_rev;
    size_t       max_rpm;
    size_t       max_accel;
    std::thread *process;
    
    void process_start();
            
public:
    
    stepper(HardwareSerial &serial, TMC2209::SerialAddress address, stepper_index_handler &index, Encoder *enc, LimitSensor *lim, std::function<void()> fcb, GPIO_PIN penable, GPIO_PIN pdiag, uint8_t runCurrent, uint8_t holdCurrent, size_t spr, size_t mrpm, size_t macc);

    void init();
    
    inline void setTargetIndex(int32_t v)
    {
        std::unique_lock ul(target_index_lk);
        target_index = v;
        tx_semaphore_ceiling_put(&sem_notify,1);
    }
    
    void setFreewheel(bool v);
    
    int32_t getPosition();
    
    void resetPosition();
};   

}

extern std::array<stepper_index_handler, 10> stepper_index;

#endif