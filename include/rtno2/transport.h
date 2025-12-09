#pragma once

#include <stdint.h>
#include "packet.h"
#include "result.h"
#include "hal/SerialDevice.h"
#include "logger.h"
#include <string>
#include <exception>


#ifndef RTNO2API
#ifdef WIN32
#ifdef _WINDLL
#ifdef rtno_proxy_EXPORTS
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

namespace ssr::rtno2
{

	class RTNO2API transport_t
	{
	private:
	protected:
		SerialDevice *serial_device_;
		uint8_t sender_info_[PACKET_SENDER_INFO_LENGTH];
		logger_t logger_;

	public:
		transport_t(SerialDevice *pSerialDevice, ssr::rtno2::LOGLEVEL loglevel);
		~transport_t(void);

	public:
		RESULT send(const packet_t &packet);
		result_t<packet_t> receive(const uint32_t wait_usec);

		RESULT is_new(const uint32_t wait_usec = 1000 * 1000);

	private:
		RESULT read(uint8_t *buffer, uint8_t size, const uint32_t wait_usec = 1000 * 1000);
		RESULT write(const uint8_t *buffer, const uint8_t size);

	public:
		void clear_rx_buffer()
		{
			serial_device_->flushRxBuffer();
		}
	};
};
