#pragma once

#include <string>
#include <sstream>

namespace ssr::rtno2
{

    enum class COMMAND : char
    {
        INITIALIZE = 'I',
        ACTIVATE = 'A',
        DEACTIVATE = 'D',
        GET_STATE = 'X',

        GET_CONTEXT_TYPE = 'B',
        EXECUTE = 'E',
        RESET = 'R',

        ONERROR = 'C',
        GET_PROFILE = 'Z',
        INPORT_PROFILE = 'P',
        OUTPORT_PROFILE = 'Q',
        PLATFORM_PROFILE = 'T',

        SEND_DATA = 'S',
        RECEIVE_DATA = 'G',

        RECEIVE_LOG = 'L',

        PACKET_ERROR = 'F',
        PACKET_ERROR_CHECKSUM = 'H',
        PACKET_ERROR_TIMEOUT = 'K',
    };

    inline std::string command_to_string(COMMAND cmd)
    {
        switch (cmd)
        {
        case COMMAND::INITIALIZE:
            return "COMMAND::INITIALIZE";
        case COMMAND::ACTIVATE:
            return "COMMAND::ACTIVATE";
        case COMMAND::DEACTIVATE:
            return "COMMAND::DEACTIVATE";
        case COMMAND::GET_STATE:
            return "COMMAND::GET_STATE";
        case COMMAND::GET_CONTEXT_TYPE:
            return "COMMAND::GET_CONTEXT_TYPE";
        case COMMAND::EXECUTE:
            return "COMMAND::EXECUTE";
        case COMMAND::RESET:
            return "COMMAND::RESET";
        case COMMAND::ONERROR:
            return "COMMAND::ONERROR";
        case COMMAND::GET_PROFILE:
            return "COMMAND::GET_PROFILE";
        case COMMAND::INPORT_PROFILE:
            return "COMMAND::OUTPORT_PROFILE";
        case COMMAND::OUTPORT_PROFILE:
            return "COMMAND::OUTPORT_PROFILE";
        case COMMAND::SEND_DATA:
            return "COMMAND::SEND_DATA";
        case COMMAND::RECEIVE_DATA:
            return "COMMAND::RECEIVE_DATA";
        case COMMAND::PACKET_ERROR:
            return "COMMAND::PACKET_ERROR";
        case COMMAND::PACKET_ERROR_CHECKSUM:
            return "COMMAND::PACKET_ERROR_CHECKSUM";
        case COMMAND::PACKET_ERROR_TIMEOUT:
            return "COMMAND::PACKET_ERROR_TIMEOUT";
        case COMMAND::RECEIVE_LOG:
            return "COMMAND::RECEIVE_LOG";
        default:
        {
            std::stringstream ss;
            ss << "COMMAND::UNKNOWN(" << (char)cmd << "/" << (int)cmd << ")";
            return ss.str();
        }
        }
    }
}