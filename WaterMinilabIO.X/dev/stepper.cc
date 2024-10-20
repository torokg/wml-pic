#include <chrono>
#include <io/host/service.hh>
#include "stepper.hh"

namespace dev
{
   
/*void
stepper::index_handler(GPIO_PIN pin)
{
    if(pin != pin_index)
        return;
    if(!GPIO_PinRead(pin))
        return;
    ++index_count;
    tx_semaphore_ceiling_put(&sem_notify,1);
}*/

stepper::stepper(HardwareSerial &serial, TMC2209::SerialAddress address, stepper_index_handler &sih, Encoder *enc, LimitSensor *lim, std::function<void()> fcb, GPIO_PIN penable, GPIO_PIN pdiag, uint8_t runCurrent, uint8_t holdCurrent, size_t spr, size_t mrpm, size_t macc)
        : index(sih)
        , encoder(enc)
        , limit(lim)
        , finish_callback(fcb)
        , last_index_count(0)
        , target_index(0)
        , current_index(0)
        , current_speed(0)
        , pin_diag(pdiag)
        , steps_per_rev(spr)
        , max_rpm(mrpm)
        , max_accel(macc)
{
    tx_semaphore_create(&sem_index,"",0);
    tx_semaphore_create(&sem_notify,"",0);

//    (void)GPIO_PinInterruptCallbackRegister(pin_index, &static_index_handler, reinterpret_cast<uintptr_t>(this));
//    GPIO_PinInterruptEnable(pin_index);

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
    //enableCoolStep();
    disableStealthChop();
    moveAtVelocity(0);
    enable();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    process = new std::thread(std::bind(&stepper::process_start,this));
}

void stepper::setFreewheel(bool v)
{
    if(!v)
    {
        std::unique_lock ul(target_index_lk);
        target_index = getPosition();
        moveAtVelocity(0);
        enable();
    }
    else
        disable();
}

void stepper::resetPosition()
{
    std::unique_lock ul(target_index_lk);
    moveAtVelocity(0);
    index.count = last_index_count = current_index = target_index = 0;
    if(encoder)
        encoder->position = 0;
}

int32_t stepper::getPosition()
{
    if(encoder)
        return encoder->position;
    else
        return current_index;
}

void stepper::process_start()
{
    index.enable();
    if(encoder)
        encoder->enable();
    moveAtVelocity(0);
    while(true)
    {
        const auto t0 = std::chrono::high_resolution_clock::now();
        
        std::unique_lock ul(target_index_lk);

        const size_t
            ic = index.count,
            lic = last_index_count,
            di = ic-lic;

        last_index_count = ic;
        
        const int32_t ti = target_index;
        
        const float mxacc = float(max_accel)*steps_per_rev*256/60;
        const float max_velocity = float(max_rpm)*steps_per_rev*256/60;
       
        if(encoder)
        {
            current_index = encoder->position;
            const float dist = ti-encoder->position;

            if(abs(dist) > 1)
            {
                int sgn = (dist < 0) ? -1 : 1;
                const float suggested_velocity = abs(float(dist)*256/10/(current_speed/2)*mxacc);
                const int32_t new_speed = (int32_t)(std::clamp(sgn*std::min(max_velocity,suggested_velocity), current_speed - mxacc/100, current_speed + mxacc/100));

                if(abs(dist) > 10)
                {
                    current_speed = new_speed;
                    moveAtVelocity(current_speed);
                    ul.unlock();
                    std::this_thread::sleep_until(t0+std::chrono::milliseconds(10));
                }

                else
                {
                    if(!current_speed)
                    {
                        current_speed = new_speed;
                        moveAtVelocity(current_speed);
                    }
                    ul.unlock();
                    std::this_thread::sleep_until(t0+std::chrono::milliseconds(1));
                }
                continue;
            }
        }
        else
        {
            if(current_speed > 0)
                current_index += di;
            else
                current_index -= di;
            const int32_t dist = ti-current_index;

            if(dist != 0)
            {
                const float suggested_velocity = abs(float(dist)*256*4/current_speed*mxacc);
                /*io::host::log(
                    "current index: ",current_index, "; target index: ",target_index,
                    "\n; max velocity: ", max_velocity, "; max accel: ", mxacc,
                    "\n; current speed: ", current_speed, "; suggested speed: ",suggested_velocity,
                    "\n; distance: ",dist,"; moving at velocity ",current_speed
               );*/

                int sgn = (dist < 0) ? -1 : 1;
                const int32_t new_speed = (int32_t)(std::clamp(sgn*std::min(max_velocity,suggested_velocity), current_speed - mxacc/100, current_speed + mxacc/100));
                current_speed = new_speed;

                if(abs(dist)>1)
                {
                    moveAtVelocity(current_speed);
                    ul.unlock();
                    std::this_thread::sleep_until(t0+std::chrono::milliseconds(10));
                }

                else
                {
                    if(!current_speed)
                    {
                        current_speed = new_speed;
                        moveAtVelocity(current_speed);
                    }
                    ul.unlock();
                    std::this_thread::sleep_until(t0+std::chrono::milliseconds(1));
                }

                continue;
            }
        }
        
        ul.unlock();
        moveAtVelocity(0);
        current_speed = 0;
        
        io::host::log("Arrived at position ",current_index); 
        finish_callback();
        tx_semaphore_get(&sem_notify,TX_WAIT_FOREVER);
        io::host::log("Departing from position ",current_index); 
    }
}


}

std::array<stepper_index_handler, 10> stepper_index = {
    stepper_index_handler{STX_INDEX_PIN},
    stepper_index_handler{STY_INDEX_PIN},
    stepper_index_handler{STZ_INDEX_PIN},
    stepper_index_handler{STC_INDEX_PIN},
    stepper_index_handler{ST4_INDEX_PIN},
    stepper_index_handler{ST5_INDEX_PIN},
    stepper_index_handler{ST6_INDEX_PIN},
    stepper_index_handler{ST7_INDEX_PIN},
    stepper_index_handler{ST8_INDEX_PIN},
    stepper_index_handler{ST9_INDEX_PIN}
};

