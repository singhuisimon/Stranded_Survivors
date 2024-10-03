/**
 * @file Log_Manager.cpp
 * @brief Implements the Log_Manager class helper functions.
 * @author Simon Chan
 * @date September 15, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

// Include header file
#include "Log_Manager.h"

// Include constants
#include "../Utility/Constant.h"

// Include standard headers
#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace lof {

    std::unique_ptr<Log_Manager> Log_Manager::instance;
    std::once_flag Log_Manager::once_flag;

    // flush set to true by default
    Log_Manager::Log_Manager()
        : do_flush(true), log_file_name(LOGFILE_DEFAULT) {
        set_type("Log_Manager");
        m_is_started = false;
        set_time(0);
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

        // Reset the Clock to start timing from now
        clock = Clock(); // Reinitialize the clock

        // Optionally, log the start-up event with timestamp
        write_log("Log_Manager::start_up(): Logging started. Log file: %s", log_file_name.c_str());

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

        // Get current elapsed time in microseconds since start
        int64_t current_time = clock.split_total();

        // Convert microseconds to hours, minutes, seconds, milliseconds
        int64_t total_milliseconds = current_time / MILLISECONDS_PER_SECOND;
        int64_t total_seconds = total_milliseconds / MILLISECONDS_PER_SECOND;
        int64_t total_minutes = total_seconds / SECONDS_PER_MINUTE;
        int64_t total_hours = total_minutes / MINUTES_PER_HOUR;

        int64_t milliseconds = total_milliseconds % MILLISECONDS_PER_SECOND;
        int64_t seconds = total_seconds % SECONDS_PER_MINUTE;
        int64_t minutes = total_minutes % MINUTES_PER_HOUR;
        int64_t hours = total_hours % HOURS_PER_DAY;

        // Format time as [HH:MM:SS.mmm]
        std::stringstream time_stream;
        time_stream << "["
            << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setfill('0') << std::setw(2) << minutes << ":"
            << std::setfill('0') << std::setw(2) << seconds << "."
            << std::setfill('0') << std::setw(3) << milliseconds
            << "] ";

        // Format the log message
        char buffer[1024];
        va_list args;
        va_start(args, fmt);
        int written = vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        if (written < 0) {
            return -1; // Error during formatting
        }

        // Write the timestamped log entry
        log_file << time_stream.str() << buffer << std::endl;

        if (do_flush) {
            log_file.flush();
        }

        // Optionally, return the number of characters written
        // Here, we return the number of characters in the user message
        return written;
    }


    void Log_Manager::set_flush(bool new_do_flush) {
        do_flush = new_do_flush;
    }

} // namespace lof