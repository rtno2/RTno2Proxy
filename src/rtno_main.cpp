
#include "hal/Serial.h"
#include "hal/EtherTcp.h"
#include "rtno2/protocol.h"
#include "rtno2/logger.h"

#include <iostream>

using namespace ssr::rtno2;

LOGLEVEL loglevel = LOGLEVEL::DEBUG;

template <typename T>
inline std::string strjoin(const std::vector<T> &v)
{
    std::stringstream ss;
    for (int i = 0; i < v.size(); i++)
    {
        ss << v[i];
        if (i != v.size() - 1)
        {
            ss << ",";
        }
    }
    return ss.str();
}

std::vector<std::string> strsplit(const std::string &str, const char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
        {
            elems.push_back(item);
        }
    }
    return elems;
}

template <typename T>
std::vector<T> atois(const std::string &str, const char delim, const std::function<T(const std::string &)> &func)
{
    std::vector<T> elems;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
        {
            elems.push_back(func(item));
        }
    }
    return elems;
}

template <typename T>
struct is_vector : std::false_type
{
};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type
{
};

template <typename T, typename = std::enable_if_t<std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, double>>>
std::string result_t_to_string(const result_t<T> &result)
{
    if (result.result == ::RESULT::OK)
    {
        std::stringstream ss;
        ss << "RESULT::OK(" << result.value.value() << ")";
        return ss.str();
    }
    return result_to_string(result.result);
}

template <typename T>
std::string result_t_to_string(const ::result_t<std::vector<T>> &result)
{
    if (result.result == RESULT::OK)
    {
        std::stringstream ss;
        ss << "RESULT::OK([" << strjoin(result.value.value()) << "])";
        return ss.str();
    }
    return result_to_string(result.result);
}

std::string print(logger_t &logger, protocol_t &rtno, const std::string &name_str)
{
    RTNO_TRACE(logger, "print({})", name_str);
    auto result = rtno.get_profile(1000 * 1000);
    if (result.result != RESULT::OK)
    {
        return result_to_string(result.result);
    }
    auto prof = result.value.value();
    RTNO_DEBUG(logger, " - getProfile -> {}", prof.to_string());
    for (auto port : prof.outports_)
    {
        if (port.name() == name_str)
        {
            auto type_code = port.typecode();
            if (type_code == TYPECODE::TIMED_BOOLEAN)
            {
            }
            else if (type_code == TYPECODE::TIMED_CHAR)
            {
            }
            else if (type_code == TYPECODE::TIMED_LONG)
            {
                auto val = rtno.receive_as<int32_t>(name_str);
                return result_t_to_string(val);
            }
            else if (type_code == TYPECODE::TIMED_FLOAT)
            {
                auto val = rtno.receive_as<float>(name_str);
                return result_t_to_string(val);
            }
            else if (type_code == TYPECODE::TIMED_DOUBLE)
            {
                auto val = rtno.receive_as<double>(name_str);
                return result_t_to_string(val);
            }
            else if (type_code == TYPECODE::TIMED_LONG_SEQ)
            {
                auto v = rtno.receive_seq_as<int32_t>(name_str);
                return result_t_to_string(v);
            }
            else if (type_code == TYPECODE::TIMED_FLOAT_SEQ)
            {
                auto v = rtno.receive_seq_as<float>(name_str);
                return result_t_to_string(v);
            }
            else if (type_code == TYPECODE::TIMED_DOUBLE_SEQ)
            {
                auto v = rtno.receive_seq_as<double>(name_str);
                return result_t_to_string(v);
            }
            else
            {
                return result_to_string(RESULT::ERR);
            }
        }
    }
    return result_to_string(RESULT::OUTPORT_NOT_FOUND);
}

RESULT inject(logger_t &logger, protocol_t &rtno, const std::string &name_str, const std::string &data_str)
{
    RTNO_DEBUG(logger, "inject({}, {})", name_str, data_str);
    auto result = rtno.get_profile(1000 * 1000);
    if (result.result != RESULT::OK)
    {
        return result.result;
    }
    auto prof = result.value.value();
    RTNO_DEBUG(logger, " - getProfile -> {}", prof.to_string());
    for (auto port : prof.inports_)
    {
        if (port.name() == name_str)
        {
            auto type_code = port.typecode();
            if (type_code == TYPECODE::TIMED_BOOLEAN)
            {
                uint8_t data = data_str == "true" ? 1 : 0;
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), sizeof(uint8_t));
            }
            else if (type_code == TYPECODE::TIMED_CHAR)
            {
                char data = data_str.c_str()[0];
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), sizeof(char));
            }
            else if (type_code == TYPECODE::TIMED_LONG)
            {
                int32_t data = atoi(data_str.c_str());
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), sizeof(int32_t));
            }
            else if (type_code == TYPECODE::TIMED_FLOAT)
            {
                float data = atof(data_str.c_str());
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), sizeof(float));
            }
            else if (type_code == TYPECODE::TIMED_DOUBLE)
            {
                double data = atof(data_str.c_str());
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), sizeof(double));
            }
            else if (type_code == TYPECODE::TIMED_LONG_SEQ)
            {
                auto data = atois<int32_t>(data_str, ',', [](const std::string &s)
                                           { return atoi(s.c_str()); });
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), data.size() * sizeof(int32_t));
            }
            else if (type_code == TYPECODE::TIMED_FLOAT_SEQ)
            {
                auto data = atois<float>(data_str, ',', [](const std::string &s)
                                         { return (float)atof(s.c_str()); });
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), data.size() * sizeof(float));
            }
            else if (type_code == TYPECODE::TIMED_DOUBLE_SEQ)
            {
                auto data = atois<double>(data_str, ',', [](const std::string &s)
                                          { return atof(s.c_str()); });
                return rtno.send_inport_data(name_str, (uint8_t *)(&data), data.size() * sizeof(double));
            }
            else
            {
                return RESULT::OK;
            }
        }
    }
    return RESULT::INPORT_NOT_FOUND;
}

ssr::SerialDevice *create_serial(const std::string &filename, const int int_arg)
{
    if (filename.substr(0, 6) == "tcp://")
    {
        try
        {
            return new ssr::EtherTcp(filename.substr(6).c_str(), int_arg);
        }
        catch (ssr::SocketException &se)
        {
            std::cerr << se.what() << std::endl;
            return nullptr;
        }
    }
    else
    {
        try
        {
            return new ssr::Serial(filename.c_str(), int_arg);
        }
        catch (net::ysuga::ComOpenException &ce)
        {
            std::cout << ce.what() << std::endl;
            return nullptr;
        }
    }
}

int do_main(logger_t &logger, protocol_t &rtno, const std::string &command, const std::vector<std::string> &args, const uint32_t wait_usec = 1000 * 1000)
{
    if (command == "getprofile")
    {
        auto prof = rtno.get_profile(wait_usec);
        if (prof.result != RESULT::OK)
        {
            std::cout << result_to_string(prof.result) << std::endl;
        }
        else
        {
            std::cout << prof.value.value().to_string() << std::endl;
        }
    }
    else if (command == "getlog")
    {
        auto logresult = rtno.get_log(wait_usec);
        if (logresult.result != RESULT::OK)
        {
            std::cout << result_to_string(logresult.result) << std::endl;
        }
        else
        {
            std::cout << logresult.value.value() << std::endl;
        }
    }
    else if (command == "getstate")
    {
        auto state = rtno.get_state(wait_usec);
        if (state.result != RESULT::OK)
        {
            std::cout << result_to_string(state.result) << std::endl;
        }
        else
        {
            std::cout << state_to_string(state.value.value()) << std::endl;
        }
    }
    else if (command == "getectype")
    {
        auto ectype = rtno.get_ec_type(wait_usec);
        if (ectype.result != RESULT::OK)
        {
            std::cout << result_to_string(ectype.result) << std::endl;
        }
        else
        {
            std::cout << ec_type_to_string(ectype.value.value()) << std::endl;
        }
    }
    else if (command == "activate")
    {
        auto state = rtno.activate(wait_usec);
        std::cout << result_to_string(state) << std::endl;
    }
    else if (command == "deactivate")
    {
        auto state = rtno.deactivate(wait_usec);
        std::cout << result_to_string(state) << std::endl;
    }
    else if (command == "execute")
    {
        auto result = rtno.execute(wait_usec);
        std::cout << result_to_string(result) << std::endl;
    }
    else if (command == "inject")
    {
        std::string name_str = args[1];
        std::string data_str = args[2];
        auto state = inject(logger, rtno, name_str, data_str);
        std::cout << result_to_string(state) << std::endl;
    }
    else if (command == "print")
    {
        std::string name_str = args[1];
        auto state = print(logger, rtno, name_str);
        std::cout << state << std::endl;
    }
    return 0;
}

int do_interactive(logger_t &logger, protocol_t &rtno)
{
    while (true)
    {
        std::cout << "> " << std::ends;
        std::string commandline;
        std::getline(std::cin, commandline);
        auto elems = strsplit(commandline, ' ');
        if (elems.size() == 0)
            continue;
        // std::cout << strjoin(elems) << std::endl;
        if (elems[0] == "exit")
            break;
        do_main(logger, rtno, elems[0], elems);
    }
    return 0;
}

void print_usage()
{
    std::cout << "> getprofile # Get RTCProfile from RTno" << std::endl;
    std::cout << "> getstate.  # Get RTCState from RTno" << std::endl;
    std::cout << "> getectype. # Get EC's type from RTno" << std::endl;
    std::cout << "> activate   # Activate RTC" << std::endl;
    std::cout << "> deactivate # Deactivate RTC" << std::endl;
    std::cout << "> execute    # Execute RTC." << std::endl;
    std::cout << "             # If RTno uses ProxySynchronousExecutionContext, " << std::endl;
    std::cout << "             # This function sends an execute trigger to RTno." << std::endl;
    std::cout << "             # Eventually, the RTno's 'on_execute' function is called once." << std::endl;
    std::cout << "> print [PORT_NAME] # Print value of output buffer of RTno's outport" << std::endl;
    std::cout << "             # Usually, output buffer is written in 'on_execute' function, so" << std::endl;
    std::cout << "             # If 'print' function causes error, you might as well do 'execute'." << std::endl;
    std::cout << "> inject [PORT_NAME] [VALUE]..." << std::endl;
    std::cout << "             # This function send value to inport's input buffer." << std::endl;
    std::cout << "             # [VALUE] is parsed according to the value type of InPort specified." << std::endl;
    std::cout << "  ex: > inject in0 2     # Write single value 2. This will work for TimedLong, TimedFloat, and TimedDouble" << std::endl;
    std::cout << "      > inject in0 2,3,4 # Write sequence value [2,3,4]. This will work for TimedLongSeq, TimedFloatSeq, and TimedDoubleSeq." << std::endl;
    std::cout << "             # You can't put any while-space and any brackets nor parentheses in the array value." << std::endl;
}

void print_usage_whole_interactive()
{
    std::cout << "Usage:" << std::endl;
    std::cout << " - If you have not opened device..." << std::endl;
    std::cout << "> open [PORT_FILE_NAME] [PORT_ARGUMENT]" << std::endl;
    std::cout << " ex: open COM3 57600 # for COM port in Windows OS." << std::endl;
    std::cout << "     open /dev/ttyUSB0 57600 # for tty port in Linux/OSX" << std::endl;
    std::cout << "     open tcp://192.168.1.1 12345 # for TCP port (addr 192.168.1.1, number 12345)" << std::endl;
    std::cout << std::endl;
    std::cout << " - If you are ready..." << std::endl;
    print_usage();
    std::cout << "> close.     # Close serial device." << std::endl;
    std::cout << "> exit       # Exit this program" << std::endl;
}
int do_whole_interactive(logger_t &logger)
{
    ssr::SerialDevice *serial_port = NULL;
    protocol_t *rtno = NULL;
    while (true)
    {
        if (rtno)
        {
            std::cout << "[opened]>" << std::ends;
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
                rtno = new protocol_t(serial_port, loglevel);
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
            if (serial_port)
            {
                do_main(logger, *rtno, elems[0], elems);
            }
            else
            {
                std::cout << "ERROR: Port is not opened." << std::endl;
                print_usage_whole_interactive();
            }
        }
    }
}

int main(const int argc, const char *argv[])
{
    // logger = ssr::getLogger("main");

    ssr::rtno2::logger_t logger(get_logger("main"));
    set_log_level(&logger, LOGLEVEL::DEBUG);

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
    std::string command = argv[3];

    auto serial_port = create_serial(filename, baudrate);
    if (!serial_port)
    {
        return -1;
    }
    auto protocol = new protocol_t(serial_port, loglevel);

    if (command == "interactive")
    {
        do_interactive(logger, *protocol);
        return 0;
    }
    do_main(logger, *protocol, command, args);
    delete protocol;
    delete serial_port;
    return 0;
}