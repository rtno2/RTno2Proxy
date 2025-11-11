#include <iostream>

#include <thread>
#include <chrono>

#include "hal/Socket.h"
#include "hal/ServerSocket.h"
#include "hal/Serial.h"

int main(const int argc, const char *argv[])
{
    try
    {
        int port_number = 10023;
        const std::string filename = argv[1];
        const int baudrate = atoi(argv[2]);

        ssr::ServerSocket ss;
        ss.bind(port_number);
        ss.listen();
        while (true)
        {
            std::cout << "[com2tcp] waiting..." << std::endl;
            ssr::Socket socket = ss.accept();

            auto pid = fork();
            if (pid == 0)
            {

                if (socket.setNonBlock(true) < 0)
                {
                    std::cerr << "[com2tcp] setNonBlock failed" << std::endl;
                    break;
                }
                std::cout << "[com2tcp] connected." << std::endl;
                ssr::Serial s(filename.c_str(), baudrate);
                s.flushRxBuffer();
                std::cout << "[com2tcp] serial port opened : " << filename << " " << baudrate << std::endl;

                const size_t MAX_BUFFER = 1024;
                uint8_t buffer[MAX_BUFFER];
                memset(buffer, 0, MAX_BUFFER);

                while (true)
                {
                    // std::cout << "while loop start" << std::endl;
                    int received_size;
                    if ((received_size = s.getSizeInRxBuffer()) < 0)
                    {
                        // Error failed.
                        std::cout << "[com2tcp] getSizeInRxBuffer failed." << std::endl;
                        break;
                    }
                    else if (received_size == 0)
                    {
                        // std::cout << "[com2tcp] No data received" << std::endl;
                    }
                    else // if (received_size > 0)
                    {
                        std::cout << "[com2tcp] sending data size: " << received_size << std::endl;
                        if (received_size > MAX_BUFFER)
                        {
                            received_size = MAX_BUFFER;
                        }
                        if (s.read(buffer, received_size) < 0)
                        {
                            std::cout << "[com2tcp] read failed." << std::endl;
                            break;
                        }

                        if (socket.write(buffer, received_size) < 0)
                        {
                            std::cout << "[com2tcp] send failed." << std::endl;
                            break;
                        }
                    }

                    // std::cout << "[com2tcp] check socket receive" << std::endl;
                    if ((received_size = socket.getSizeInRxBuffer()) < 0)
                    {
                        // Error
                        std::cerr << "[com2tcp] getSizeInRxBuffer failed." << std::endl;
                        break;
                    }
                    else if (received_size == 0)
                    {
                        // std::cout << "[com2tcp] No data received from socket." << std::endl;
                    }
                    else // if (received_size > 0)
                    {
                        std::cout << "receiving data size: " << received_size << std::endl;
                        if ((received_size = socket.read(buffer, received_size)) < 0)
                        {
                            std::cout << "[com2tcp] recv failed (received_size=" << received_size << ")" << std::endl;
                            continue;
                        }
                        if (received_size > 0)
                        {
                            if (s.write(buffer, received_size) < 0)
                            {
                                std::cout << "[com2tcp] write failed." << std::endl;
                                break;
                            }
                        }
                    }
                    // std::cout << "[com2tcp] sleep 1 microsecond" << std::endl;
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                } // while
            } // if pid==0
            else if (pid > 0)
            {
                std::cout << "[com2tcp] forked child process pid=" << pid << std::endl;
                continue;
            }
            else
            {
                std::cerr << "[com2tcp] fork failed." << std::endl;
            }
            std::cout << "[com2tcp] disconnected." << std::endl;
        }
        std::cout << "[com2tcp] server socket closed." << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cout << "[com2tcp] Exception: " << e.what() << std::endl;
        return -1;
    }
}