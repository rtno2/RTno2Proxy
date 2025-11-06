

#pragma once

#include <cstdint>
#include <string.h> // memcpy
#include <sstream>

#include "command.h"
#include "result.h"

#define PACKET_HEADER_SIZE 3
#define PACKET_RECEIVE_HEADER_SIZE 3
#define PACKET_SENDER_INFO_LENGTH 4

namespace ssr::rtno2
{

  class packet_t
  {
  private:
    uint8_t *m_pData;

  public:
    packet_t(const COMMAND command, const RESULT result, const uint8_t *data = NULL, const uint8_t size = 0) : m_pData(NULL)
    {
      initialize(command, result, data, size);
    }

    packet_t(const packet_t &p) : m_pData(NULL)
    {
      initialize(p.get_command(), p.get_result(), p.getData(), p.getDataLength());
    }

    ~packet_t()
    {
      delete m_pData;
    }

  public:
    void operator=(const packet_t &p)
    {
      initialize(p.get_command(), p.get_result(), p.getData(), p.getDataLength());
    }

  public:
    COMMAND get_command() const { return (COMMAND)m_pData[0]; }
    RESULT get_result() const { return (RESULT)m_pData[1]; }
    uint8_t getDataLength() const { return m_pData[2]; }
    // uint8_t *getSenderInfo() {return m_pData + PACKET_HEADER_SIZE;}
    uint8_t getPacketLength() const { return PACKET_HEADER_SIZE + m_pData[2]; }
    const uint8_t *getData() const { return m_pData + PACKET_HEADER_SIZE; }
    const uint8_t *serialize() const { return m_pData; }
    uint8_t getSum() const
    {
      uint8_t sum = 0;
      for (uint32_t i = 0; i < getPacketLength(); i++)
        sum += m_pData[i];
      return sum;
    }

  private:
    void initialize(const COMMAND command, const RESULT result, const uint8_t *data = NULL, const uint8_t size = 0)
    {
      delete m_pData;
      m_pData = new uint8_t[size + PACKET_HEADER_SIZE];
      m_pData[0] = (uint8_t)command;
      m_pData[1] = (uint8_t)result;
      m_pData[2] = size;
      if (size > 0)
      {
        memcpy(m_pData + PACKET_HEADER_SIZE, data, size);
      }
    }

  public:
    std::string to_string() const
    {
      std::stringstream ss;
      ss << "packet_t(CMD=" << command_to_string(get_command()) << ",RES=" << result_to_string(get_result()) << ",DATASIZE=" << (int)this->getDataLength() << ")";
      return ss.str();
    }
  };
}
