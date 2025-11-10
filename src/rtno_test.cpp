
#include "hal/Serial.h"
#include "hal/EtherTcp.h"
#include "rtno2/protocol.h"
#include "rtno2/logger.h"

#include "test_function.h"
#include <iostream>

using namespace ssr::rtno2;

const static ssr::rtno2::LOGLEVEL loglevel = ssr::rtno2::LOGLEVEL::WARN;
/**
 * テスト関数の本体
 *
 *
 */
void do_test(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol)
{
    auto prof_result = protocol.get_profile(200 * 1000, 15);
    if (prof_result.result != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "[TEST][FAILED] Getting profile failed. {}", ssr::rtno2::result_to_string(prof_result.result));
        return;
    }
    RTNO_INFO(logger, "[TEST] Profile: {}", prof_result.value.value().to_string());
    auto profile = prof_result.value.value();
    // 以降はポートが存在する場合にのみテストを実行する

    // Bool test
    auto bool_in_result = profile.inport("bool_in");
    auto bool_out_result = profile.outport("bool_out");
    if (bool_in_result.result == ssr::rtno2::RESULT::OK && bool_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_send_and_recv_test<bool>("Bool data test. Send true then recv it", logger, protocol, "bool_in", "bool_out", false, true);
        conduct_send_and_recv_test<bool>("Bool negative data test. Send false then recv it", logger, protocol, "bool_in", "bool_out", false, false);
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping Bool data test. bool_in or bool_out port not found. in({}), out({})", ssr::rtno2::result_to_string(bool_in_result.result), ssr::rtno2::result_to_string(bool_out_result.result));
    }

    // Char test
    auto char_in_result = profile.inport("char_in");
    auto char_out_result = profile.outport("char_out");
    if (char_in_result.result == ssr::rtno2::RESULT::OK && char_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_send_and_recv_test<char>("Character data test. Send 'c' then recv it", logger, protocol, "char_in", "char_out", 'a', 'c');
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping Character data test. char_in or char_out port not found. in({}), out({})", ssr::rtno2::result_to_string(char_in_result.result), ssr::rtno2::result_to_string(char_out_result.result));
    }

    // Octet test
    auto octet_in_result = profile.inport("octet_in");
    auto octet_out_result = profile.outport("octet_out");
    if (octet_in_result.result == ssr::rtno2::RESULT::OK && octet_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_send_and_recv_test<uint8_t>("Octet data test. Send 0x32 then recv it", logger, protocol, "octet_in", "octet_out", 0x01, 0x32);
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping Octet data test. octet_in or octet_out port not found. in({}), out({})", ssr::rtno2::result_to_string(octet_in_result.result), ssr::rtno2::result_to_string(octet_out_result.result));
    }

    // Long test
    auto long_in_result = profile.inport("long_in");
    auto long_out_result = profile.outport("long_out");
    if (long_in_result.result == ssr::rtno2::RESULT::OK && long_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_send_and_recv_test<int32_t>("Long positive data test. Send 3 then recv it", logger, protocol, "long_in", "long_out", 1, 3);
        conduct_send_and_recv_test<int32_t>("Long negative data test. Send -3 then recv it", logger, protocol, "long_in", "long_out", 1, -3);
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping Long data test. long_in or long_out port not found. in({}), out({})", ssr::rtno2::result_to_string(long_in_result.result), ssr::rtno2::result_to_string(long_out_result.result));
    }
    // Float test
    auto float_in_result = profile.inport("float_in");
    auto float_out_result = profile.outport("float_out");
    if (float_in_result.result == ssr::rtno2::RESULT::OK && float_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_send_and_recv_test<float>("Float positive data test. Send 3.0f then recv it", logger, protocol, "float_in", "float_out", 1.0f, 3.0f);
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping Float data test. float_in or float_out port not found. in({}), out({})", ssr::rtno2::result_to_string(float_in_result.result), ssr::rtno2::result_to_string(float_out_result.result));
    }
    // Double test
    auto double_in_result = profile.inport("double_in");
    auto double_out_result = profile.outport("double_out");
    if (double_in_result.result == ssr::rtno2::RESULT::OK && double_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_send_and_recv_test<double>("Double positive data test. Send 3.0 then recv it", logger, protocol, "double_in", "double_out", 1.0, 3.0);
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping Double data test. double_in or double_out port not found. in({}), out({})", ssr::rtno2::result_to_string(double_in_result.result), ssr::rtno2::result_to_string(double_out_result.result));
    }
    // BoolSeq test
    auto boolseq_in_result = profile.inport("bool_seq_in");
    auto boolseq_out_result = profile.outport("bool_seq_out");
    if (boolseq_in_result.result == ssr::rtno2::RESULT::OK && boolseq_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_seq_send_and_recv_test<bool>("BoolSeq data test. Send true then recv it", logger, protocol, "bool_seq_in", "bool_seq_out", {false}, {true, true});
        conduct_seq_send_and_recv_test<bool>("BoolSeq negative data test. Send false then recv it", logger, protocol, "bool_seq_in", "bool_seq_out", {false}, {false, false});
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping BoolSeq data test. bool_seq_in or bool_seq_out port not found. in({}), out({})", ssr::rtno2::result_to_string(boolseq_in_result.result), ssr::rtno2::result_to_string(boolseq_out_result.result));
    }
    // CharSeq test
    auto charseq_in_result = profile.inport("char_seq_in");
    auto charseq_out_result = profile.outport("char_seq_out");
    if (charseq_in_result.result == ssr::rtno2::RESULT::OK && charseq_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_seq_send_and_recv_test<char>("CharacterSeq data test. Send 'abc' then recv it", logger, protocol, "char_seq_in", "char_seq_out", {'a'}, {'a', 'b', 'c'});
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping CharacterSeq data test. char_seq_in or char_seq_out port not found. in({}), out({})", ssr::rtno2::result_to_string(charseq_in_result.result), ssr::rtno2::result_to_string(charseq_out_result.result));
    }
    // OctetSeq test
    auto octetseq_in_result = profile.inport("octet_seq_in");
    auto octetseq_out_result = profile.outport("octet_seq_out");
    if (octetseq_in_result.result == ssr::rtno2::RESULT::OK && octetseq_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_seq_send_and_recv_test<uint8_t>("OctetSeq data test. Send 0x32,0x33,0x34 then recv it", logger, protocol, "octet_seq_in", "octet_seq_out", {0x01}, {0x12, 0x13});
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping OctetSeq data test. octet_seq_in or octet_seq_out port not found. in({}), out({})", ssr::rtno2::result_to_string(octetseq_in_result.result), ssr::rtno2::result_to_string(octetseq_out_result.result));
    }
    // LongSeq test
    auto longseq_in_result = profile.inport("long_seq_in");
    auto longseq_out_result = profile.outport("long_seq_out");
    if (longseq_in_result.result == ssr::rtno2::RESULT::OK && longseq_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_seq_send_and_recv_test<int32_t>("LongSeq positive single element vector test. Send {3} then recv it", logger, protocol, "long_seq_in", "long_seq_out", {1}, {3});
        conduct_seq_send_and_recv_test<int32_t>("LongSeq positive vector test. Send {3, 2, 1} then recv it", logger, protocol, "long_seq_in", "long_seq_out", {1}, {3, 2, 1});
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping LongSeq data test. long_seq_in or long_seq_out port not found. in({}), out({})", ssr::rtno2::result_to_string(longseq_in_result.result), ssr::rtno2::result_to_string(longseq_out_result.result));
    }
    // FloatSeq test
    auto floatseq_in_result = profile.inport("float_seq_in");
    auto floatseq_out_result = profile.outport("float_seq_out");
    if (floatseq_in_result.result == ssr::rtno2::RESULT::OK && floatseq_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_seq_send_and_recv_test<float>("FloatSeq positive single element vector test. Send {3.0f} then recv it", logger, protocol, "float_seq_in", "float_seq_out", {1.0f}, {3.0f});
        conduct_seq_send_and_recv_test<float>("FloatSeq positive vector test. Send {3.0f, 2.0f, 1.0f} then recv it", logger, protocol, "float_seq_in", "float_seq_out", {1.0f}, {3.0f, 2.0f, 1.0f});
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping FloatSeq data test. float_seq_in or float_seq_out port not found. in({}), out({})", ssr::rtno2::result_to_string(floatseq_in_result.result), ssr::rtno2::result_to_string(floatseq_out_result.result));
    }
    // DoubleSeq test
    auto doubleseq_in_result = profile.inport("double_seq_in");
    auto doubleseq_out_result = profile.outport("double_seq_out");
    if (doubleseq_in_result.result == ssr::rtno2::RESULT::OK && doubleseq_out_result.result == ssr::rtno2::RESULT::OK)
    {
        conduct_seq_send_and_recv_test<double>("DoubleSeq positive single element vector test. Send {3.0} then recv it", logger, protocol, "double_seq_in", "double_seq_out", {1.0}, {3.0});
        conduct_seq_send_and_recv_test<double>("DoubleSeq positive vector test. Send {3.0, 2.0, 1.0} then recv it", logger, protocol, "double_seq_in", "double_seq_out", {1.0}, {3.0, 2.0, 1.0});
    }
    else
    {
        RTNO_WARN(logger, "[TEST] Skipping DoubleSeq data test. double_seq_in or double_seq_out port not found. in({}), out({})", ssr::rtno2::result_to_string(doubleseq_in_result.result), ssr::rtno2::result_to_string(doubleseq_out_result.result));
    }
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
    auto protocol = new protocol_t(serial_port, loglevel, loglevel);
    try
    {
        do_test(logger, *protocol);
    }
    catch (const std::exception &e)
    {
        RTNO_ERROR(logger, "[TEST] Exception: {}", e.what());
    }
    delete protocol;
    delete serial_port;
    return 0;
}