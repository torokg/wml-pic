#include <chrono>
#include "stepper.hh"

namespace dev
{
   
void
stepper::static_index_handler(GPIO_PIN pin, uintptr_t context)
{ reinterpret_cast<stepper*>(context)->index_handler(pin); }

void
stepper::index_handler(GPIO_PIN pin)
{
    if(pin != pin_index)
        return;
    ++index_count;
    tx_semaphore_ceiling_put(&sem_notify,1);
}

stepper::stepper(HardwareSerial &serial, TMC2209::SerialAddress address, GPIO_PIN penable, GPIO_PIN pindex, GPIO_PIN pdiag, uint8_t runCurrent, uint8_t holdCurrent, size_t spr, size_t mrpm, size_t macc)
        : index_count(0)
        , last_index_count(0)
        , target_index(0)
        , current_index(0)
        , current_speed(0)
        , pin_index(pindex)
        , pin_diag(pdiag)
        , steps_per_rev(spr)
        , max_rpm(mrpm)
        , max_accel(macc)
{
    tx_semaphore_create(&sem_index,"",0);
    tx_semaphore_create(&sem_notify,"",0);

    (void)GPIO_PinInterruptCallbackRegister(pin_index, &static_index_handler, reinterpret_cast<uintptr_t>(this));
    GPIO_PinInterruptEnable(pin_index);

    setup(serial,115200,address);

    if(!isCommunicating())
        io::host::log("\033[31;01m; TMC2209 is not communicating\033[0m");
    
    setHardwareEnablePin(penable);
    useExternalSenseResistors();
    setMicrostepsPerStep(256);
    setRunCurrent(runCurrent);
    setHoldCurrent(holdCurrent);
    setCoolStepCurrentIncrement(TMC2209::CURRENT_INCREMENT_8);
    setCoolStepMeasurementCount(TMC2209::MEASUREMENT_COUNT_32);
    enableAnalogCurrentScaling();
    enableCoolStep();
    disableStealthChop();
    enable();
    process = new std::thread(std::bind(&stepper::process_start,this));
}

void stepper::process_start()
{
    moveAtVelocity(0);
    while(true)
    {
        const auto t0 = std::chrono::high_resolution_clock::now();
        
        std::unique_lock ul(target_index_lk);
        const int32_t ti = target_index;
        ul.unlock();
        
        const size_t
            ic = index_count,
            lic = last_index_count,
            di = ic-lic;
        
        last_index_count = ic;
        
        if(current_speed > 0)
            current_index += di;
        else
            current_index -= di;
        
        const int32_t dist = ti-current_index;
        if(dist != 0)
        {
            const float mxacc = float(max_accel)*steps_per_rev*256/60;
            const float max_velocity = float(max_rpm)*steps_per_rev*256/60;
            const float suggested_velocity = abs(float(dist)*256*2/current_speed*mxacc);
            /*io::host::log(
                "current index: ",current_index, "; target index: ",target_index,
                "\n; max velocity: ", max_velocity, "; max accel: ", mxacc,
                "\n; current speed: ", current_speed, "; suggested speed: ",suggested_velocity,
                "\n; distance: ",dist,"; moving at velocity ",current_speed
           );*/
            
            int sgn = (dist < 0) ? -1 : 1;
            const int32_t new_speed = (int32_t)(std::clamp(sgn*std::min(max_velocity,suggested_velocity), current_speed - mxacc/100, current_speed + mxacc/100)*std::clamp((float)abs(dist)/3,0.3f,1.0f));
            
            if(abs(dist)>1)
            {
                if(current_speed != new_speed)
                {
                    current_speed = new_speed;
                    moveAtVelocity(current_speed);
                }
                std::this_thread::sleep_until(t0+std::chrono::milliseconds(10));
            }
            
            else
                tx_semaphore_get(&sem_notify,TX_WAIT_FOREVER);
            
            continue;
        }
        
        else
        {
            moveAtVelocity(0);
            current_speed = 0;
            io::host::log("Arrived at position ",current_index); 
        }
        
        
        tx_semaphore_get(&sem_notify,TX_WAIT_FOREVER);
        io::host::log("Departing from position ",current_index); 
    }
}


}

