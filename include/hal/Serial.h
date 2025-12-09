#pragma once

#include "SerialDevice.h"
#include "SerialPort.h"


#ifndef RTNO2API
#ifdef WIN32
#ifdef _WINDLL
#ifdef rtno_hal_EXPORTS
#define RTNO2API __declspec(dllexport)
#else
#define RTNO2API __declspec(dllimport)
#endif
#else
#define RTNO2API
#endif
#else
#define RTNO2API
#endif
#endif

namespace ssr
{
      class RTNO2API Serial : public SerialDevice
      {
      private:
            net::ysuga::SerialPort *m_pSerialPort;

      public:
            Serial(const char *portName, const int boardrate);
            virtual ~Serial(void);

      public:
            /**
             * @brief flush receive buffer.
             * @return zero if success.
             */
            virtual void flushRxBuffer()
            {
                  m_pSerialPort->flushRxBuffer();
            }

            /**
             * @brief flush transmit buffer.
             * @return zero if success.
             */
            virtual void flushTxBuffer()
            {
                  m_pSerialPort->flushTxBuffer();
            }

      public:
            /**
             * @brief Get stored datasize of in Rx Buffer
             * @return Stored Data Size of Rx Buffer;
             */
            virtual RETVAL getSizeInRxBuffer()
            {
                  return m_pSerialPort->getSizeInRxBuffer();
            }

            /**
             * @brief write data to Tx Buffer of Serial Port.
             *
             */
            virtual RETVAL write(const uint8_t *src, const uint8_t size)
            {
                  return m_pSerialPort->write(src, size);
            }

            /**
             * @brief read data from RxBuffer of Serial Port
             */
            virtual RETVAL read(uint8_t *dst, const uint8_t size)
            {
                  return m_pSerialPort->read(dst, size);
            }

            virtual RETVAL getSenderInfo(uint8_t *buffer)
            {
                  buffer[0] = 'U', buffer[1] = 'A', buffer[2] = 'R', buffer[3] = 'T';
                  return 0;
            }
      };

}
