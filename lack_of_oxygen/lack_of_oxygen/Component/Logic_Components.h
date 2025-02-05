#pragma once

#ifndef LOF_LOGIC_COMPONENT_H
#define LOF_LOGIC_COMPONENT_H

#include "../Utility/Type.h"
#include "../Component/Component.h"
#include "../Utility/Constant.h"
#include "../Utility/Vector2D.h"
#include <unordered_set>
#include <vector>
#include <string>

namespace lof {

    using ScriptData = std::unordered_map<std::string, std::variant<int, float, std::string, bool, Vec2D>>;

    class Logic_Component : public Component {
    public:

        struct LogicData {
            std::string script_name;
            std::string init_func;
            std::string update_func;
            std::string end_func;
            ScriptData script_data;
            ExecutionState state;
            bool is_active;

            LogicData(const std::string& name, const std::string& init, const std::string& update,
                const std::string& end, const ScriptData& data, bool active = true)
                : script_name(name), init_func(init), update_func(update),
                end_func(end), script_data(data), is_active(active), state(ExecutionState::Uninitialized) {
            }
        };

        std::vector<std::shared_ptr<LogicData>> logic_datas;

        Logic_Component() = default;

        const std::vector<std::shared_ptr<LogicData>>& get_logic_datas() const { return logic_datas; }

        void add_script(const std::string& name, const std::string& init, const std::string& update,
            const std::string& end, const ScriptData& data, bool active = true) {
            auto script = std::make_shared<LogicData>(name, init, update, end, data, true);

            logic_datas.push_back(script);
        }

        std::shared_ptr<LogicData> find_logic_data(const std::string& script_name, const std::string& update_func) const{
            auto it = std::find_if(logic_datas.begin(), logic_datas.end(),
                [&](const std::shared_ptr<LogicData>& logic_data) {
                    return logic_data->script_name == script_name &&
                        logic_data->update_func == update_func;
                });
            return (it != logic_datas.end()) ? *it : nullptr;
        }

        // Getter and Setter for the script data
        const ScriptData& get_script_data(const std::string& script_name, const std::string& update_func) const {
            auto logic_data = find_logic_data(script_name, update_func);
            if (logic_data) {
                return logic_data->script_data;
            }
            throw std::runtime_error("Script not found");
        }

        template<typename T>
        T get_script_individual_data(const ScriptData& script_data, const std::string& data_name) {
            auto it = script_data.find(data_name);
            if (it == script_data.end()) {
                throw std::out_of_range("Key not found in ScriptData: " + data_name);
            }
            if (auto value = std::get_if<T>(&it->second)) {
                return *value;
            }
            else {
                throw std::bad_variant_access();
            }
        }

        void set_script_data(const std::string& script_name, const std::string& update_func, const ScriptData& new_data) {
            auto logic_data = find_logic_data(script_name, update_func);
            if (logic_data) {
                logic_data->script_data = new_data;
            }
            else {
                throw std::runtime_error("Script not found");
            }
        }

        void set_active(const std::string& script_name, const std::string& update_func, bool active) {
            for (auto& logic_data : logic_datas) {
                if (logic_data->script_name == script_name && logic_data->update_func == update_func) {
                    logic_data->is_active = active;
                    return;
                }
            }

            throw std::runtime_error("Script instance not found: " + script_name + "( " + update_func + ")");
        }

        void remove_script(const std::string& script_name, const std::string& update_func) {
            auto it = std::remove_if(logic_datas.begin(), logic_datas.end(),
                [&](const std::shared_ptr<LogicData>& logic_data) {
                    return logic_data->script_name == script_name &&
                        logic_data->update_func == update_func;
                });
            if (it != logic_datas.end()) {
                logic_datas.erase(it, logic_datas.end());
            }
        }

        ExecutionState get_state(const std::string& script_name, const std::string& update_func) const {
            for (auto& logic_data : logic_datas) {
                if (logic_data->script_name == script_name && logic_data->update_func == update_func) {
                    return logic_data->state;
                }
            }
            return ExecutionState::Terminated;
        }

        void set_state(const std::string& script_name, const std::string& update_func, ExecutionState new_state) {
            for (auto& logic_data : logic_datas) {
                if (logic_data->script_name == script_name && logic_data->update_func == update_func) {
                    logic_data->state = new_state;
                    return;
                }
            }
        }

    };


    //class Logic_Component : public Component {
    //public:

    //    using ParamVariant = std::variant<float, int, bool, Vec2D, std::string>;

    //    struct ScriptData {
    //        std::unordered_map<std::string, ParamVariant> params;
    //        std::unordered_set<std::string> metadata_keys;
    //        std::string script_name;

    //        ScriptData(const std::string& name) : script_name(name) {}
    //    };

    //    std::unordered_map<std::string, std::unordered_map<std::string, ScriptData>> scripts;
    //    std::string category_name;
    //    bool is_active{ true };
    //    float timer{ 0.0f };

    //    Logic_Component(const std::string& category = "dummy") : category_name(category), is_active(true), timer(0.0f) {
    //        preset_values();
    //    }

    //    virtual void add_script(const std::string& category, const std::string& script_name) {

    //        if (script_name == "dummy" || category == "dummy") {
    //            return;
    //        }

    //        if (scripts[category].find(script_name) == scripts[category].end()) {
    //            scripts[category][script_name] = ScriptData(script_name); //Initialize empty parameters
    //            set_param(category, script_name, "timer", timer, true);
    //            set_param(category, script_name, "is_active", is_active);
    //        }
    //    }

    //    virtual void remove_script(const std::string& category, const std::string& script_name) {
    //        auto category_it = scripts.find(category);
    //        if (category_it != scripts.end()) {
    //            category_it->second.erase(script_name);
    //        }
    //    }

    //    //updates/set new key-value pair.
    //    virtual void set_param(const std::string& category, const std::string& script_name, const std::string& key, ParamVariant value, bool is_metadata = false) {
    //        auto& script = scripts[category][script_name]; //retrieve the scriptdata for the specified script_name
    //        script.params[key] = value; //set or update the parameter for this script
    //        if (is_metadata) {
    //            script.metadata_keys.insert(key); //optionally add to metadata_keys if is_metadata is true
    //        }
    //    }

    //    virtual void preset_values(const std::string& script_extension = "") {
    //        //keep all these metadata since it isn't needed to be printed
    //        add_script(category_name, "dummy" + script_extension);
    //    }

    //    //use to check if it exist. just use if(!variable) to catch any non-existant param gotten.
    //    std::optional<ParamVariant> get_param(const std::string& category, const std::string& script_name, const std::string& key) const {
    //        auto category_it = scripts.find(category);
    //        if (category_it != scripts.end()) {
    //            auto script_it = category_it->second.find(script_name);
    //            if (script_it != category_it->second.end()) {
    //                auto param_it = script_it->second.params.find(key);
    //                if (param_it != script_it->second.params.end()) {
    //                    return param_it->second;
    //                }
    //            }
    //        }
    //        return std::nullopt;
    //    }

    //    //ensure u retrieve the type of value u really want
    //    template <typename T>
    //    std::optional<T> get_param_as(const std::string& category, const std::string& script_name, const std::string& key) const {
    //        auto param = get_param(category, script_name, key);
    //        if (param && std::holds_alternative<T>(*param)) {
    //            return std::get<T>(*param);
    //        }
    //        return std::nullopt;
    //    }

    //    //use this before calling get_param_as to avoid accessing non-existent script
    //    bool has_script(const std::string& category, const std::string& script_name) const {
    //        auto category_it = scripts.find(category);
    //        if (category_it != scripts.end()) {
    //            return category_it->second.find(script_name) != category_it->second.end();
    //        }
    //        return false;
    //    }

    //    bool is_metadata(const std::string& category, const std::string& script_name, const std::string& key) const {
    //        auto category_it = scripts.find(category);
    //        if (category_it != scripts.end()) {
    //            auto script_it = category_it->second.find(script_name);
    //            if (script_it != category_it->second.end()) {
    //                return script_it->second.metadata_keys.count(key) > 0;
    //            }
    //        }
    //        return false;
    //    }

    //    void toggle_is_active(const std::string& category, const std::string& script_name) {
    //        auto is_active_param = get_param_as<bool>(category, script_name, "is_active");
    //        if (is_active_param) {
    //            //toggle the is_active state
    //            set_param(category, script_name, "is_active", !*is_active_param);
    //        }
    //    }

    //    std::string get_unique_component_name(const std::string& component_name, const std::string& unique_identifier) const{
    //        return component_name + "_" + unique_identifier;
    //    }

    //    virtual ~Logic_Component() = default;

    //};

    //class Object_Moving_Script_Component : public Logic_Component {
    //public:
    //    enum class MovementPattern {
    //        LINEAR = 0,            // Move back and forth in a line
    //        CIRCULAR = 1           // Move in a circular path
    //    };

    //    MovementPattern movement_pattern{ MovementPattern::LINEAR };
    //    float movement_speed{ 100.0f };
    //    float movement_range{ 200.0f };
    //    Vec2D origin_pos{ 0.0f, 0.0f };
    //    bool reverse_direction{ false };
    //    bool rotate_with_motion{ false };

    //    Object_Moving_Script_Component(const std::string& script_name) : Logic_Component("object_movmeent_script"){
    //        preset_values(script_name);
    //    }

    //    static std::string pattern_to_string(MovementPattern pattern) {
    //        switch (pattern) {
    //        case MovementPattern::LINEAR: return "LINEAR";
    //        case MovementPattern::CIRCULAR: return "CIRCULAR";
    //        default: return "UNKNOWN";
    //        }
    //        return (pattern == MovementPattern::LINEAR) ? "LINEAR" : "CIRCULAR";
    //    }

    //    static MovementPattern string_to_pattern(const std::string& str) {
    //        if (str == "LINEAR") return MovementPattern::LINEAR;
    //        if (str == "CIRCULAR") return MovementPattern::CIRCULAR;
    //        return MovementPattern::LINEAR;
    //    }

    //    void preset_values(const std::string& unique_script_name) override{
    //        add_script(category_name, unique_script_name);
    //        set_param(category_name, unique_script_name, "movement_speed", movement_speed);
    //        set_param(category_name, unique_script_name, "movement_range", movement_range);
    //        set_param(category_name, unique_script_name, "timer", 0.0f);
    //        set_param(category_name, unique_script_name, "origin_pos", origin_pos);
    //        set_param(category_name, unique_script_name, "reverse_direction", reverse_direction);
    //        set_param(category_name, unique_script_name, "rotate_with_motion", rotate_with_motion);
    //        
    //        std::string pattern_str = pattern_to_string(movement_pattern);
    //        set_param(category_name, unique_script_name, "movement_pattern", pattern_str);
    //    }

    //    MovementPattern get_movement_pattern(const std::string& script_name) const {
    //        auto param = get_param_as<std::string>(category_name, script_name, "movement_pattern");
    //        return param ? string_to_pattern(*param) : MovementPattern::LINEAR;
    //    }

    //    void set_movement_pattern(const std::string& script_name, MovementPattern pattern) {
    //        movement_pattern = pattern;
    //        set_param(category_name, script_name, "movement_pattern", pattern_to_string(pattern));
    //    }
    //};

}

//my first attempt
//public:
//    enum class LogicType {
//        RANDOM_MOVEMENT = 0,
//        PLAYER_MOVEMENT = 1,
//        WORM_HOLE = 2,
//        AIR_VENT = 3,
//        MINERALS = 4,
//        OXYGEN = 5,
//        MINING = 6,
//        LAVA = 7,
//        TNT = 8
//    };
//
//    LogicType logic_type;
//    bool is_active{ true };
//    float timer{ 0.0f };
//
//    //TODO: THINK ABOUT IT MAYBE INSTEAD OF A PAIR JUST A VECTOR OF LOGICTYPE WILL DO
//    std::vector<std::pair<bool, LogicType>> logic_container;
//
//    //Logic_Component(LogicType type) : logic_type(type) {}

//original
   //public:
    //    enum class LogicType {
    //        MOVING_PLATFORM    // Platform that moves between points
    //    };

    //    enum class MovementPattern {
    //        LINEAR = 0,            // Move back and forth in a line
    //        CIRCULAR = 1           // Move in a circular path
    //    };

    //    LogicType logic_type;
    //    MovementPattern movement_pattern;
    //    bool is_active{ true };           // Whether object is currently active/visible
    //    float timer{ 0.0f };              // For movement timing
    //    float movement_speed{ 100.0f };   // Speed of movement
    //    float movement_range{ 200.0f };   // Range of movement
    //    Vec2D origin_pos;                 // Starting/center position
    //    bool reverse_direction{ false };   // For changing direction (horizontal/vertical)
    //    bool rotate_with_motion{ false }; // Whether object rotates to face movement direction

    //    Logic_Component(LogicType type = LogicType::MOVING_PLATFORM,
    //        MovementPattern pattern = MovementPattern::LINEAR)
    //        : logic_type(type)
    //        , movement_pattern(pattern) {}

    //    // Add helper method to set movement pattern
    //    void set_movement_pattern(MovementPattern pattern) {
    //        movement_pattern = pattern;
    //        // Reset timer when changing patterns
    //        timer = 0.0f;
    //    }

 /*virtual void set_param(const std::string& script_name, const std::string& key, float value) {
            auto& script = scripts[script_name];
            scripts[script_name].float_params[key] = value;
        }

        virtual void set_param(const std::string& script_name, const std::string& key, int value) {
            auto& script = scripts[script_name];
            scripts[script_name].int_params[key] = value;
        }

        virtual void set_param(const std::string& script_name, const std::string& key, bool value) {
            auto& script = scripts[script_name];
            scripts[script_name].bool_params[key] = value;
        }

        virtual void set_param(const std::string& script_name, const std::string& key, Vec2D value) {
            auto& script = scripts[script_name];
            scripts[script_name].vec2d_params[key] = value;
        }*/


        //std::optional<float> get_float_param(const std::string& script_name, const std::string& key) const {
        //    auto it = scripts.find(script_name);
        //    if (it != scripts.end()) {
        //        auto param_it = it->second.float_params.find(key);
        //        if (param_it != it->second.float_params.end()) {
        //            return param_it->second;
        //        }
        //    }
        //    return std::nullopt; // Return empty if not found
        //}

        //std::optional<int> get_int_param(const std::string& script_name, const std::string& key) const {
        //    auto it = scripts.find(script_name);
        //    if (it != scripts.end()) {
        //        auto param_it = it->second.int_params.find(key);
        //        if (param_it != it->second.int_params.end()) {
        //            return param_it->second;
        //        }
        //    }
        //    return std::nullopt;
        //}

        //std::optional<bool> get_bool_param(const std::string& script_name, const std::string& key) const {
        //    auto it = scripts.find(script_name);
        //    if (it != scripts.end()) {
        //        auto param_it = it->second.bool_params.find(key);
        //        if (param_it != it->second.bool_params.end()) {
        //            return param_it->second;
        //        }
        //    }
        //    return std::nullopt;
        //}

        //std::optional<Vec2D> get_vec2d_param(const std::string& script_name, const std::string& key) const {
        //    auto it = scripts.find(script_name);
        //    if (it != scripts.end()) {
        //        auto param_it = it->second.vec2d_params.find(key);
        //        if (param_it != it->second.vec2d_params.end()) {
        //            return param_it->second;
        //        }
        //    }
        //    return std::nullopt;
        //}

#endif