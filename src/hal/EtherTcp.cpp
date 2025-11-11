#include "EtherTcp.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <queue>

#define BUFSIZE 1024
uint8_t buffer[BUFSIZE];
int32_t buffer_start = 0;
int32_t buffer_end = 0;

std::queue<uint8_t> buf_queue;

std::mutex buffer_mutex;

void buffer_push(uint8_t dat)
{
	std::lock_guard<std::mutex> lock(buffer_mutex);
	// buffer[buffer_end] = dat;
	// buffer_end++;
	// if (buffer_end >= BUFSIZE)
	// {
	// 	buffer_end = 0;
	// }
	buf_queue.push(dat);
}

void buffer_push(uint8_t *data, size_t size)
{
	// std::lock_guard<std::mutex> lock(buffer_mutex);
	for (size_t i = 0; i < size; i++)
	{
		buffer_push(data[i]);
	}
}

uint8_t buffer_pop()
{
	std::lock_guard<std::mutex> lock(buffer_mutex);
	// uint8_t dat = buffer[buffer_start];
	// buffer_start++;
	// if (buffer_start >= BUFSIZE)
	// {
	// 	buffer_start = 0;
	// }
	// return dat;
	uint8_t dat = buf_queue.front();
	buf_queue.pop();
	return dat;
}

void buffer_clear()
{
	std::lock_guard<std::mutex> lock(buffer_mutex);
	// buffer_start = 0;
	// buffer_end = 0;
	while (!buf_queue.empty())
	{
		buf_queue.pop();
	}
}

int32_t get_buffer_size()
{
	std::lock_guard<std::mutex> lock(buffer_mutex);
	// int32_t size = buffer_end - buffer_start;
	// if (size < 0)
	// {
	// 	size += BUFSIZE;
	// }
	// return size;
	return buf_queue.size();
}

using namespace ssr;

EtherTcp::EtherTcp(const char *ipAddress, int32_t port)
{
	m_pSocket = new ssr::Socket(ipAddress, port);
	m_thread = std::thread([this]
						   { this->svc(); });
}

EtherTcp::~EtherTcp()
{
	std::cout << "EtherTcp::~EtherTcp() called" << std::endl;
	m_Endflag = 1;
	m_thread.join();
	delete m_pSocket;
}

void EtherTcp::flushRxBuffer()
{
	// Clear the receive buffer
	buffer_clear();
}

/**
 * @brief flush transmit buffer.
 * @return zero if success.
 */
void EtherTcp::flushTxBuffer()
{
}

/**
 * @brief Get stored datasize of in Rx Buffer
 * @return Stored Data Size of Rx Buffer;
 */
RETVAL EtherTcp::getSizeInRxBuffer()
{
	/*
		fd_set read_fdset;
		FD_ZERO(&read_fdset);
		FD_SET(m_ServerSocket, &read_fdset);

		select(0, &read_fdset, NULL, NULL, NULL);

		if(FD_ISSET(m_ServerSocket, &read_fdset)) {
			uint8_t buf[BUFSIZE];
			RETVAL readable_size = BUFSIZE - get_buffer_size();
			RETVAL read_size = recv(m_ServerSocket, (char*)buf, readable_size, 0);
			for(RETVAL i = 0;i < read_size;i++) {
				buffer_push(buf[i]);
			}
		}

		return get_buffer_size();;
	*/
	//	return 256;

	return ::get_buffer_size();
}

/**
 * @brief write data to Tx Buffer of Serial Port.
 *
 */
RETVAL EtherTcp::write(const uint8_t *src, const uint8_t size)
{
	int retval = m_pSocket->write(src, size);
	if (retval < 0)
	{
		std::cerr << "[EtherTcp] write error." << std::endl;
		perror("EtherTcp::write error");
	}
	return retval;
}

/**
 * @brief read data from RxBuffer of Serial Port
 */
RETVAL EtherTcp::read(uint8_t *dst, const uint8_t size)
{
	// return recv(m_ServerSocket, (char*)dst, size, 0);
	uint8_t *dist_buf = (uint8_t *)dst;
	uint32_t size_read = size;
	if (size >= (uint32_t)get_buffer_size())
	{
		size_read = get_buffer_size();
	}
	for (uint32_t i = 0; i < size_read; i++)
	{
		dist_buf[i] = buffer_pop();
	}

	return size_read;
}

RETVAL EtherTcp::svc(void)
{
	m_Endflag = 0;
	m_pSocket->setNonBlock(1);
	while (!m_Endflag)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		uint8_t buf;
		const int BUFFER_SIZE = 1024;
		uint8_t buffer[BUFFER_SIZE];
		int32_t receive_size;
		if ((receive_size = m_pSocket->getSizeInRxBuffer()) > 0)
		{
			int32_t sz = m_pSocket->read(buffer, receive_size > BUFFER_SIZE ? BUFFER_SIZE : receive_size);
			if (sz > 0)
			{
				// buffer_push(buffer[0]);
				buffer_push(buffer, sz);
			}
			else if (sz < 0)
			{
				// Error
				// std::cerr << "[EtherTcp] read error." << std::endl;
				// perror("EtherTcp::svc read error");
				// exit(-1);
			}
		}
	}
	return 0;
}

RETVAL EtherTcp::getSenderInfo(uint8_t *buffer)
{
	buffer[0] = 'E', buffer[1] = 'T', buffer[2] = 'C', buffer[3] = 'P';
	return 0;
}
