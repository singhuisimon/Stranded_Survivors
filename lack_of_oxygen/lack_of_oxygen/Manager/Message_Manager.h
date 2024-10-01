#pragma once
#ifndef LOF_MESSAGING_SYSTEM_H
#define LOF_MESSAGING_SYSTEM_H

#include "Manager.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <any>

namespace lof {

    class Observer {
    public:
        virtual void HandleMessage(const std::string& message_type, const std::any& msg_data) = 0;
        virtual ~Observer() = default;
    };

    class Observable {
    public:
        virtual void RegisterObserver(const std::string& message_type, Observer* observer) = 0;
        virtual void UnregisterObserver(const std::string& message_type, Observer* observer) = 0;
        virtual void SendToObservers(const std::string& message_type, const std::any& msg_data) = 0;
        virtual void ProcessMessage(const std::string& message_type, const std::any& msg_data) = 0;
        virtual ~Observable() = default;
    };

    class MessageManager : public Manager, public Observable {
    public:
        static MessageManager& get_instance();

        int start_up() override;
        void shut_down() override;

        // Observable interface implementation
        void RegisterObserver(const std::string& message_type, Observer* observer) override;
        void UnregisterObserver(const std::string& message_type, Observer* observer) override;
        void SendToObservers(const std::string& message_type, const std::any& msg_data) override;
        void ProcessMessage(const std::string& message_type, const std::any& msg_data) override;

    private:
        MessageManager();
        MessageManager(const MessageManager&) = delete;
        MessageManager& operator=(const MessageManager&) = delete;

        std::unordered_map<std::string, std::vector<Observer*>> observers;
    };

    // Define a macro for easy access to the MessageManager singleton
#define MM lof::MessageManager::get_instance()

} // namespace lof

#endif // LOF_MESSAGING_SYSTEM_H