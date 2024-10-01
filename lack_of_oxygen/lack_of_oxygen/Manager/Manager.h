/**
 * @file Manager.h
 * @brief Base class for all managers in the system.
 * @author Simon Chan
 * @date September 15, 2024
 * Copyright (C) 20xx DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
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

    protected:
        std::string m_type;    // Manager type identifier.
        bool m_is_started;     // True if start_up() succeeded.

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
    };

} // namespace lof

#endif // LOF_MANAGER_H