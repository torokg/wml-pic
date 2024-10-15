#ifndef DEV_STEPPER_HH
# define DEV_STEPPER_HH
# include <drivers/TMC2209.h>
namespace dev
{

class stepper
    : public TMC2209
{
    TX_SEMAPHORE sem_index;
    TX_SEMAPHORE sem_notify;
    std::mutex   target_index_lk;
    volatile size_t  index_count;
    volatile size_t  last_index_count;
    volatile int32_t target_index;
    volatile int32_t current_index;
    volatile int32_t current_speed;
    volatile bool last_dir;

    GPIO_PIN     pin_index;
    GPIO_PIN     pin_diag;
    size_t       steps_per_rev;
    size_t       max_rpm;
    size_t       max_accel;
    std::thread *process;
    
    static void static_index_handler(GPIO_PIN pin, uintptr_t context);
    
    void index_handler(GPIO_PIN pin);
    
    void process_start();
            
public:
    
    stepper(HardwareSerial &serial, TMC2209::SerialAddress address, GPIO_PIN penable, GPIO_PIN pindex, GPIO_PIN pdiag, uint8_t runCurrent, uint8_t holdCurrent, size_t spr, size_t mrpm, size_t macc);
    
    inline void setTargetIndex(int32_t v)
    {
        std::unique_lock ul(target_index_lk);
        target_index = v;
        tx_semaphore_ceiling_put(&sem_notify,1);
    }
};   

}

#endif