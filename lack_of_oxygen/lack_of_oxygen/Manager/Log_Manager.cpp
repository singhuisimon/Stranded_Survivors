/**
 * @file Log_Manager.cpp
 * @brief Implements the Log_Manager class methods.
 * @author Simon Chan
 * @date September 15, 2024
 */

// Include header file
#include "Log_Manager.h"

// Include standard headers
#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <sstream>

namespace lof {

    std::unique_ptr<Log_Manager> Log_Manager::instance;
    std::once_flag Log_Manager::once_flag;


    Log_Manager::Log_Manager()
        : do_flush(false), log_file_name(LOGFILE_DEFAULT) {
        set_type("Log_Manager");
        m_is_started = false;
    }


    Log_Manager::~Log_Manager() {
        if (is_started()) {
            shut_down();
        }
    }


    Log_Manager& Log_Manager::get_instance() {
        std::call_once(once_flag, []() {
            instance.reset(new Log_Manager);
            });
        return *instance;
    }


    int Log_Manager::start_up() {
        return start_up(LOGFILE_DEFAULT);
    }


    int Log_Manager::start_up(const std::string& new_log_file_name) {
        if (is_started()) {
            return 0; // Already started
        }

        this->log_file_name = new_log_file_name;
        log_file.open(new_log_file_name, std::ios::out | std::ios::trunc);

        if (!log_file.is_open()) {
            return -1; // Failed to open log file
        }

        m_is_started = true;
        return 0;
    }


    void Log_Manager::shut_down() {
        if (!is_started()) {
            return; // Not started
        }

        if (log_file.is_open()) {
            log_file.close();
        }

        m_is_started = false;
    }


    int Log_Manager::write_log(const char* fmt, ...) {
        if (!is_started() || !log_file.is_open()) {
            return -1; // Log_Manager not started or log file not open
        }

        char buffer[1024];
        va_list args;
        va_start(args, fmt);
        int written = vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        if (written < 0) {
            return -1; // Error during formatting
        }

        log_file << buffer << std::endl;

        if (do_flush) {
            log_file.flush();
        }

        return written;
    }


    void Log_Manager::set_flush(bool new_do_flush) {
        do_flush = new_do_flush;
    }

} // namespace lof