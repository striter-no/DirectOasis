#pragma once
#include <chrono>

namespace extra{

    typedef std::chrono::_V2::steady_clock::time_point chrono_tpoint_t;

    chrono_tpoint_t getChronoTimeNow(){
        return std::chrono::steady_clock::now();
    }

    template<class durationT = std::chrono::milliseconds>
    float getChronoElapsed(chrono_tpoint_t begin){
        return std::chrono::duration_cast<durationT>(getChronoTimeNow() - begin).count();
    }
}