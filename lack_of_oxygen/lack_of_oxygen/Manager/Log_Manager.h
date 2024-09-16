/**
 * @file Log_Manager.h
 * @brief Manages logging functionality within the game engine.
 * @author Simon Chan
 * @date September 15, 2024
 */

#pragma once

#ifndef LOF_LOG_MANAGER_H
#define LOF_LOG_MANAGER_H

 // Include base Manager class
#include "Manager.h"

 // Include standard headers
#include <string>
#include <fstream>
#include <memory>
#include <mutex>

namespace lof {

#define LM lof::Log_Manager::get_instance()

    const std::string LOGFILE_DEFAULT = "Lack_Of_Oxygen.log";

    class Log_Manager : public Manager {
    private:
        static std::unique_ptr<Log_Manager> instance;
        static std::once_flag once_flag;

        bool do_flush;
        std::ofstream log_file;
        std::string log_file_name;

        /**
         * @brief Private constructor to enforce singleton pattern.
         */
        Log_Manager();

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        Log_Manager(const Log_Manager&) = delete;

        /**
         * @brief Deleted assignment operator to prevent assignment.
         */
        Log_Manager& operator=(const Log_Manager&) = delete;

    public:
        /**
         * @brief Virtual destructor for Log_Manager.
         */
        virtual ~Log_Manager();

        /**
         * @brief Gets the singleton instance of Log_Manager.
         * @return Reference to the Log_Manager instance.
         */
        static Log_Manager& get_instance();

        /**
         * @brief Starts up the Log_Manager with default settings.
         * @return 0 if successful, else a negative number.
         */
        int start_up() override;

        /**
         * @brief Starts up the Log_Manager with a specified log file.
         * @param log_file_name The name of the log file to use.
         * @return 0 if successful, else a negative number.
         */
        int start_up(const std::string& log_file_name);

        /**
         * @brief Shuts down the Log_Manager.
         */
        void shut_down() override;

        /**
         * @brief Writes a formatted log message to the log file.
         * @param fmt The format string.
         * @param ... Variable arguments to be formatted.
         * @return Number of bytes written, or -1 if an error occurred.
         */
        int write_log(const char* fmt, ...);

        /**
         * @brief Sets whether to flush the log file after each write.
         * @param new_do_flush True to enable flushing, false otherwise.
         */
        void set_flush(bool new_do_flush = true);
    };

} // namespace lof

#endif // LOF_LOG_MANAGER_H