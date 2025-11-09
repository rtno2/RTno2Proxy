
#include "hal/Serial.h"
#include "hal/EtherTcp.h"
#include "rtno2/protocol.h"
#include "rtno2/logger.h"

#include "test_function.h"
#include <iostream>

using namespace ssr::rtno2;

/**
 * テスト関数の本体
 *
 *
 */
void do_test(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol)
{

    conduct_send_and_recv_test<bool>("Bool data test. Send true then recv it", logger, protocol, "bool_in", "bool_out", false, true);
    conduct_send_and_recv_test<bool>("Bool negative data test. Send false then recv it", logger, protocol, "bool_in", "bool_out", false, false);
    conduct_send_and_recv_test<char>("Character data test. Send 'c' then recv it", logger, protocol, "char_in", "char_out", 'a', 'c');
    conduct_send_and_recv_test<uint8_t>("Octet data test. Send 0x32 then recv it", logger, protocol, "octet_in", "octet_out", 0x01, 0x32);

    conduct_send_and_recv_test<int32_t>("Long positive data test. Send 3 then recv it", logger, protocol, "long_in", "long_out", 1, 3);
    conduct_send_and_recv_test<int32_t>("Long negative data test. Send -3 then recv it", logger, protocol, "long_in", "long_out", 1, -3);
    conduct_send_and_recv_test<float>("Float positive data test. Send 3.0f then recv it", logger, protocol, "float_in", "float_out", 1.0f, 3.0f);
    conduct_send_and_recv_test<double>("Double positive data test. Send 3.0 then recv it", logger, protocol, "double_in", "double_out", 1.0, 3.0);
    /*
    conduct_seq_send_and_recv_test<int32_t>("LongSeq positive single element vector test. Send {3} then recv it", logger, protocol, "long_seq_in", "long_seq_out", {1}, {3});
    conduct_seq_send_and_recv_test<int32_t>("LongSeq positive vector test. Send {3, 2, 1} then recv it", logger, protocol, "long_seq_in", "long_seq_out", {1}, {3, 2, 1});
    conduct_seq_send_and_recv_test<float>("FloatSeq positive single element vector test. Send {3.0f} then recv it", logger, protocol, "float_seq_in", "float_seq_out", {1.0f}, {3.0f});
    conduct_seq_send_and_recv_test<float>("FloatSeq positive vector test. Send {3.0f, 2.0f, 1.0f} then recv it", logger, protocol, "float_seq_in", "float_seq_out", {1.0f}, {3.0f, 2.0f, 1.0f});
    conduct_seq_send_and_recv_test<double>("DoubleSeq positive single element vector test. Send {3.0} then recv it", logger, protocol, "double_seq_in", "double_seq_out", {1.0}, {3.0});
    conduct_seq_send_and_recv_test<double>("DoubleSeq positive vector test. Send {3.0, 2.0, 1.0} then recv it", logger, protocol, "double_seq_in", "double_seq_out", {1.0}, {3.0, 2.0, 1.0});

    conduct_seq_send_and_recv_test<bool>("BoolSeq data test. Send true then recv it", logger, protocol, "bool_seq_in", "bool_seq_out", {false}, {true, true});
    conduct_seq_send_and_recv_test<bool>("BoolSeq negative data test. Send false then recv it", logger, protocol, "bool_seq_in", "bool_seq_out", {false}, {false, false});
    conduct_seq_send_and_recv_test<char>("CharacterSeq data test. Send 'c' then recv it", logger, protocol, "char_seq_in", "char_seq_out", {'a'}, {'a', 'b', 'c'});
    conduct_seq_send_and_recv_test<uint8_t>("OctetSeq data test. Send 0x32 then recv it", logger, protocol, "octet_seq_in", "octet_seq_out", {0x01}, {0x32, 0x33, 0x34});
    */
}

int do_whole_interactive(logger_t &logger)
{
    ssr::SerialDevice *serial_port = NULL;
    protocol_t *rtno = NULL;
    while (true)
    {
        if (rtno)
        {
            do_test(logger, *rtno);
            return 0;
        }
        else
        {
            std::cout << "[not opened]> " << std::ends;
        }
        std::string commandline;
        if (!std::getline(std::cin, commandline))
        {
            return 0; // Ctrl+D
        }
        auto elems = strsplit(commandline, ' ');
        if (elems.size() == 0)
        {
            continue;
        }
        if (elems[0] == "open")
        {
            if (elems.size() < 3)
            {
                std::cout << "ERROR. open need argument. type 'help' to get more info." << std::endl;
                continue;
            }
            std::string filename = elems[1];
            int baudrate = atoi(elems[2].c_str());
            serial_port = create_serial(filename.c_str(), baudrate);
            if (serial_port)
            {
                rtno = new protocol_t(serial_port);
            }
        }
        else if (elems[0] == "close")
        {
            delete rtno;
            rtno = nullptr;
            delete serial_port;
            serial_port = nullptr;
        }
        else if (elems[0] == "exit")
        {
            delete rtno;
            delete serial_port;
            return 0;
        }
        else
        {
        }
    }
}

int main(const int argc, const char *argv[])
{
    // logger = ssr::getLogger("main");

    ssr::rtno2::logger_t logger(get_logger("main"));
    set_log_level(&logger, LOGLEVEL::INFO);

    if (argc == 1)
    {
        // Whole interactive mode
        return do_whole_interactive(logger);
    }

    std::vector<std::string> args;
    for (int i = 3; i < argc; i++)
    {
        std::string arg = argv[i];
        args.push_back(arg);
    }
    auto filename = argv[1];
    int baudrate = atoi(argv[2]);

    auto serial_port = create_serial(filename, baudrate);
    if (!serial_port)
    {
        return -1;
    }
    auto protocol = new protocol_t(serial_port, ssr::rtno2::LOGLEVEL::WARN,
                                   ssr::rtno2::LOGLEVEL::WARN);
    do_test(logger, *protocol);
    delete protocol;
    delete serial_port;
    return 0;
}