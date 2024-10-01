/**
 * @file Manager.h
 * @brief Base class for all managers in the system.
 * @author Simon Chan
 * @date September 15, 2024
 */

#ifndef LOF_MANAGER_H
#define LOF_MANAGER_H

// Include standard headers
#include <string>

namespace lof {

    /**
     * @class Manager
     * @brief Base class for all managers in the system.
     */
    class Manager {

    private:
        int64_t manager_time = 0; // Private data member to store manager's consumption time in game loop.

    protected:
        std::string m_type;    //< Manager type identifier.
        bool m_is_started;     //< True if start_up() succeeded.

        /**
         * @brief Set the type identifier of the Manager.
         * @param new_type The new type as a string.
         */
        void set_type(const std::string& new_type);

    public:
        /**
         * @brief Default constructor.
         */
        Manager();

        /**
         * @brief Virtual destructor.
         */
        virtual ~Manager();

        /**
         * @brief Get the type identifier of the Manager.
         * @return The type as a string.
         */
        std::string get_type() const;

        /**
         * @brief Startup the Manager.
         * @return 0 if successful, else a negative number.
         */
        virtual int start_up();

        /**
         * @brief Shutdown the Manager.
         */
        virtual void shut_down();

        /**
         * @brief Check if the Manager has been started successfully.
         * @return True if started, false otherwise.
         */
        bool is_started() const;


        /**
         * @brief Virtual function to get the manager_time private member of the Manager class.
         * @return The manager's consumption time.
         */
        virtual int64_t get_time() const {
            return manager_time;
        }

        /**
         * @brief Virtual function to set the manager_time private member of the System class.
         * @param time The value of time to set the private member variable.
         */
        virtual void set_time(int64_t time) {
            manager_time = time;
        }
    };

} // namespace lof

#endif // LOF_MANAGER_H