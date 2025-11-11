#include "packet.h"
#include "result.h"
#include "transport.h"

#include <chrono>
#include <thread>
#include <iostream>

#include "definition.h"

#define PACKET_WAITING_TIME 3000 // ms
#define PACKET_WAITING_DELAY 100 // us
#define PACKET_WAITING_COUNT (PACKET_WAITING_TIME * 1000 / PACKET_WAITING_DELAY)

const uint32_t RTNO_INFINITE = 0xFFFFFFFF;

using namespace ssr::rtno2;

transport_t::transport_t(SerialDevice *pSerialDevice, ssr::rtno2::LOGLEVEL loglevel) : logger_(get_logger("Transport"))
{
	set_log_level(&logger_, loglevel);
	RTNO_TRACE(logger_, "Transport() called");
	serial_device_ = pSerialDevice;
}

transport_t::~transport_t(void)
{
}

RESULT transport_t::read(uint8_t *buffer, uint8_t size, uint32_t wait_usec)
{
	RTNO_TRACE(logger_, "transport_t::read(wait_usec={}) called", wait_usec);
	if (size == 0)
	{
		return RESULT::OK;
	}
	auto start_time = std::chrono::system_clock::now();
	while (1)
	{
		if (serial_device_->getSizeInRxBuffer() >= size)
		{
			break;
		}
		auto duration = std::chrono::duration<double>(std::chrono::system_clock::now() - start_time).count();
		if (duration * 1000000 > wait_usec && wait_usec != UINT32_MAX)
		{
			return RESULT::TIMEOUT;
		}
	}
	int read_size = serial_device_->read(buffer, size);
	if (read_size != size)
	{
		RTNO_ERROR(logger_, "transport_t::read() read size mismatch (expected={}, actual={})", size, read_size);
		return RESULT::ERR;
	}
	return RESULT::OK;
}

RESULT transport_t::write(const uint8_t *buffer, const uint8_t size)
{
	RTNO_TRACE(logger_, "Transport::write() called");

	for (uint8_t i = 0; i < size; i++)
	{
		serial_device_->write(buffer + i, 1);
		// std::this_thread::sleep_for(std::chrono::microseconds(PACKET_SENDING_DELAY));
	}
	return RESULT::OK;
}

RESULT transport_t::send(const packet_t &packet)
{
	RTNO_TRACE(logger_, "transport_t::send({}) called", packet.to_string());
	const uint8_t headers[2] = {0x0a, 0x0a};
	RESULT result;
	if ((result = write(headers, 2)) != RESULT::OK)
	{
		RTNO_ERROR(logger_, "transport_t::send() send startbytes failed {}", result_to_string(result));
		return result;
	}
	if ((result = write(packet.serialize(), packet.getPacketLength())) != RESULT::OK)
	{
		RTNO_ERROR(logger_, "transport_t::send() send body failed {}", result_to_string(result));
		return result;
	}
	uint8_t sum = packet.getSum();
	if ((result = write(&sum, 1)) != RESULT::OK)
	{
		RTNO_ERROR(logger_, "transport_t::send() send sum failed {}", result_to_string(result));
		return result;
	}
	RTNO_TRACE(logger_, "transport_t::send() exit");
	return RESULT::OK;
}

RESULT transport_t::is_new(const uint32_t wait_usec)
{
	RTNO_TRACE(logger_, "transport_t::is_new({}) called", wait_usec);
	uint8_t buf;
	RESULT result;
	while (1)
	{
		if ((result = this->read(&buf, 1, wait_usec)) != RESULT::OK)
		{
			if (result == RESULT::TIMEOUT)
			{
				RTNO_TRACE(logger_, "transport_t::is_new() exit with TIMEOUT");
				return RESULT::TIMEOUT;
			}
			RTNO_WARN(logger_, "transport_t::is_new() exit with Error {}", result_to_string(result));
			return result;
		}
		if (buf != 0x0a)
		{
			continue;
		}

		if ((result = this->read(&buf, 1, wait_usec)) != RESULT::OK)
		{
			if (result == RESULT::TIMEOUT)
			{
				RTNO_WARN(logger_, "In transport_t::is_new() reading 2nd starting packet failed. exit with TIMEOUT");
				return RESULT::TIMEOUT;
			}
			RTNO_WARN(logger_, "transport_t::is_new() exit with Error {}", result_to_string(result));
			return result;
		}
		if (buf == 0x0a)
		{
			break;
		}
	}

	RTNO_TRACE(logger_, "transport_t::is_new() exit with OK");
	return RESULT::OK;
}

result_t<packet_t> transport_t::receive(const uint32_t wait_usec /*=RTNO_INFINITE*/)
{
	RTNO_TRACE(logger_, "receive(wait_usec={}) called", wait_usec);
	uint8_t header[PACKET_RECEIVE_HEADER_SIZE]; // header[0] : COMMAND, header[1]: RESULT, header[2] : length
	RESULT result;
	if ((result = read(header, PACKET_RECEIVE_HEADER_SIZE, wait_usec)) != RESULT::OK)
	{
		RTNO_WARN(logger_, "receive() exit with PACKET_RECEIVE_HEADER_SIZE");
		if (result == RESULT::TIMEOUT)
		{
			RTNO_ERROR(logger_, "receive() exit with read header timeout");
			return RESULT::PACKET_HEADER_TIMEOUT;
		}
		return result;
	}

	uint8_t sender[4];
	serial_device_->getSenderInfo(sender);
	// if(read(sender, 4, wait_usec) < 0) {
	//     throw TimeOutException();
	//   }

	uint8_t data_buffer[256];
	if ((result = read(data_buffer, header[2], wait_usec)) != RESULT::OK)
	{
		RTNO_WARN(logger_, "receive() exit with read data timeout");
		if (result == RESULT::TIMEOUT)
		{
			RTNO_ERROR(logger_, "receive() exit with read data body timeout");
			return RESULT::PACKET_BODY_TIMEOUT;
		}
		return result;
	}

	packet_t packet((COMMAND)header[0], (RESULT)header[1], data_buffer, header[2]);
	uint8_t sum = packet.getSum();

	uint8_t buf;
	if ((result = read(&buf, 1, wait_usec)) != RESULT::OK)
	{
		if (result == RESULT::TIMEOUT)
		{
			RTNO_ERROR(logger_, "receive() exit with read checksum timeout");
			return RESULT::PACKET_CHECKSUM_TIMEOUT;
		}
		return result;
	}

	if (sum != buf)
	{
		RTNO_ERROR(logger_, "receive() exit with checksum error. CHECKSUM value of received packet ({}) and calculated one ({}) is different. data is {}", (int)buf, (int)sum, packet.to_string());
		return RESULT::CHECKSUM_ERROR;
	}

	RTNO_TRACE(logger_, "receive() exit");
	return packet;
}
