#ifndef DEV_SERVO_HH
# define DEV_SERVO_HH
# include <algorithm>
# include <cstdint>
# include <cstddef>
# include <thread>
# include <mutex>
# include <condition_variable>
# include <drivers/oc.hh>
namespace dev
{

    
class servo
    : public std::chrono::high_resolution_clock

{
    oc_type _;
    const float lob;
    const float hib;
    
    time_point depart_time;
    time_point target_time;
    
    volatile float current_angle;
    volatile float target_angle;
    
    volatile bool departed;
    volatile bool pending;
    
    std::mutex lk;
    std::condition_variable cv;
    std::thread *process;
    
    void process_start();
    
    bool do_target_approach(std::unique_lock<std::mutex> &ul, time_point &t);
    void update_angle(float angle);
    
public:
    
    servo(oc_type oc, float lob, float hib);

    template<typename Rep, typename Ratio>
    inline void setAngle(float angle, std::chrono::duration<Rep,Ratio> dur)
    {
        const auto t0 = now();
        std::unique_lock ul(lk);
        target_angle = angle;
        target_time = t0+dur;
        pending = true;

        cv.notify_all();
    }
       
};

}
#endif
