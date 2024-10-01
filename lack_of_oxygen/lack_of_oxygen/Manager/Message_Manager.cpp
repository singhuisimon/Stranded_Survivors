#include "Message_Manager.h"
#include "Log_Manager.h"
#include <algorithm>

namespace lof {

    MessageManager::MessageManager() {
        set_type("MessageManager");
    }

    MessageManager& MessageManager::get_instance() {
        static MessageManager instance;
        return instance;
    }

    int MessageManager::start_up() {
        if (is_started()) {
            return 0; // Already started
        }

        LM.write_log("MessageManager::start_up(): MessageManager started successfully.");
        m_is_started = true;
        return 0;
    }

    void MessageManager::shut_down() {
        if (!is_started()) {
            return;
        }

        observers.clear();
        LM.write_log("MessageManager::shut_down(): MessageManager shut down successfully.");
        m_is_started = false;
    }

    void MessageManager::RegisterObserver(const std::string& message_type, Observer* observer) {
        if (!is_started()) {
            LM.write_log("MessageManager::RegisterObserver(): MessageManager is not started.");
            return;
        }

        observers[message_type].push_back(observer);
        LM.write_log("MessageManager::RegisterObserver(): Registered observer for message type '%s'.", message_type.c_str());
    }

    void MessageManager::UnregisterObserver(const std::string& message_type, Observer* observer) {
        if (!is_started()) {
            LM.write_log("MessageManager::UnregisterObserver(): MessageManager is not started.");
            return;
        }

        auto it = observers.find(message_type);
        if (it != observers.end()) {
            auto& obs_list = it->second;
            obs_list.erase(std::remove(obs_list.begin(), obs_list.end(), observer), obs_list.end());
            LM.write_log("MessageManager::UnregisterObserver(): Unregistered observer for message type '%s'.", message_type.c_str());
        }
    }

    void MessageManager::SendToObservers(const std::string& message_type, const std::any& msg_data) {
        if (!is_started()) {
            LM.write_log("MessageManager::SendToObservers(): MessageManager is not started.");
            return;
        }

        auto it = observers.find(message_type);
        if (it != observers.end()) {
            for (auto observer : it->second) {
                observer->HandleMessage(message_type, msg_data);
            }
        }
        LM.write_log("MessageManager::SendToObservers(): Sent message of type '%s' to observers.", message_type.c_str());
    }

    void MessageManager::ProcessMessage(const std::string& message_type, const std::any& msg_data) {
        if (!is_started()) {
            LM.write_log("MessageManager::ProcessMessage(): MessageManager is not started.");
            return;
        }

        SendToObservers(message_type, msg_data);
        LM.write_log("MessageManager::ProcessMessage(): Processed message of type '%s'.", message_type.c_str());
    }

} // namespace lof