#ifndef RESULT_HEADER_FILE_INCLUDED
#define RESULT_HEADER_FILE_INCLUDED

#include <optional>
#include <string>
#include <sstream>

namespace ssr::rtno2
{

  enum class RESULT : uint8_t
  {
    OK = '@',
    ERR = 'x',
    NONE = '!',
    TIMEOUT = 1,
    DATA_TIMEOUT = 2,
    CHECKSUM_ERROR = 3,
    NOT_AVAILABLE = 4,
    INVALID_PRESTATE = 'H',
    INPORT_NOT_FOUND = 'F',
    OUTPORT_NOT_FOUND = 'D',
    OUTPORT_BUFFER_EMPTY = 'I',

    PACKET_START_TIMEOUT = 'S',
    PACKET_HEADER_TIMEOUT = 'T',
    PACKET_BODY_TIMEOUT = 'U',
    PACKET_CHECKSUM_TIMEOUT = 'C',

    LOG_DATA_EXCEED_SIZE = 'X',
    UNINITIALIZED = '_',
  };

  inline const std::string result_to_string(RESULT r)
  {
    switch (r)
    {
    case RESULT::OK:
      return "RESULT::OK";
    case RESULT::ERR:
      return "RESULT::ERROR";
    case RESULT::NONE:
      return "RESULT::NONE";
    case RESULT::TIMEOUT:
      return "RESULT::TIMEOUT";
    case RESULT::DATA_TIMEOUT:
      return "RESULT::DATA_TIMEOUT";
    case RESULT::CHECKSUM_ERROR:
      return "RESULT::CHECKSUM_ERROR";
    case RESULT::NOT_AVAILABLE:
      return "RESULT::NOT_AVAILABLE";
    case RESULT::INVALID_PRESTATE:
      return "RESULT::INVALID_PRESTATE";
    case RESULT::INPORT_NOT_FOUND:
      return "RESULT::INPORT_NOT_FOUND";
    case RESULT::OUTPORT_NOT_FOUND:
      return "RESULT::OUTPORT_NOT_FOUND";
    case RESULT::OUTPORT_BUFFER_EMPTY:
      return "RESULT::OUTPORT_BUFFER_EMPTY";
    case RESULT::PACKET_START_TIMEOUT:
      return "RESULT::PACKET_START_TIMEOUT";
    case RESULT::PACKET_HEADER_TIMEOUT:
      return "RESULT::PACKET_BODY_TIMEOUT";
    case RESULT::PACKET_BODY_TIMEOUT:
      return "RESULT::PACKET_BODY_TIMEOUT";
    case RESULT::LOG_DATA_EXCEED_SIZE:
      return "RESULT::LOG_DATA_EXCEED_SIZE";
    case RESULT::UNINITIALIZED:
      return "RESULT::UNINITIALIZED";
    default:
    {
      std::stringstream ss;
      ss << "RESULT::UNKNOWN(" << (char)r << "/" << (int)r << ")";
      return ss.str();
    }
    }
  }

  template <typename T>
  class result_t
  {
  public:
    std::optional<T> value;
    RESULT result;

  public:
    result_t() : result(RESULT::UNINITIALIZED) {}
    result_t(T &&t) : value(t), result(RESULT::OK) {}
    result_t(RESULT r) : value(std::nullopt), result(r) {}
  };

}

#endif