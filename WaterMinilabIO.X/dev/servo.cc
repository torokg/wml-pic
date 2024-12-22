#include "servo.hh"
#include <functional>
namespace dev
{
    servo::servo(oc_type oc, float lob, float hib)
        : _(oc)
        , lob(lob)
        , hib(hib)
        , depart_time(now())
        , target_time(time_point::max())
        , current_angle(0)
        , target_angle(0)
        , departed(false)
        , pending(false)
        , process(new std::thread(std::bind(&servo::process_start,this)))
    { setAngle(0,std::chrono::milliseconds(0)); }
    
    
    void servo::update_angle(float next_angle)
    {
        float angle = std::clamp(next_angle,-1.f,1.f)+1.f;
            angle /= 2;
            angle *= hib-lob;
            angle += lob;
            _.set((uint16_t)(angle*_.period));
        current_angle = next_angle;
    }
    
    bool servo::do_target_approach(std::unique_lock<std::mutex> &ul, servo::time_point &t)
    {
        const auto t0 = now();
        const auto dt = t0-t;
        t = t0;
        const auto rt = (target_time-t0);
        if(rt <= std::chrono::milliseconds(0))
        {
            update_angle(target_angle);
            return true;
        }
        else
        {
            float next_angle = current_angle;
            next_angle += (target_angle - next_angle) *
                std::chrono::duration_cast<std::chrono::milliseconds>(dt).count() /
                std::chrono::duration_cast<std::chrono::milliseconds>(rt).count();
            
            update_angle(next_angle);
        }
        cv.wait_for(ul,std::chrono::milliseconds(10));
        return false;
    }
        
    void servo::process_start()
    {
        std::unique_lock ul(lk);
        auto t = now();
        while(true)
        {
            if(!do_target_approach(ul,t))
                continue;
            
            if(pending && departed)
            {
                //io::host::log("Arrived at position ",current_index); 
                departed = false;
                pending = false;
                //finish_callback();
            }
            cv.wait_for(ul,std::chrono::milliseconds(10));
            if(pending)
            {
                departed = true;
                //io::host::log("Departing from position ",current_index); 
            }
            
        }
        
    }
}
