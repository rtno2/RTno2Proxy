#pragma once

#include <string>
#include <sstream>

namespace ssr::rtno2
{

    enum class TYPECODE : char
    {
        TIMED_BOOLEAN = 'b',
        TIMED_OCTET = 'o',
        TIMED_CHAR = 'c',
        TIMED_LONG = 'l',
        TIMED_FLOAT = 'f',
        TIMED_DOUBLE = 'd',

        TIMED_BOOLEAN_SEQ = 'B',
        TIMED_OCTET_SEQ = 'O',
        TIMED_CHAR_SEQ = 'C',

        TIMED_LONG_SEQ = 'L',
        TIMED_FLOAT_SEQ = 'F',
        TIMED_DOUBLE_SEQ = 'D',
    };

    inline std::string typecode_to_str(TYPECODE typecode)
    {
        switch (typecode)
        {
        case TYPECODE::TIMED_BOOLEAN:
            return "TimedBoolean";
        case TYPECODE::TIMED_CHAR:
            return "TimedChar";
        case TYPECODE::TIMED_OCTET:
            return "TimedOctet";
        case TYPECODE::TIMED_LONG:
            return "TimedLong";
        case TYPECODE::TIMED_FLOAT:
            return "TimedFloat";
        case TYPECODE::TIMED_DOUBLE:
            return "TimedDouble";

        case TYPECODE::TIMED_BOOLEAN_SEQ:
            return "TimedBooleanSeq";
        case TYPECODE::TIMED_CHAR_SEQ:
            return "TimedCharSeq";
        case TYPECODE::TIMED_OCTET_SEQ:
            return "TimedOctetSeq";

        case TYPECODE::TIMED_LONG_SEQ:
            return "TimedLongSeq";
        case TYPECODE::TIMED_FLOAT_SEQ:
            return "TimedFloatSeq";
        case TYPECODE::TIMED_DOUBLE_SEQ:
            return "TimedDoubleSeq";
        default:
            return "UnknownTypeCode";
        }
    }
}