#pragma once

#include <string>
#include <sstream>

namespace ssr::rtno2
{

    enum class EC_TYPE : char
    {

        MAINLOOP = 1,
        PROXY_SYNCHRONOUS = 2,
        FSP_TIMER = 3,
        TIMER_ONE = 4,
        // TIMER1 = 5,
        UNKNOWN = 127,

        // PROXY_SYNCHRONOUS = 0x21,
        // TIMER_ONE = 0x22,
        // TIMER_TWO = 0x23,
        // FSP_TIMER = 0x24,
    };

    inline std::string ec_type_to_string(EC_TYPE ec_type)
    {
        switch (ec_type)
        {
        case EC_TYPE::MAINLOOP:
            return "MainLoopExecutionContext";
        case EC_TYPE::PROXY_SYNCHRONOUS:
            return "ProxySynchronousExecutionContext";
        case EC_TYPE::TIMER_ONE:
            return "Timer1ExecutionContext";
        // case EC_TYPE::TIMER_TWO:
        //     return "Timer2ExecutionContext";
        case EC_TYPE::FSP_TIMER:
            return "FSPTimerExecutionContext";
        default:
        {
            std::stringstream ss;
            ss << "UnknownExecutionContext(" << (int)ec_type << ")";
            return ss.str();
        }
        }
    }
}