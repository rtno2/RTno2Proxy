#pragma once

#include "hal/Serial.h"
#include "hal/EtherTcp.h"
#include "rtno2/protocol.h"
#include "rtno2/logger.h"

ssr::rtno2::RESULT setup(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol);
ssr::rtno2::RESULT teardown(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol);

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

template <typename T>
ssr::rtno2::RESULT do_send_and_recv_test(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol, const std::string &inport_name, const std::string &outport_name, const T initial_data, const T send_data)
{
    uint32_t wait_usec = 200 * 1000;
    int try_count = 10;
    ssr::rtno2::RESULT result;
    {
        auto v = protocol.receive_as<T>(outport_name, wait_usec, try_count);
        if (v.result != ssr::rtno2::RESULT::OK)
        {
            RTNO_ERROR(logger, "receive_as('{}') failed({})", outport_name, ssr::rtno2::result_to_string(v.result));
            return v.result;
        }
        if (v.value.value() != initial_data)
        {
            RTNO_ERROR(logger, "received value is invalid ({} != {})", initial_data, v.value.value());
            return ssr::rtno2::RESULT::ERR;
        }
    }

    if ((result = protocol.send_as<T>(inport_name, send_data, wait_usec, try_count)) != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "send_as('{}') failed({})", inport_name, ssr::rtno2::result_to_string(result));
        return result;
    }
    if ((result = protocol.execute(wait_usec, try_count)) != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "execute failed({})", ssr::rtno2::result_to_string(result));
        return result;
    }
    {
        auto v = protocol.receive_as<T>(outport_name, wait_usec, try_count);
        if (v.result != ssr::rtno2::RESULT::OK)
        {
            RTNO_ERROR(logger, "receive_as('{}') failed({})", outport_name, ssr::rtno2::result_to_string(v.result));
            return v.result;
        }
        if (v.value.value() != send_data)
        {
            RTNO_ERROR(logger, "received value is invalid ({} != {})", send_data, v.value.value());
            return ssr::rtno2::RESULT::ERR;
        }
    }
    return ssr::rtno2::RESULT::OK;
}

template <typename T>
void conduct_send_and_recv_test(const std::string &test_name, ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol, const std::string &inport_name, const std::string &outport_name, const T initial_data, const T send_data)
{
    ssr::rtno2::RESULT result;
    RTNO_INFO(logger, "[TEST]'{}'", test_name);
    if ((result = setup(logger, protocol)) == ssr::rtno2::RESULT::OK)
    {
        RTNO_DEBUG(logger, "  [SETUP]'{}' OK", test_name);
        if ((result = do_send_and_recv_test<T>(logger, protocol, inport_name, outport_name, initial_data, send_data)) != ssr::rtno2::RESULT::OK)
        {
            RTNO_ERROR(logger, "  [FAILED]'{}' failed. RESULT: {}", test_name, ssr::rtno2::result_to_string(result));
        }
        else
        {
            RTNO_INFO(logger, "  [PASSED]'{}' OK", test_name);
        }
        if ((result = teardown(logger, protocol)) == ssr::rtno2::RESULT::OK)
        {
            RTNO_DEBUG(logger, "  [TEARDOWN]'{}' OK", test_name);
        }
    }
    else
    {
        RTNO_ERROR(logger, "  [FAILED]'{}' setup failed. RESULT: {}", test_name, ssr::rtno2::result_to_string(result));
    }
}

/********** */

template <typename T>
bool seq_equal(const std::vector<T> &a, const std::vector<T> &b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

template <typename T>
std::string vec_to_str(const std::vector<T> &v)
{
    return "[" + strjoin(v) + "]";
}

template <typename T>
ssr::rtno2::RESULT do_seq_send_and_recv_test(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol, const std::string &inport_name, const std::string &outport_name, const std::vector<T> &initial_data, const std::vector<T> &send_data)
{
    uint32_t wait_usec = 2000 * 1000;
    int try_count = 10;
    ssr::rtno2::RESULT result;
    {
        auto v = protocol.receive_seq_as<T>(outport_name, wait_usec, try_count);
        if (v.result != ssr::rtno2::RESULT::OK)
        {
            RTNO_ERROR(logger, "receive_seq_as('{}') failed({})", outport_name, ssr::rtno2::result_to_string(v.result));
            return v.result;
        }
        if (!seq_equal(v.value.value(), initial_data))
        {
            RTNO_ERROR(logger, "received value is invalid ({} != {})", vec_to_str(initial_data), vec_to_str(v.value.value()));
            return ssr::rtno2::RESULT::ERR;
        }
    }

    if ((result = protocol.send_seq_as<T>(inport_name, send_data, send_data.size(), wait_usec, try_count)) != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "send_seq_as('{}') failed({})", inport_name, ssr::rtno2::result_to_string(result));
        return result;
    }
    if ((result = protocol.execute(wait_usec, try_count)) != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "execute failed({})", ssr::rtno2::result_to_string(result));
        return result;
    }
    {
        auto v = protocol.receive_seq_as<T>(outport_name, wait_usec, try_count);
        if (v.result != ssr::rtno2::RESULT::OK)
        {
            RTNO_ERROR(logger, "receive_seq_as('{}') failed({})", outport_name, ssr::rtno2::result_to_string(v.result));
            return v.result;
        }
        if (!seq_equal(v.value.value(), send_data))
        {
            RTNO_ERROR(logger, "received value is invalid ({} != {})", vec_to_str(send_data), vec_to_str(v.value.value()));
            return ssr::rtno2::RESULT::ERR;
        }
    }
    return ssr::rtno2::RESULT::OK;
}

template <typename T>
void conduct_seq_send_and_recv_test(const std::string &test_name, ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol, const std::string &inport_name, const std::string &outport_name, const std::vector<T> &initial_data, const std::vector<T> &send_data)
{
    ssr::rtno2::RESULT result;
    if ((result = setup(logger, protocol)) == ssr::rtno2::RESULT::OK)
    {
        // RTNO_INFO(logger, "'{}'", test_name);
        if ((result = do_seq_send_and_recv_test<T>(logger, protocol, inport_name, outport_name, initial_data, send_data)) != ssr::rtno2::RESULT::OK)
        {
            RTNO_ERROR(logger, "'{}' failed. RESULT: {}", test_name, ssr::rtno2::result_to_string(result));
        }
        if ((result = teardown(logger, protocol)) == ssr::rtno2::RESULT::OK)
        {
            RTNO_INFO(logger, "'{}' OK", test_name);
        }
    }
}

/*******************************************************************
 *
 * For initial settings of test
 *
 *******************************************************************/
ssr::rtno2::RESULT setup(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol)
{
    uint32_t wait_usec = 200 * 1000;
    int try_count = 10;
    ssr::rtno2::RESULT result;
    if ((result = protocol.activate(wait_usec, try_count)) != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "in setup, activate failed({})", ssr::rtno2::result_to_string(result));
        return result;
    }
    if ((result = protocol.execute(wait_usec, try_count)) != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "in setup, execute failed({})", ssr::rtno2::result_to_string(result));
        return result;
    }
    return ssr::rtno2::RESULT::OK;
}

ssr::rtno2::RESULT teardown(ssr::rtno2::logger_t &logger, ssr::rtno2::protocol_t &protocol)
{
    uint32_t wait_usec = 200 * 1000;
    int try_count = 10;
    ssr::rtno2::RESULT result;
    if ((result = protocol.deactivate(wait_usec, try_count)) != ssr::rtno2::RESULT::OK)
    {
        RTNO_ERROR(logger, "in teardown, deactivate failed({})", ssr::rtno2::result_to_string(result));
        return result;
    }
    return ssr::rtno2::RESULT::OK;
}

/*******************************************************************
 *
 * For initial settings of protocol
 *
 *******************************************************************/
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
