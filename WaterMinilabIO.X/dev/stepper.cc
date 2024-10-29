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
        , homing(false)
        , pending(false)
        , departed(false)
{

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
        std::unique_lock ul(lk);
        target_index = getPosition();
        current_speed = 0;
        moveAtVelocity(0);
        enable();
    }
    else
        disable();
}

void stepper::setTargetIndex(int32_t v)
{
    std::unique_lock ul(lk);
    if(homing)
        return;
    target_index = v;
    pending = true;

    cv.notify_all();
}

void stepper::home()
{
    std::unique_lock ul(lk);
    homing = true;
    homing_retries = 0;
    pending = true;

    cv.notify_all();
}

void stepper::_resetPosition()
{
    moveAtVelocity(0);
    current_speed = index.count = last_index_count = current_index = target_index = 0;
    if(encoder)
        encoder->position = 0;
}

void stepper::resetPosition()
{
    std::unique_lock ul(lk);
    _resetPosition();
}

int32_t stepper::getPosition()
{
    if(encoder)
        return encoder->position;
    else
        return current_index;
}
void stepper::setMaxAccel(uint32_t rpmps)
{
    std::unique_lock ul(lk);
    max_accel = rpmps;
}

void stepper::setMaxRpm(uint32_t rpm)
{
    std::unique_lock ul(lk);
    max_rpm = rpm;
}

bool stepper::do_homing(std::unique_lock<std::mutex> &ul, stepper::time_point &t)
{
    if(!limit)
        return true;
    const auto v = (*limit)();
    if(v == 0)
    {
        _resetPosition();
        if(++homing_retries >= 400)
        {
            homing = false;
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return false;
    }
    
    
    if(abs(v) > 250)
        target_index = (v/abs(v))*30;
    else
        target_index = (v/abs(v));
    
    index.count = last_index_count = current_index = 0;
    if(encoder)
        encoder->position = 0;
    do_target_approach(ul,t);
    
    return false;
}

bool stepper::do_target_approach(std::unique_lock<std::mutex> &ul, stepper::time_point &t)
{
    const auto t0 = now();
    const auto dt = t0-t;
    t = t0;

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
        const int32_t pos = encoder->position;
        const int32_t delta_i = pos - current_index;
        current_index = pos;
        
        const int32_t dist = ti-pos;
        if(abs(dist) > 0)
        {
            volatile float measured_velocity = 0.f;
            if(dt >= std::chrono::milliseconds(5))
            {
                const double dtsec = double(dt.count())*decltype(dt)::period::num/decltype(dt)::period::den;
                measured_velocity = float((double(delta_i)*256/10)/dtsec);
                //if(measured_velocity < current_speed/2)
                //    current_speed /= 2;
            }
            int sgn = (dist < 0) ? -1 : 1;
            const float suggested_velocity = abs(float(dist)*256/10/(current_speed/2)*mxacc);
            int32_t new_speed = (int32_t)(std::clamp(sgn*std::min(max_velocity,suggested_velocity), current_speed - mxacc/100, current_speed + mxacc/100));

            if(abs(dist) > 10)
            {
                current_speed = new_speed;
                moveAtVelocity(current_speed);
                cv.wait_until(ul, t0+std::chrono::milliseconds(10));
                return false;
            }
           
            if(!current_speed)
            {
                if(!new_speed)
                    current_speed = (int32_t)(float(dist)*256/10*100);
                else
                    current_speed = new_speed;
                moveAtVelocity(current_speed);
            }
            cv.wait_until(ul, t0+std::chrono::milliseconds(1));
            return false;
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

            int sgn = (dist < 0) ? -1 : 1;
            const int32_t new_speed = (int32_t)(std::clamp(sgn*std::min(max_velocity,suggested_velocity), current_speed - mxacc/100, current_speed + mxacc/100));
            current_speed = new_speed;

            if(abs(dist) > 1)
            {
                moveAtVelocity(current_speed);
                cv.wait_until(ul,t0+std::chrono::milliseconds(10));
            }

            else
            {
                if(!current_speed)
                {
                    current_speed = new_speed;
                    moveAtVelocity(current_speed);
                }
                cv.wait_until(ul,t0+std::chrono::milliseconds(1));
            }
            return false;
        }
    }
    return true;
}
    
void stepper::process_start()
{
    std::unique_lock ul(lk);
    index.enable();
    if(encoder)
        encoder->enable();
    moveAtVelocity(0);
    
    auto t = std::chrono::high_resolution_clock::now();
    while(true)
    {
        if(homing)
        {
            if(!do_homing(ul,t))
                continue;
        }
                    
        else
        {
            if(!do_target_approach(ul,t))
                continue;
        }
 
        current_speed = 0;
        moveAtVelocity(0);
        if(pending && departed)
        {
            //io::host::log("Arrived at position ",current_index); 
            departed = false;
            pending = false;
            finish_callback();
        }
        cv.wait_for(ul,std::chrono::milliseconds(20));
        if(pending)
        {
            departed = true;
            //io::host::log("Departing from position ",current_index); 
        }
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


extern "C" void stepper_index_notify(uint8_t index)
{ stepper_index[index].notify(); }
