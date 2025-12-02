#include <iostream>

#include <thread>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "hal/Socket.h"
#include "hal/ServerSocket.h"
#include "hal/Serial.h"

int main(const int argc, const char *argv[])
{

    using logger_t = spdlog::logger;
    spdlog::set_level(spdlog::level::info);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("com2tcp_log.txt", true);
    file_sink->set_level(spdlog::level::trace);

    std::vector<std::shared_ptr<spdlog::sinks::sink>> sink_list = {file_sink, console_sink};

    auto logger = spdlog::logger("com2tcp", sink_list.begin(), sink_list.end());
    logger.info("com2tcp started.");
    try
    {
        const std::string filename = argv[1];
        const int baudrate = atoi(argv[2]);
        const int port_number = (argc >= 4) ? atoi(argv[3]) : 10000;

        ssr::ServerSocket ss;
        ss.bind(port_number);
        ss.listen();
        while (true)
        {
            logger.info(" - com2tcp listening on port {}", port_number);
            ssr::Socket socket = ss.accept();

            auto pid = fork();
            if (pid == 0)
            {
                logger.info(" - com2tcp connected.");

                if (socket.setNonBlock(true) < 0)
                {
                    logger.error(" - com2tcp setNonBlock failed");
                    break;
                }
                logger.info(" - com2tcp connected.");
                ssr::Serial s(filename.c_str(), baudrate);
                s.flushRxBuffer();
                logger.info(" - com2tcp serial port opened : {} {}", filename, baudrate);

                const size_t MAX_BUFFER = 1024;
                uint8_t buffer[MAX_BUFFER];
                memset(buffer, 0, MAX_BUFFER);

                while (true)
                {
                    // std::cout << "while loop start" << std::endl;
                    logger.info(" - com2tcp check serial receive");
                    int received_size;
                    if ((received_size = s.getSizeInRxBuffer()) < 0)
                    {
                        // Error failed.
                        logger.error(" - com2tcp getSizeInRxBuffer failed.");
                        break;
                    }
                    else if (received_size == 0)
                    {
                        logger.info(" - com2tcp No data received from serial.");
                    }
                    else // if (received_size > 0)
                    {
                        logger.debug(" - com2tcp sending data size: {} to socket", received_size);
                        if (received_size > MAX_BUFFER)
                        {
                            received_size = MAX_BUFFER;
                        }
                        if (s.read(buffer, received_size) < 0)
                        {
                            logger.error(" - com2tcp read failed.");
                            break;
                        }
                        logger.debug(" - com2tcp sending data to socket");
                        // Write function always blocks when peer is shutdown.
                        // Use thread to avoid blocking main loop.

                        if (socket.write(buffer, received_size) < 0)
                        {
                            logger.error(" - com2tcp send failed.");
                            break;
                        }
                        perror("com2tcp send failed");
                    }

                    logger.info(" - com2tcp check socket receive");
                    if ((received_size = socket.getSizeInRxBuffer()) < 0)
                    {
                        // Error
                        logger.error(" - com2tcp getSizeInRxBuffer failed.");
                        break;
                    }
                    else if (received_size == 0)
                    {
                        logger.info(" - com2tcp No data received from socket.");
                    }
                    else // if (received_size > 0)
                    {
                        logger.debug(" - com2tcp receiving data size: {}", received_size);
                        if ((received_size = socket.read(buffer, received_size)) < 0)
                        {
                            logger.error(" - com2tcp recv failed (received_size={})", received_size);
                            continue;
                        }
                        if (received_size > 0)
                        {
                            if (s.write(buffer, received_size) < 0)
                            {
                                logger.error(" - com2tcp write failed.");
                                break;
                            }
                        }
                    }
                    logger.debug(" - com2tcp sleep 1 microsecond");
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                } // while
            } // if pid==0
            else if (pid > 0)
            {
                logger.info(" - com2tcp forked child process pid={}", pid);
                continue;
            }
            else
            {
                logger.error(" - com2tcp fork failed.");
            }
            logger.info(" - com2tcp disconnected.");
        }
        logger.info(" - com2tcp server socket closed.");
        return 0;
    }
    catch (const std::exception &e)
    {
        perror("com2tcp Exception");
        logger.error(" - com2tcp Exception: {}", e.what());
        return -1;
    }
}